/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file oz/Alloc.cc
 */

#include "Alloc.hh"

#include "System.hh"
#include "Log.hh"

#include <cstdlib>
#include <cstring>

#ifdef _WIN32
# include <malloc.h>
# include <windows.h>
#else
# include <pthread.h>
#endif

namespace oz
{

static_assert( ( Alloc::ALIGNMENT & ( Alloc::ALIGNMENT - 1 ) ) == 0,
               "Alloc::ALIGNMENT should be power of two" );

#ifdef OZ_TRACE_LEAKS

// Holds info about a memory allocation, used to track memory leaks and new/delete mismatches.
struct TraceEntry
{
  TraceEntry* next;
  void*       address;
  size_t      size;
  StackTrace  stackTrace;
};

static TraceEntry* firstObjectTraceEntry = null;
static TraceEntry* firstArrayTraceEntry  = null;

// If we deallocate from two different threads at once with OZ_TRACE_LEAKS, changing the list
// of allocated blocks while iterating it in another thread can result in a SIGSEGV.
#ifdef _WIN32
static CRITICAL_SECTION sectionMutex;
#else
static pthread_mutex_t sectionMutex = PTHREAD_MUTEX_INITIALIZER;
#endif

#endif

int    Alloc::count     = 0;
size_t Alloc::amount    = 0;

int    Alloc::sumCount  = 0;
size_t Alloc::sumAmount = 0;

int    Alloc::maxCount  = 0;
size_t Alloc::maxAmount = 0;

OZ_WEAK_SYMBOL
bool Alloc::isLocked = false;

void Alloc::printStatistics()
{
  log.println( "Alloc statistics {" );
  log.indent();

  log.println( "current chunks     %d", count  );
  log.println( "current amount     %.2f MiB (%d B)",
               float( amount ) / ( 1024.0f*1024.0f ), int( amount ) );
  log.println( "maximum chunks     %d", maxCount );
  log.println( "maximum amount     %.2f MiB (%d B)",
               float( maxAmount ) / ( 1024.0f*1024.0f ), int( maxAmount ) );
  log.println( "cumulative chunks  %d", sumCount );
  log.println( "cumulative amount  %.2f MiB (%d B)",
               float( sumAmount ) / ( 1024.0f*1024.0f ), int( sumAmount ) );

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
#ifdef OZ_POINTER_32
    log.println( "Leaked object at %p of size %d B allocated", bt->address, bt->size );
#else
    log.println( "Leaked object at %p of size %lld B allocated", bt->address, ulong64( bt->size ) );
#endif
    log.indent();
    log.printTrace( &bt->stackTrace );
    log.unindent();

    bt = bt->next;
  }

  bt = firstArrayTraceEntry;
  while( bt != null ) {
#ifdef OZ_POINTER_32
    log.println( "Leaked array at %p of size %d B allocated", bt->address, bt->size );
#else
    log.println( "Leaked array at %p of size %lld B allocated", bt->address, ulong64( bt->size ) );
#endif
    log.indent();
    log.printTrace( &bt->stackTrace );
    log.unindent();

    bt = bt->next;
  }
}

#endif

}

using namespace oz;

/**
 * <tt>operator new</tt> implementation with memory statistics and optionally memory alignment
 * and leak tracing.
 *
 * @ingroup oz
 */
void* operator new ( size_t size ) throw( std::bad_alloc )
{
  hard_assert( !Alloc::isLocked );

  size += Alloc::alignUp( sizeof( size_t ) );

#ifdef _WIN32
  void* ptr = _aligned_malloc( size, Alloc::ALIGNMENT );
  if( ptr == null ) {
    System::trap();
    throw std::bad_alloc();
  }
#else
  void* ptr;
  if( posix_memalign( &ptr, Alloc::ALIGNMENT, size ) != 0 ) {
    System::trap();
    throw std::bad_alloc();
  }
#endif

#ifdef OZ_TRACE_LEAKS
  TraceEntry* st = reinterpret_cast<TraceEntry*>( malloc( sizeof( TraceEntry ) ) );

# ifdef _WIN32
  InitializeCriticalSection( &sectionMutex );
  EnterCriticalSection( &sectionMutex );
# else
  pthread_mutex_lock( &sectionMutex );
# endif

  st->next       = firstObjectTraceEntry;
  st->address    = ptr;
  st->size       = size;
  st->stackTrace = StackTrace::current();

  firstObjectTraceEntry = st;

# ifdef _WIN32
  LeaveCriticalSection( &sectionMutex );
  DeleteCriticalSection( &sectionMutex );
# else
  pthread_mutex_unlock( &sectionMutex );
# endif
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

/**
 * <tt>operator new[]</tt> implementation with memory statistics and optionally memory alignment
 * and leak tracing.
 *
 * @ingroup oz
 */
void* operator new[] ( size_t size ) throw( std::bad_alloc )
{
  hard_assert( !Alloc::isLocked );

  size += Alloc::alignUp( sizeof( size_t ) );

#ifdef _WIN32
  void* ptr = _aligned_malloc( size, Alloc::ALIGNMENT );
  if( ptr == null ) {
    System::trap();
    throw std::bad_alloc();
  }
#else
  void* ptr;
  if( posix_memalign( &ptr, Alloc::ALIGNMENT, size ) != 0 ) {
    System::trap();
    throw std::bad_alloc();
  }
#endif

#ifdef OZ_TRACE_LEAKS
  TraceEntry* st = reinterpret_cast<TraceEntry*>( malloc( sizeof( TraceEntry ) ) );

# ifdef _WIN32
  InitializeCriticalSection( &sectionMutex );
  EnterCriticalSection( &sectionMutex );
# else
  pthread_mutex_lock( &sectionMutex );
# endif

  st->next       = firstArrayTraceEntry;
  st->address    = ptr;
  st->size       = size;
  st->stackTrace = StackTrace::current();

  firstArrayTraceEntry = st;

# ifdef _WIN32
  LeaveCriticalSection( &sectionMutex );
  DeleteCriticalSection( &sectionMutex );
# else
  pthread_mutex_unlock( &sectionMutex );
# endif
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

/**
 * <tt>operator delete</tt> implementation for the matching <tt>operator new</tt>.
 *
 * @ingroup oz
 */
void operator delete ( void* ptr ) throw()
{
  if( ptr == null ) {
    return;
  }

  hard_assert( !Alloc::isLocked );

  size_t size  = reinterpret_cast<size_t*>( ptr )[-1];
  char*  chunk = reinterpret_cast<char*>( ptr ) - Alloc::alignUp( sizeof( size_t ) );

  --Alloc::count;
  Alloc::amount -= size;

#ifndef NDEBUG
  memset( chunk, 0xee, size );
#endif

#ifdef OZ_TRACE_LEAKS
  System::resetSignals();

# ifdef _WIN32
  InitializeCriticalSection( &sectionMutex );
  EnterCriticalSection( &sectionMutex );
# else
  pthread_mutex_lock( &sectionMutex );
# endif

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
      free( st );

      goto backtraceFound;
    }
    prev = st;
    st = st->next;
  }

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

# ifdef _WIN32
  LeaveCriticalSection( &sectionMutex );
  DeleteCriticalSection( &sectionMutex );
# else
  pthread_mutex_unlock( &sectionMutex );
# endif
#endif

#ifdef _WIN32
  _aligned_free( chunk );
#else
  free( chunk );
#endif
}

/**
 * <tt>operator delete[]</tt> implementation for the matching <tt>operator new[]</tt>.
 *
 * @ingroup oz
 */
void operator delete[] ( void* ptr ) throw()
{
  if( ptr == null ) {
    return;
  }

  hard_assert( !Alloc::isLocked );

  size_t size  = reinterpret_cast<size_t*>( ptr )[-1];
  char*  chunk = reinterpret_cast<char*>( ptr ) - Alloc::alignUp( sizeof( size_t ) );

  --Alloc::count;
  Alloc::amount -= size;

#ifndef NDEBUG
  memset( chunk, 0xee, size );
#endif

#ifdef OZ_TRACE_LEAKS
  System::resetSignals();

# ifdef _WIN32
  InitializeCriticalSection( &sectionMutex );
  EnterCriticalSection( &sectionMutex );
# else
  pthread_mutex_lock( &sectionMutex );
# endif

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
      free( st );

      goto backtraceFound;
    }
    prev = st;
    st = st->next;
  }

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

# ifdef _WIN32
  LeaveCriticalSection( &sectionMutex );
  DeleteCriticalSection( &sectionMutex );
# else
  pthread_mutex_unlock( &sectionMutex );
# endif
#endif

#ifdef _WIN32
  _aligned_free( chunk );
#else
  free( chunk );
#endif
}
