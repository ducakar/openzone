/*
 *  Alloc.cpp
 *
 *  Overload default new and delete operators to provide allocation statistics and optionally
 *  check for leaks and mismatched new/delete (if OZ_TRACE_LEAKS is turned on).
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "Alloc.hpp"

#include "System.hpp"
#include "Log.hpp"

#include <cstdlib>
#include <cstring>

#ifdef OZ_MINGW
# include <malloc.h>
#endif

#ifdef OZ_TRACE_LEAKS
# ifndef OZ_MINGW
#  include <pthread.h>
# else
#  include <windows.h>
# endif
#endif

namespace oz
{

  static_assert( ( Alloc::ALIGNMENT & ( Alloc::ALIGNMENT - 1 ) ) == 0,
                 "Alloc::ALIGNMENT should be power of two" );

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
#ifndef OZ_MINGW
  static pthread_mutex_t sectionMutex = PTHREAD_MUTEX_INITIALIZER;
#else
# define pthread_mutex_lock( mutex ) \
  InitializeCriticalSection( mutex ); \
  EnterCriticalSection( mutex )

# define pthread_mutex_unlock( mutex ) \
  LeaveCriticalSection( mutex ); \
  DeleteCriticalSection( mutex )

  static CRITICAL_SECTION sectionMutex;
#endif

#endif

  int    Alloc::count     = 0;
  size_t Alloc::amount    = 0;

  int    Alloc::sumCount  = 0;
  size_t Alloc::sumAmount = 0;

  int    Alloc::maxCount  = 0;
  size_t Alloc::maxAmount = 0;

  OZ_WEAK_SYMBOL
  bool Alloc::isLocked  = false;

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
using oz::System;

#ifdef OZ_TRACE_LEAKS

using oz::log;
using oz::TraceEntry;
using oz::firstObjectTraceEntry;
using oz::firstArrayTraceEntry;
using oz::sectionMutex;

#endif

#ifdef OZ_MINGW

static int posix_memalign( void** ptr, size_t alignment, size_t size )
{
  void** originalPtr = reinterpret_cast<void**>( malloc( sizeof( void* ) + size + alignment - 1 ) );
  void** beginPtr = Alloc::alignUp( originalPtr + 1 );

  if( originalPtr == null ) {
    return -1;
  }

  beginPtr[-1] = originalPtr;
  *ptr = reinterpret_cast<void*>( beginPtr );

  return 0;
}

static void posix_memalign_free( void* ptr )
{
  void** beginPtr = reinterpret_cast<void**>( ptr );

  free( beginPtr[-1] );
}

#else

# define posix_memalign_free( ptr ) free( ptr )

#endif

void* operator new ( size_t size ) throw( std::bad_alloc )
{
  hard_assert( !Alloc::isLocked );
  hard_assert( size != 0 );

  size += Alloc::alignUp( sizeof( size_t ) );

  void* ptr;
  if( posix_memalign( &ptr, Alloc::ALIGNMENT, size ) ) {
    System::trap();
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

  ptr = reinterpret_cast<char*>( ptr ) + Alloc::alignUp( sizeof( size_t ) );
  reinterpret_cast<size_t*>( ptr )[-1] = size;

  return ptr;
}

void* operator new[] ( size_t size ) throw( std::bad_alloc )
{
  hard_assert( !Alloc::isLocked );
  hard_assert( size != 0 );

  size += Alloc::alignUp( sizeof( size_t ) );

  void* ptr;
  if( posix_memalign( &ptr, Alloc::ALIGNMENT, size ) ) {
    System::trap();
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

  ptr = reinterpret_cast<char*>( ptr ) + Alloc::alignUp( sizeof( size_t ) );
  reinterpret_cast<size_t*>( ptr )[-1] = size;

  return ptr;
}

void operator delete ( void* ptr ) throw()
{
  hard_assert( !Alloc::isLocked );

  if( ptr == null ) {
    return;
  }

  size_t size  = reinterpret_cast<size_t*>( ptr )[-1];
  char*  chunk = reinterpret_cast<char*>( ptr ) - Alloc::alignUp( sizeof( size_t ) );

  --Alloc::count;
  Alloc::amount -= size;

#ifndef NDEBUG
  memset( chunk, 0xee, size );
#endif

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

  posix_memalign_free( chunk );
}

void operator delete[] ( void* ptr ) throw()
{
  hard_assert( !Alloc::isLocked );

  if( ptr == null ) {
    return;
  }

  size_t size  = reinterpret_cast<size_t*>( ptr )[-1];
  char*  chunk = reinterpret_cast<char*>( ptr ) - Alloc::alignUp( sizeof( size_t ) );

  --Alloc::count;
  Alloc::amount -= size;

#ifndef NDEBUG
  memset( chunk, 0xee, size );
#endif

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

  posix_memalign_free( chunk );
}
