/*
 *  Alloc.cpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

/**
 * @file oz/Alloc.cpp
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
# include <pthread.h>
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

// If we deallocate from two different threads at once with OZ_TRACE_LEAKS, changing the list
// of allocated blocks while iterating it in another thread can result in a SIGSEGV.
static pthread_mutex_t sectionMutex = PTHREAD_MUTEX_INITIALIZER;

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
               float( amount ) / ( 1024.0f*1024.0f ), amount );
  log.println( "maximum chunks     %d", maxCount );
  log.println( "maximum amount     %.2f MiB (%d B)",
               float( maxAmount ) / ( 1024.0f*1024.0f ), maxAmount );
  log.println( "cumulative chunks  %d", sumCount );
  log.println( "cumulative amount  %.2f MiB (%d B)",
               float( sumAmount ) / ( 1024.0f*1024.0f ), sumAmount );

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

using namespace oz;

#if defined( OZ_MINGW ) && defined( OZ_SIMD )

/**
 * Emulation of POSIX function posix_memalign.
 *
 * @ingroup oz
 */
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

/**
 * Free storage allocated by the fake posix_memalign function.
 *
 * @ingroup oz
 */
static void posix_memalign_free( void* ptr )
{
  void** beginPtr = reinterpret_cast<void**>( ptr );

  free( beginPtr[-1] );
}

#endif

/**
 * <tt>operator new</tt> implementation with memory statistics and optionally memory alignment
 * and leak tracing.
 *
 * @ingroup oz
 */
void* operator new ( size_t size ) throw( std::bad_alloc )
{
  hard_assert( !Alloc::isLocked );
  hard_assert( size != 0 );

  size += Alloc::alignUp( sizeof( size_t ) );

#ifdef OZ_SIMD
  void* ptr;
  if( posix_memalign( &ptr, Alloc::ALIGNMENT, size ) ) {
    System::trap();
    throw std::bad_alloc();
  }
#else
  void* ptr = malloc( size );
  if( ptr == null ) {
    System::trap();
    throw std::bad_alloc();
  }
#endif

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

/**
 * <tt>operator new[]</tt> implementation with memory statistics and optionally memory alignment
 * and leak tracing.
 *
 * @ingroup oz
 */
void* operator new[] ( size_t size ) throw( std::bad_alloc )
{
  hard_assert( !Alloc::isLocked );
  hard_assert( size != 0 );

  size += Alloc::alignUp( sizeof( size_t ) );

#ifdef OZ_SIMD
  void* ptr;
  if( posix_memalign( &ptr, Alloc::ALIGNMENT, size ) ) {
    System::trap();
    throw std::bad_alloc();
  }
#else
  void* ptr = malloc( size );
  if( ptr == null ) {
    System::trap();
    throw std::bad_alloc();
  }
#endif

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

/**
 * <tt>operator delete</tt> implementation for the matching <tt>operator new</tt>.
 *
 * @ingroup oz
 */
void operator delete ( void* ptr ) noexcept
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

#if defined( OZ_MINGW ) && defined( OZ_SIMD )
  posix_memalign_free( chunk );
#else
  free( chunk );
#endif
}

/**
 * <tt>operator delete[]</tt> implementation for the matching <tt>operator new[]</tt>.
 *
 * @ingroup oz
 */
void operator delete[] ( void* ptr ) noexcept
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

#if defined( OZ_MINGW ) && defined( OZ_SIMD )
  posix_memalign_free( chunk );
#else
  free( chunk );
#endif
}
