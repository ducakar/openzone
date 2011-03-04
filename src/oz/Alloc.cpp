/*
 *  Alloc.cpp
 *
 *  Overload default new and delete operators for slightly better performance (ifndef OZ_ALLOC) or
 *  provide heap allocation statistics (ifdef OZ_ALLOC).
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "Alloc.hpp"

#include "System.hpp"
#include "Log.hpp"

#include <cstdlib>

#ifdef OZ_TRACE_LEAKS
# include <pthread.h>
#endif

namespace oz
{

  static_assert( ( Alloc::ALIGNMENT & ( Alloc::ALIGNMENT - 1 ) ) == 0,
                 "Alloc::ALIGNMENT should be power of two" );

#ifdef OZ_ALLOC_STATISTICS
  static_assert( sizeof( size_t ) <= size_t( Alloc::ALIGNMENT ),
                 "Alloc::ALIGNEMENT should not be less than sizeof( size_t ) when using memory "
                 "allocation statistics." );
#endif

#ifdef OZ_TRACE_LEAKS

  struct TraceEntry
  {
    TraceEntry* next;
    void*       address;
    size_t      size;
    int         nFrames;
    char*       frames;
  };

  static TraceEntry* firstObjectTraceEntry = null;
  static TraceEntry* firstArrayTraceEntry  = null;

  // if we deallocate from two different threads at once with OZ_TRACE_LEAKS, changing the list
  // of allocated blocks while iterating it in another thread can result in a SIGSEGV.
  static pthread_mutex_t sectionMutex = PTHREAD_MUTEX_INITIALIZER;

#endif

  int  Alloc::count     = 0;
  long Alloc::amount    = 0;

  int  Alloc::sumCount  = 0;
  long Alloc::sumAmount = 0;

  int  Alloc::maxCount  = 0;
  long Alloc::maxAmount = 0;

  OZ_WEAK_SYMBOL
  bool Alloc::isLocked  = false;

#ifndef OZ_ALLOC_STATISTICS

  void Alloc::printStatistics()
  {}

#else

  void Alloc::printStatistics()
  {
    log.println( "Alloc statistics {" );
    log.indent();

    log.println( "current chunks     %d", Alloc::count  );
    log.println( "current amount     %.2f MiB (%d B)",
                 float( Alloc::amount ) / ( 1024.0f*1024.0f ), Alloc::amount );
    log.println( "maximum chunks     %d", Alloc::maxCount );
    log.println( "maximum amount     %.2f MiB (%d B)",
                 float( Alloc::maxAmount ) / ( 1024.0f*1024.0f ), Alloc::maxAmount );
    log.println( "cumulative chunks  %d", Alloc::sumCount );
    log.println( "cumulative amount  %.2f MiB (%d B)",
                 float( Alloc::sumAmount ) / ( 1024.0f*1024.0f ), Alloc::sumAmount );

    log.unindent();
    log.println( "}" );
  }

#endif

#ifndef OZ_TRACE_LEAKS

  void Alloc::printLeaks()
  {}

#else

  void Alloc::printLeaks()
  {
    const TraceEntry* bt;

    bt = firstObjectTraceEntry;
    while( bt != null ) {
      log.println( "Leaked object at %p of size %d B allocated", bt->address, bt->size );
      log.indent();
      log.printTrace( bt->frames, bt->nFrames );
      log.unindent();

      bt = bt->next;
    }

    bt = firstArrayTraceEntry;
    while( bt != null ) {
      log.println( "Leaked array at %p of size %d B allocated", bt->address, bt->size );
      log.indent();
      log.printTrace( bt->frames, bt->nFrames );
      log.unindent();

      bt = bt->next;
    }
  }

#endif

}

using oz::max;
using oz::Alloc;

#ifdef OZ_TRACE_LEAKS

using oz::log;
using oz::System;
using oz::TraceEntry;
using oz::firstObjectTraceEntry;
using oz::firstArrayTraceEntry;
using oz::sectionMutex;

#endif

#ifndef OZ_ALLOC_STATISTICS

void* operator new ( size_t size ) throw( std::bad_alloc )
{
  hard_assert( !Alloc::isLocked );
  hard_assert( size != 0 );

  void* ptr;
  if( posix_memalign( &ptr, Alloc::ALIGNMENT, size ) ) {
    throw std::bad_alloc();
  }

#ifdef OZ_TRACE_LEAKS
  TraceEntry* st = reinterpret_cast<TraceEntry*>( malloc( sizeof( TraceEntry ) ) );

  pthread_mutex_lock( &sectionMutex );

  st->next    = firstObjectTraceEntry;
  st->address = ptr;
  st->size    = size;
  st->nFrames = System::getStackTrace( &st->frames );

  firstObjectTraceEntry = st;

  pthread_mutex_unlock( &sectionMutex );
#endif

  return ptr;
}

void* operator new[] ( size_t size ) throw( std::bad_alloc )
{
  hard_assert( !Alloc::isLocked );
  hard_assert( size != 0 );

  void* ptr;
  if( posix_memalign( &ptr, Alloc::ALIGNMENT, size ) ) {
    throw std::bad_alloc();
  }

#ifdef OZ_TRACE_LEAKS
  TraceEntry* st = reinterpret_cast<TraceEntry*>( malloc( sizeof( TraceEntry ) ) );

  pthread_mutex_lock( &sectionMutex );

  st->next    = firstArrayTraceEntry;
  st->address = ptr;
  st->size    = size;
  st->nFrames = System::getStackTrace( &st->frames );

  firstArrayTraceEntry = st;

  pthread_mutex_unlock( &sectionMutex );
#endif

  return ptr;
}

void operator delete ( void* ptr ) throw()
{
  hard_assert( !Alloc::isLocked );
  hard_assert( ptr != null );

#ifdef OZ_TRACE_LEAKS
  System::resetSignals();

  pthread_mutex_lock( &sectionMutex );

  TraceEntry* st   = firstObjectTraceEntry;
  TraceEntry* prev = null;

  while( st != null ) {
    if( st->address == ptr ) {
      if( prev == null ) {
        firstObjectTraceEntry = st->next;
      }
      else {
        prev->next = st->next;
      }
      free( st->frames );
      free( st );

      goto backtraceFound;
    }
    prev = st;
    st = st->next;
  }
  // loop fell through
  System::trap();

  st   = firstArrayTraceEntry;
  prev = null;

  while( st != null ) {
    if( st->address == ptr ) {
      System::abort( "ALLOC: new[] -> delete mismatch for block at %p", chunk );
      break;
    }
    prev = st;
    st = st->next;
  }

  System::abort( "ALLOC: Trying to free object at %p that does not seem to be allocated", chunk );

  backtraceFound:;
  pthread_mutex_unlock( &sectionMutex );
#endif

  free( ptr );
}

void operator delete[] ( void* ptr ) throw()
{
  hard_assert( !Alloc::isLocked );
  hard_assert( ptr != null );

#ifdef OZ_TRACE_LEAKS
  System::resetSignals();

  pthread_mutex_lock( &sectionMutex );

  TraceEntry* st   = firstArrayTraceEntry;
  TraceEntry* prev = null;

  while( st != null ) {
    if( st->address == ptr ) {
      if( prev == null ) {
        firstArrayTraceEntry = st->next;
      }
      else {
        prev->next = st->next;
      }
      free( st->frames );
      free( st );

      goto backtraceFound;
    }
    prev = st;
    st = st->next;
  }
  // loop fell through
  System::trap();

  st   = firstObjectTraceEntry;
  prev = null;

  while( st != null ) {
    if( st->address == ptr ) {
      System::abort( "ALLOC: new -> delete[] mismatch for block at %p", chunk );
      break;
    }
    prev = st;
    st = st->next;
  }

  System::abort( "ALLOC: Trying to free array at %p that does not seem to be allocated", chunk );

  backtraceFound:;
  pthread_mutex_unlock( &sectionMutex );
#endif

  free( ptr );
}

#else

#ifdef OZ_MSVC
void* operator new ( size_t size )
#else
void* operator new ( size_t size ) throw( std::bad_alloc )
#endif
{
  hard_assert( !Alloc::isLocked );
  hard_assert( size != 0 );

  size += Alloc::ALIGNMENT;

  void* ptr;
  if( posix_memalign( &ptr, Alloc::ALIGNMENT, size ) ) {
    throw std::bad_alloc();
  }

#ifdef OZ_TRACE_LEAKS
  TraceEntry* st = reinterpret_cast<TraceEntry*>( malloc( sizeof( TraceEntry ) ) );

  pthread_mutex_lock( &sectionMutex );

  st->next    = firstObjectTraceEntry;
  st->address = ptr;
  st->size    = size;
  st->nFrames = System::getStackTrace( &st->frames );

  firstObjectTraceEntry = st;

  pthread_mutex_unlock( &sectionMutex );
#endif

  ++Alloc::count;
  Alloc::amount += size;

  ++Alloc::sumCount;
  Alloc::sumAmount += size;

  Alloc::maxCount = max( Alloc::count, Alloc::maxCount );
  Alloc::maxAmount = max( Alloc::amount, Alloc::maxAmount );

  reinterpret_cast<size_t*>( ptr )[0] = size;
  return reinterpret_cast<char*>( ptr ) + Alloc::ALIGNMENT;
}

#ifdef OZ_MSVC
void* operator new[] ( size_t size )
#else
void* operator new[] ( size_t size ) throw( std::bad_alloc )
#endif
{
  hard_assert( !Alloc::isLocked );
  hard_assert( size != 0 );

  size += Alloc::ALIGNMENT;

  void* ptr;
  if( posix_memalign( &ptr, Alloc::ALIGNMENT, size ) ) {
    throw std::bad_alloc();
  }

#ifdef OZ_TRACE_LEAKS
  TraceEntry* st = reinterpret_cast<TraceEntry*>( malloc( sizeof( TraceEntry ) ) );

  pthread_mutex_lock( &sectionMutex );

  st->next    = firstArrayTraceEntry;
  st->address = ptr;
  st->size    = size;
  st->nFrames = System::getStackTrace( &st->frames );

  firstArrayTraceEntry = st;

  pthread_mutex_unlock( &sectionMutex );
#endif

  ++Alloc::count;
  Alloc::amount += size;

  ++Alloc::sumCount;
  Alloc::sumAmount += size;

  Alloc::maxCount = max( Alloc::count, Alloc::maxCount );
  Alloc::maxAmount = max( Alloc::amount, Alloc::maxAmount );

  reinterpret_cast<size_t*>( ptr )[0] = size;
  return reinterpret_cast<char*>( ptr ) + Alloc::ALIGNMENT;
}

void operator delete ( void* ptr ) throw()
{
  hard_assert( !Alloc::isLocked );
  hard_assert( ptr != null );

  char*  chunk = reinterpret_cast<char*>( ptr ) - Alloc::ALIGNMENT;
  size_t size  = reinterpret_cast<size_t*>( chunk )[0];

#ifdef OZ_TRACE_LEAKS
  System::resetSignals();

  pthread_mutex_lock( &sectionMutex );

  TraceEntry* st   = firstObjectTraceEntry;
  TraceEntry* prev = null;

  while( st != null ) {
    if( st->address == chunk ) {
      hard_assert( st->size == size );

      if( prev == null ) {
        firstObjectTraceEntry = st->next;
      }
      else {
        prev->next = st->next;
      }
      free( st->frames );
      free( st );

      goto backtraceFound;
    }
    prev = st;
    st = st->next;
  }
  // loop fell through
  System::trap();

  st   = firstArrayTraceEntry;
  prev = null;

  while( st != null ) {
    if( st->address == chunk ) {
      System::abort( "ALLOC: new[] -> delete mismatch for block at %p", chunk );
      break;
    }
    prev = st;
    st = st->next;
  }

  System::abort( "ALLOC: Trying to free object at %p that does not seem to be allocated", chunk );

  backtraceFound:;
  pthread_mutex_unlock( &sectionMutex );
#endif

  --Alloc::count;
  Alloc::amount -= size;

  free( chunk );
}

void operator delete[] ( void* ptr ) throw()
{
  hard_assert( !Alloc::isLocked );
  hard_assert( ptr != null );

  char*  chunk = reinterpret_cast<char*>( ptr ) - Alloc::ALIGNMENT;
  size_t size  = reinterpret_cast<size_t*>( chunk )[0];

#ifdef OZ_TRACE_LEAKS
  System::resetSignals();

  pthread_mutex_lock( &sectionMutex );

  TraceEntry* st   = firstArrayTraceEntry;
  TraceEntry* prev = null;

  while( st != null ) {
    if( st->address == chunk ) {
      hard_assert( st->size == size );

      if( prev == null ) {
        firstArrayTraceEntry = st->next;
      }
      else {
        prev->next = st->next;
      }
      free( st->frames );
      free( st );

      goto backtraceFound;
    }
    prev = st;
    st = st->next;
  }
  // loop fell through
  System::trap();

  st   = firstObjectTraceEntry;
  prev = null;

  while( st != null ) {
    if( st->address == chunk ) {
      System::abort( "ALLOC: new -> delete[] mismatch for block at %p", chunk );
    }
    prev = st;
    st = st->next;
  }

  System::abort( "ALLOC: Trying to free array at %p that does not seem to be allocated", chunk );

  backtraceFound:;
  pthread_mutex_unlock( &sectionMutex );
#endif

  --Alloc::count;
  Alloc::amount -= size;

  free( chunk );
}

#endif
