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

#include "StackTrace.hpp"
#include "Log.hpp"

#include <cstring>
#include <cstdlib>
#include <cstdio>

namespace oz
{

  static_assert( ( Alloc::ALIGNMENT & ( Alloc::ALIGNMENT - 1 ) ) == 0,
                 "Alloc::ALIGNMENT should be power of two" );

  int  Alloc::count     = 0;
  long Alloc::amount    = 0;

  int  Alloc::sumCount  = 0;
  long Alloc::sumAmount = 0;

  int  Alloc::maxCount  = 0;
  long Alloc::maxAmount = 0;

#ifndef OZ_ALLOC_TRACELEAKS

  void Alloc::dumpLeaks()
  {}

#else

  struct TraceEntry
  {
    TraceEntry* next;
    void*       address;
    int         size;
    int         nFrames;
    char*       frames;
  };

  TraceEntry*   firstObjectTraceEntry = null;
  TraceEntry*   firstArrayTraceEntry  = null;

  void Alloc::dumpLeaks()
  {
    TraceEntry* bt;
    TraceEntry* next;

    bt = firstObjectTraceEntry;
    while( bt != null ) {
      printf( "Leaked object at %p of size %d B allocated in\n", bt->address, bt->size );

      log.indent();
      log.printTrace( bt->frames, bt->nFrames );
      log.unindent();

      next = bt->next;
      free( bt->frames );
      free( bt );
      bt = next;
    }
    firstObjectTraceEntry = null;

    bt = firstArrayTraceEntry;
    while( bt != null ) {
      printf( "Leaked array at %p of size %d B allocated in\n", bt->address, bt->size );

      log.indent();
      log.printTrace( bt->frames, bt->nFrames );
      log.unindent();

      next = bt->next;
      free( bt->frames );
      free( bt );
      bt = next;
    }
    firstArrayTraceEntry = null;
  }

#endif

}

using oz::max;
using oz::Alloc;

#ifdef OZ_ALLOC_TRACELEAKS

using oz::StackTrace;
using oz::TraceEntry;
using oz::firstObjectTraceEntry;
using oz::firstArrayTraceEntry;

#endif

#ifndef OZ_ALLOC_STATISTICS

void* operator new ( size_t size ) throw( std::bad_alloc )
{
  void* ptr;
  if( posix_memalign( &ptr, Alloc::ALIGNMENT, size ) ) {
    throw std::bad_alloc();
  }

#ifdef OZ_ALLOC_TRACELEAKS
  TraceEntry* st = reinterpret_cast<TraceEntry*>( malloc( sizeof( TraceEntry ) ) );

  st->next    = firstObjectTraceEntry;
  st->address = ptr;
  st->size    = size;
  st->nFrames = StackTrace::get( &st->frames );

  firstObjectTraceEntry = st;
#endif

  return ptr;
}

void* operator new[] ( size_t size ) throw( std::bad_alloc )
{
  void* ptr;
  if( posix_memalign( &ptr, Alloc::ALIGNMENT, size ) ) {
    throw std::bad_alloc();
  }

#ifdef OZ_ALLOC_TRACELEAKS
  TraceEntry* st = reinterpret_cast<TraceEntry*>( malloc( sizeof( TraceEntry ) ) );

  st->next    = firstArrayTraceEntry;
  st->address = ptr;
  st->size    = size;
  st->nFrames = StackTrace::get( &st->frames );

  firstArrayTraceEntry = st;
#endif

  return ptr;
}

void operator delete ( void* ptr ) throw()
{
#ifdef OZ_ALLOC_TRACELEAKS
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
  fprintf( stderr, "ALLOC: Trying to free object at %p that was not found on the list of "
           "allocated objects\n", ptr );

  st   = firstArrayTraceEntry;
  prev = null;

  while( st != null ) {
    if( st->address == ptr ) {
      fprintf( stderr, "However, it was found on the list of allocated arrays "
               "(new -> delete[] mismatch)\n" );
      break;
    }
    prev = st;
    st = st->next;
  }

  abort();

  backtraceFound:;
#endif

  free( ptr );
}

void operator delete[] ( void* ptr ) throw()
{
#ifdef OZ_ALLOC_TRACELEAKS
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
  fprintf( stderr, "ALLOC: Trying to free array at %p that was not found on the list of "
           "allocated arrays\n", ptr );

  st   = firstObjectTraceEntry;
  prev = null;

  while( st != null ) {
    if( st->address == ptr ) {
      fprintf( stderr, "However, it was found on the list of allocated objects "
               "(new[] -> delete mismatch)\n" );
      break;
    }
    prev = st;
    st = st->next;
  }

  abort();

  backtraceFound:;
#endif

  free( ptr );
}

#else

static_assert( sizeof( size_t ) <= size_t( Alloc::ALIGNMENT ),
               "Alloc::ALIGNEMENT should not be less than sizeof( size_t ) when using memory "
               "allocation statistics." );

#ifdef OZ_MSVC
void* operator new ( size_t size )
#else
void* operator new ( size_t size ) throw( std::bad_alloc )
#endif
{
  size += Alloc::ALIGNMENT;

  void* ptr;
  if( posix_memalign( &ptr, Alloc::ALIGNMENT, size ) ) {
    throw std::bad_alloc();
  }

#ifdef OZ_ALLOC_TRACELEAKS
  TraceEntry* st = reinterpret_cast<TraceEntry*>( malloc( sizeof( TraceEntry ) ) );

  st->next    = firstObjectTraceEntry;
  st->address = ptr;
  st->size    = size;
  st->nFrames = StackTrace::get( &st->frames );

  firstObjectTraceEntry = st;
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
  size += Alloc::ALIGNMENT;

  void* ptr;
  if( posix_memalign( &ptr, Alloc::ALIGNMENT, size ) ) {
    throw std::bad_alloc();
  }

#ifdef OZ_ALLOC_TRACELEAKS
  TraceEntry* st = reinterpret_cast<TraceEntry*>( malloc( sizeof( TraceEntry ) ) );

  st->next    = firstArrayTraceEntry;
  st->address = ptr;
  st->nFrames = StackTrace::get( &st->frames );

  firstArrayTraceEntry = st;
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
  assert( ptr != null );

  char* chunk = reinterpret_cast<char*>( ptr ) - Alloc::ALIGNMENT;

#ifdef OZ_ALLOC_TRACELEAKS
  TraceEntry* st   = firstObjectTraceEntry;
  TraceEntry* prev = null;

  while( st != null ) {
    if( st->address == chunk ) {
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
  fprintf( stderr, "ALLOC: Trying to free object at %p that was not found on the list of "
           "allocated objects\n", chunk );

  st   = firstArrayTraceEntry;
  prev = null;

  while( st != null ) {
    if( st->address == chunk ) {
      fprintf( stderr, "However, it was found on the list of allocated arrays "
               "(new -> delete[] mismatch)\n" );
      break;
    }
    prev = st;
    st = st->next;
  }

  abort();

  backtraceFound:;
#endif

  size_t size = reinterpret_cast<size_t*>( chunk )[0];

  --Alloc::count;
  Alloc::amount -= size;

  free( chunk );
}

void operator delete[] ( void* ptr ) throw()
{
  assert( ptr != null );

  char* chunk = reinterpret_cast<char*>( ptr ) - Alloc::ALIGNMENT;

#ifdef OZ_ALLOC_TRACELEAKS
  TraceEntry* st   = firstArrayTraceEntry;
  TraceEntry* prev = null;

  while( st != null ) {
    if( st->address == chunk ) {
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
  fprintf( stderr, "ALLOC: Trying to free array at %p that was not found on the list of "
           "allocated arrays\n", chunk );

  st   = firstObjectTraceEntry;
  prev = null;

  while( st != null ) {
    if( st->address == chunk ) {
      fprintf( stderr, "However, it was found on the list of allocated objects "
               "(new[] -> delete mismatch)\n" );
      break;
    }
    prev = st;
    st = st->next;
  }

  abort();

  backtraceFound:;
#endif

  size_t size = reinterpret_cast<size_t*>( chunk )[0];

  --Alloc::count;
  Alloc::amount -= size;

  free( chunk );
}

#endif
