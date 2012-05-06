/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
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
// If we deallocate from two different threads at once with OZ_TRACK_LEAKS, changing the list of
// allocated blocks while iterating it in another thread can result in a SIGSEGV, so list operations
// must be protected my a spin lock.
struct TraceEntry
{
  TraceEntry* next;
  void*       address;
  size_t      size;
  StackTrace  stackTrace;
};

static bool                 isConstructed         = false;
static TraceEntry* volatile firstObjectTraceEntry = null;
static TraceEntry* volatile firstArrayTraceEntry  = null;
# if defined( __native_client__ )
static pthread_mutex_t      traceEntryListLock;
# elif defined( _WIN32 )
static CRITICAL_SECTION     traceEntryListLock;
# else
static pthread_spinlock_t   traceEntryListLock;
# endif

#endif

int    Alloc::count     = 0;
size_t Alloc::amount    = 0;

int    Alloc::sumCount  = 0;
size_t Alloc::sumAmount = 0;

int    Alloc::maxCount  = 0;
size_t Alloc::maxAmount = 0;

void Alloc::printSummary()
{
  Log::println( "Alloc summary {" );
  Log::indent();

  Log::println( "current chunks     %d", count );
  Log::println( "current amount     %.2f MiB (%ld B)",
                float( amount ) / ( 1024.0f*1024.0f ), ulong( amount ) );
  Log::println( "maximum chunks     %d", maxCount );
  Log::println( "maximum amount     %.2f MiB (%ld B)",
                float( maxAmount ) / ( 1024.0f*1024.0f ), ulong( maxAmount ) );
  Log::println( "cumulative chunks  %d", sumCount );
  Log::println( "cumulative amount  %.2f MiB (%ld B)",
                float( sumAmount ) / ( 1024.0f*1024.0f ), ulong( sumAmount ) );

  Log::unindent();
  Log::println( "}" );
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
    Log::println( "Leaked object at %p of size %d B allocated", bt->address, int( bt->size ) );
    Log::indent();
    Log::printTrace( &bt->stackTrace );
    Log::unindent();

    bt = bt->next;
  }

  bt = firstArrayTraceEntry;
  while( bt != null ) {
    Log::println( "Leaked array at %p of size %d B allocated", bt->address, int( bt->size ) );
    Log::indent();
    Log::printTrace( &bt->stackTrace );
    Log::unindent();

    bt = bt->next;
  }
}

#endif

static void* aligned_malloc( size_t size )
{
#if defined( __native_client__ ) || defined( __ANDROID__ )

  size += Alloc::alignUp( sizeof( void* ) );

  void* ptr = malloc( size );
  if( ptr == null ) {
    return null;
  }

  void* begin = Alloc::alignUp<char>( static_cast<char*>( ptr ) + sizeof( void* ) );
  static_cast<void**>( begin )[-1] = ptr;

  return begin;

#elif defined( _WIN32 )

  return _aligned_malloc( size, Alloc::ALIGNMENT );

#else

  void* ptr;
  if( posix_memalign( &ptr, Alloc::ALIGNMENT, size ) != 0 ) {
    return null;
  }
  return ptr;

#endif
}

static void aligned_free( void* ptr )
{
#if defined( __native_client__ ) || defined( __ANDROID__ )

  ptr = static_cast<void**>( ptr )[-1];
  free( ptr );

#elif defined( _WIN32 )

  _aligned_free( ptr );

#else

  free( ptr );

#endif
}

static void* allocateObject( void* ptr, size_t size )
{
#ifdef OZ_TRACK_LEAKS

if( !isConstructed ) {
# if defined( __native_client__ )
    pthread_mutex_init( &traceEntryListLock, null );
# elif defined( _WIN32 )
    InitializeCriticalSection( &traceEntryListLock );
# else
    pthread_spin_init( &traceEntryListLock, PTHREAD_PROCESS_PRIVATE );
# endif
    isConstructed = true;
  }

  TraceEntry* st = static_cast<TraceEntry*>( malloc( sizeof( TraceEntry ) ) );

  st->address    = ptr;
  st->size       = size;
  st->stackTrace = StackTrace::current( 1 );

# if defined( __native_client__ )
  pthread_mutex_lock( &traceEntryListLock );
# elif defined( _WIN32 )
  EnterCriticalSection( &traceEntryListLock );
# else
  pthread_spin_lock( &traceEntryListLock );
# endif

  st->next = firstObjectTraceEntry;
  firstObjectTraceEntry = st;

# if defined( __native_client__ )
  pthread_mutex_unlock( &traceEntryListLock );
# elif defined( _WIN32 )
  LeaveCriticalSection( &traceEntryListLock );
# else
  pthread_spin_unlock( &traceEntryListLock );
# endif

#endif

  ++Alloc::count;
  Alloc::amount += size;

  ++Alloc::sumCount;
  Alloc::sumAmount += size;

  Alloc::maxCount = max<int>( Alloc::count, Alloc::maxCount );
  Alloc::maxAmount = max<size_t>( Alloc::amount, Alloc::maxAmount );

  ptr = static_cast<char*>( ptr ) + Alloc::alignUp( sizeof( size_t ) );
  static_cast<size_t*>( ptr )[-1] = size;

  return ptr;
}

static void* allocateArray( void* ptr, size_t size )
{
#ifdef OZ_TRACK_LEAKS

  if( !isConstructed ) {
# if defined( __native_client__ )
    pthread_mutex_init( &traceEntryListLock, null );
# elif defined( _WIN32 )
    InitializeCriticalSection( &traceEntryListLock );
# else
    pthread_spin_init( &traceEntryListLock, PTHREAD_PROCESS_PRIVATE );
# endif
    isConstructed = true;
  }

  TraceEntry* st = static_cast<TraceEntry*>( malloc( sizeof( TraceEntry ) ) );

  st->address    = ptr;
  st->size       = size;
  st->stackTrace = StackTrace::current( 1 );

# if defined( __native_client__ )
  pthread_mutex_lock( &traceEntryListLock );
# elif defined( _WIN32 )
  EnterCriticalSection( &traceEntryListLock );
# else
  pthread_spin_lock( &traceEntryListLock );
# endif

  st->next = firstArrayTraceEntry;
  firstArrayTraceEntry = st;

# if defined( __native_client__ )
  pthread_mutex_unlock( &traceEntryListLock );
# elif defined( _WIN32 )
  LeaveCriticalSection( &traceEntryListLock );
# else
  pthread_spin_unlock( &traceEntryListLock );
# endif

#endif

  ++Alloc::count;
  Alloc::amount += size;

  ++Alloc::sumCount;
  Alloc::sumAmount += size;

  Alloc::maxCount = max<int>( Alloc::count, Alloc::maxCount );
  Alloc::maxAmount = max<size_t>( Alloc::amount, Alloc::maxAmount );

  ptr = static_cast<char*>( ptr ) + Alloc::alignUp( sizeof( size_t ) );
  static_cast<size_t*>( ptr )[-1] = size;

  return ptr;
}

static void deallocateObject( void* ptr )
{
  size_t size = static_cast<size_t*>( ptr )[-1];
  ptr = static_cast<char*>( ptr ) - Alloc::alignUp( sizeof( size_t ) );

  --Alloc::count;
  Alloc::amount -= size;

#ifndef NDEBUG
  memset( ptr, 0xee, size );
#endif

#ifdef OZ_TRACK_LEAKS

# if defined( __native_client__ )
  pthread_mutex_lock( &traceEntryListLock );
# elif defined( _WIN32 )
  EnterCriticalSection( &traceEntryListLock );
# else
  pthread_spin_lock( &traceEntryListLock );
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

# if defined( __native_client__ )
      pthread_mutex_unlock( &traceEntryListLock );
# elif defined( _WIN32 )
      LeaveCriticalSection( &traceEntryListLock );
# else
      pthread_spin_unlock( &traceEntryListLock );
# endif

      goto backtraceFound;
    }
    prev = st;
    st = st->next;
  }

# if defined( __native_client__ )
  pthread_mutex_unlock( &traceEntryListLock );
# elif defined( _WIN32 )
  LeaveCriticalSection( &traceEntryListLock );
# else
  pthread_spin_unlock( &traceEntryListLock );
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

backtraceFound:

  free( st );

#endif

  aligned_free( ptr );
}

static void deallocateArray( void* ptr )
{
  size_t size = static_cast<size_t*>( ptr )[-1];
  ptr = static_cast<char*>( ptr ) - Alloc::alignUp( sizeof( size_t ) );

  --Alloc::count;
  Alloc::amount -= size;

#ifndef NDEBUG
  memset( ptr, 0xee, size );
#endif

#ifdef OZ_TRACK_LEAKS

# if defined( __native_client__ )
  pthread_mutex_lock( &traceEntryListLock );
# elif defined( _WIN32 )
  EnterCriticalSection( &traceEntryListLock );
# else
  pthread_spin_lock( &traceEntryListLock );
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

# if defined( __native_client__ )
      pthread_mutex_unlock( &traceEntryListLock );
# elif defined( _WIN32 )
      LeaveCriticalSection( &traceEntryListLock );
# else
      pthread_spin_unlock( &traceEntryListLock );
# endif

      goto backtraceFound;
    }
    prev = st;
    st = st->next;
  }

# if defined( __native_client__ )
  pthread_mutex_unlock( &traceEntryListLock );
# elif defined( _WIN32 )
  LeaveCriticalSection( &traceEntryListLock );
# else
  pthread_spin_unlock( &traceEntryListLock );
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

backtraceFound:

  free( st );

#endif

  aligned_free( ptr );
}

}

using namespace oz;

#if OZ_GCC <= 406
extern void* operator new ( size_t size ) throw ( std::bad_alloc )
#else
extern void* operator new ( size_t size )
#endif
{
  size += Alloc::alignUp( sizeof( size_t ) );

  void* ptr = aligned_malloc( size );

  if( ptr == null ) {
    System::trap();
    throw std::bad_alloc();
  }
  return allocateObject( ptr, size );
}

#if OZ_GCC <= 406
extern void* operator new[] ( size_t size ) throw ( std::bad_alloc )
#else
extern void* operator new[] ( size_t size )
#endif
{
  size += Alloc::alignUp( sizeof( size_t ) );

  void* ptr = aligned_malloc( size );

  if( ptr == null ) {
    System::trap();
    throw std::bad_alloc();
  }
  return allocateArray( ptr, size );
}

void operator delete ( void* ptr ) noexcept
{
  if( ptr == null ) {
    return;
  }

  deallocateObject( ptr );
}

void operator delete[] ( void* ptr ) noexcept
{
  if( ptr == null ) {
    return;
  }

  deallocateArray( ptr );
}

void* operator new ( size_t size, const std::nothrow_t& ) noexcept
{
  size += Alloc::alignUp( sizeof( size_t ) );

  void* ptr = aligned_malloc( size );

  if( ptr == null ) {
    System::trap();
    return null;
  }
  return allocateObject( ptr, size );
}

void* operator new[] ( size_t size, const std::nothrow_t& ) noexcept
{
  size += Alloc::alignUp( sizeof( size_t ) );

  void* ptr = aligned_malloc( size );

  if( ptr == null ) {
    System::trap();
    return null;
  }
  return allocateArray( ptr, size );
}

void operator delete ( void* ptr, const std::nothrow_t& ) noexcept
{
  if( ptr == null ) {
    return;
  }

  deallocateObject( ptr );
}

void operator delete[] ( void* ptr, const std::nothrow_t& ) noexcept
{
  if( ptr == null ) {
    return;
  }

  deallocateArray( ptr );
}
