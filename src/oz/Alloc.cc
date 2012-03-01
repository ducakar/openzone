/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file oz/Alloc.cc
 */

#include "Alloc.hh"

#include "System.hh"
#include "Log.hh"

#include "windefs.h"
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

#ifdef OZ_TRACK_LEAKS

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

// If we deallocate from two different threads at once with OZ_TRACK_LEAKS, changing the list
// of allocated blocks while iterating it in another thread can result in a SIGSEGV.

#ifdef _WIN32

struct CriticalSectionWrapper
{
  CRITICAL_SECTION id;

  CriticalSectionWrapper()
  {
    InitializeCriticalSection( &id );
  }

  ~CriticalSectionWrapper()
  {
    DeleteCriticalSection( &id );
  }
};

static CriticalSectionWrapper mutex;

#else

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#endif

#endif

int    Alloc::count     = 0;
size_t Alloc::amount    = 0;

int    Alloc::sumCount  = 0;
size_t Alloc::sumAmount = 0;

int    Alloc::maxCount  = 0;
size_t Alloc::maxAmount = 0;

void Alloc::printSummary()
{
  log.println( "Alloc summary {" );
  log.indent();

  log.println( "current chunks     %d", count );
  log.println( "current amount     %.2f MiB (%ld B)",
               float( amount ) / ( 1024.0f*1024.0f ), ulong( amount ) );
  log.println( "maximum chunks     %d", maxCount );
  log.println( "maximum amount     %.2f MiB (%ld B)",
               float( maxAmount ) / ( 1024.0f*1024.0f ), ulong( maxAmount ) );
  log.println( "cumulative chunks  %d", sumCount );
  log.println( "cumulative amount  %.2f MiB (%ld B)",
               float( sumAmount ) / ( 1024.0f*1024.0f ), ulong( sumAmount ) );

  log.unindent();
  log.println( "}" );
}

#ifndef OZ_TRACK_LEAKS

void Alloc::printLeaks()
{}

#else

void Alloc::printLeaks()
{
  const TraceEntry* bt;

  bt = firstObjectTraceEntry;
  while( bt != null ) {
    log.println( "Leaked object at %p of size %d B allocated", bt->address, int( bt->size ) );
    log.indent();
    log.printTrace( &bt->stackTrace );
    log.unindent();

    bt = bt->next;
  }

  bt = firstArrayTraceEntry;
  while( bt != null ) {
    log.println( "Leaked array at %p of size %d B allocated", bt->address, int( bt->size ) );
    log.indent();
    log.printTrace( &bt->stackTrace );
    log.unindent();

    bt = bt->next;
  }
}

#endif

static void* allocateObject( void* ptr, size_t size )
{
#ifdef OZ_TRACK_LEAKS
  TraceEntry* st = reinterpret_cast<TraceEntry*>( malloc( sizeof( TraceEntry ) ) );

# ifdef _WIN32
  EnterCriticalSection( &mutex.id );
# else
  pthread_mutex_lock( &mutex );
# endif

  st->next       = firstObjectTraceEntry;
  st->address    = ptr;
  st->size       = size;
  st->stackTrace = StackTrace::current( 1 );

  firstObjectTraceEntry = st;

# ifdef _WIN32
  LeaveCriticalSection( &mutex.id );
# else
  pthread_mutex_unlock( &mutex );
# endif
#endif

  ++Alloc::count;
  Alloc::amount += size;

  ++Alloc::sumCount;
  Alloc::sumAmount += size;

  Alloc::maxCount = max<int>( Alloc::count, Alloc::maxCount );
  Alloc::maxAmount = max<size_t>( Alloc::amount, Alloc::maxAmount );

  ptr = reinterpret_cast<char*>( ptr ) + Alloc::alignUp( sizeof( size_t ) );
  reinterpret_cast<size_t*>( ptr )[-1] = size;

  return ptr;
}

static void* allocateArray( void* ptr, size_t size )
{
#ifdef OZ_TRACK_LEAKS
  TraceEntry* st = reinterpret_cast<TraceEntry*>( malloc( sizeof( TraceEntry ) ) );

# ifdef _WIN32
  EnterCriticalSection( &mutex.id );
# else
  pthread_mutex_lock( &mutex );
# endif

  st->next       = firstArrayTraceEntry;
  st->address    = ptr;
  st->size       = size;
  st->stackTrace = StackTrace::current( 1 );

  firstArrayTraceEntry = st;

# ifdef _WIN32
  LeaveCriticalSection( &mutex.id );
# else
  pthread_mutex_unlock( &mutex );
# endif
#endif

  ++Alloc::count;
  Alloc::amount += size;

  ++Alloc::sumCount;
  Alloc::sumAmount += size;

  Alloc::maxCount = max<int>( Alloc::count, Alloc::maxCount );
  Alloc::maxAmount = max<size_t>( Alloc::amount, Alloc::maxAmount );

  ptr = reinterpret_cast<char*>( ptr ) + Alloc::alignUp( sizeof( size_t ) );
  reinterpret_cast<size_t*>( ptr )[-1] = size;

  return ptr;
}

static void deallocateObject( void* ptr )
{
  size_t size = reinterpret_cast<size_t*>( ptr )[-1];
  ptr = reinterpret_cast<char*>( ptr ) - Alloc::alignUp( sizeof( size_t ) );

  --Alloc::count;
  Alloc::amount -= size;

#ifndef NDEBUG
  memset( ptr, 0xee, size );
#endif

#ifdef OZ_TRACK_LEAKS
# ifdef _WIN32
  EnterCriticalSection( &mutex.id );
# else
  pthread_mutex_lock( &mutex );
# endif

  TraceEntry* st   = firstObjectTraceEntry;
  TraceEntry* prev = null;

  while( st != null ) {
    if( st->address == ptr ) {
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

# ifdef _WIN32
  LeaveCriticalSection( &mutex.id );
# else
  pthread_mutex_unlock( &mutex );
# endif

  // Check if allocated as an array.
  st = firstArrayTraceEntry;

  while( st != null ) {
    if( st->address == ptr ) {
      break;
    }
    st = st->next;
  }

  if( st == null ) {
    System::error( 1, "ALLOC: Trying to free object at %p that has not been allocated", ptr );
  }
  else {
    System::error( 1, "ALLOC: new[] -> delete mismatch for block at %p", ptr );
  }

backtraceFound:;

# ifdef _WIN32
  LeaveCriticalSection( &mutex.id );
# else
  pthread_mutex_unlock( &mutex );
# endif
#endif

#ifdef _WIN32
  _aligned_free( ptr );
#else
  free( ptr );
#endif
}

static void deallocateArray( void* ptr )
{
  size_t size = reinterpret_cast<size_t*>( ptr )[-1];
  ptr = reinterpret_cast<char*>( ptr ) - Alloc::alignUp( sizeof( size_t ) );

  --Alloc::count;
  Alloc::amount -= size;

#ifndef NDEBUG
  memset( ptr, 0xee, size );
#endif

#ifdef OZ_TRACK_LEAKS
# ifdef _WIN32
  EnterCriticalSection( &mutex.id );
# else
  pthread_mutex_lock( &mutex );
# endif

  TraceEntry* st   = firstArrayTraceEntry;
  TraceEntry* prev = null;

  while( st != null ) {
    if( st->address == ptr ) {
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

# ifdef _WIN32
  LeaveCriticalSection( &mutex.id );
# else
  pthread_mutex_unlock( &mutex );
# endif

  // Check if allocated as an object.
  st = firstObjectTraceEntry;

  while( st != null ) {
    if( st->address == ptr ) {
      break;
    }
    st = st->next;
  }

  if( st == null ) {
    System::error( 1, "ALLOC: Trying to free array at %p that has not been allocated", ptr );
  }
  else {
    System::error( 1, "ALLOC: new -> delete[] mismatch for block at %p", ptr );
  }

backtraceFound:;

# ifdef _WIN32
  LeaveCriticalSection( &mutex.id );
# else
  pthread_mutex_unlock( &mutex );
# endif
#endif

#ifdef _WIN32
  _aligned_free( ptr );
#else
  free( ptr );
#endif
}

}

using namespace oz;

void* operator new ( std::size_t size ) throw( std::bad_alloc )
{
  size += Alloc::alignUp( sizeof( size_t ) );

#if defined( __ANDROID__ )
  void* ptr = malloc( size );
  if( ptr == null ) {
#elif defined( _WIN32 )
  void* ptr = _aligned_malloc( size, Alloc::ALIGNMENT );
  if( ptr == null ) {
#else
  void* ptr;
  if( posix_memalign( &ptr, Alloc::ALIGNMENT, size ) != 0 ) {
#endif
    System::trap();
    throw std::bad_alloc();
  }

  return allocateObject( ptr, size );
}

void* operator new[] ( std::size_t size ) throw( std::bad_alloc )
{
  size += Alloc::alignUp( sizeof( size_t ) );

#if defined( __ANDROID__ )
  void* ptr = malloc( size );
  if( ptr == null ) {
#elif defined( _WIN32 )
  void* ptr = _aligned_malloc( size, Alloc::ALIGNMENT );
  if( ptr == null ) {
#else
  void* ptr;
  if( posix_memalign( &ptr, Alloc::ALIGNMENT, size ) != 0 ) {
#endif
    System::trap();
    throw std::bad_alloc();
  }

  return allocateArray( ptr, size );
}

void operator delete ( void* ptr ) throw()
{
  if( ptr == null ) {
    return;
  }

  deallocateObject( ptr );
}

void operator delete[] ( void* ptr ) throw()
{
  if( ptr == null ) {
    return;
  }

  deallocateArray( ptr );
}

void* operator new ( std::size_t size, const std::nothrow_t& ) throw()
{
  size += Alloc::alignUp( sizeof( size_t ) );

#if defined( __ANDROID__ )
  void* ptr = malloc( size );
  if( ptr == null ) {
#elif defined( _WIN32 )
  void* ptr = _aligned_malloc( size, Alloc::ALIGNMENT );
  if( ptr == null ) {
#else
  void* ptr;
  if( posix_memalign( &ptr, Alloc::ALIGNMENT, size ) != 0 ) {
#endif
    System::trap();
    return null;
  }

  return allocateObject( ptr, size );
}

void* operator new[] ( std::size_t size, const std::nothrow_t& ) throw()
{
  size += Alloc::alignUp( sizeof( size_t ) );

#if defined( __ANDROID__ )
  void* ptr = malloc( size );
  if( ptr == null ) {
#elif defined( _WIN32 )
  void* ptr = _aligned_malloc( size, Alloc::ALIGNMENT );
  if( ptr == null ) {
#else
  void* ptr;
  if( posix_memalign( &ptr, Alloc::ALIGNMENT, size ) != 0 ) {
#endif
    System::trap();
    return null;
  }

  return allocateArray( ptr, size );
}

void operator delete ( void* ptr, const std::nothrow_t& ) throw()
{
  if( ptr == null ) {
    return;
  }

  deallocateObject( ptr );
}

void operator delete[] ( void* ptr, const std::nothrow_t& ) throw()
{
  if( ptr == null ) {
    return;
  }

  deallocateArray( ptr );
}
