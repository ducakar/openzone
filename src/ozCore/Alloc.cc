/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file ozCore/Alloc.cc
 */

#include "Alloc.hh"

#include "System.hh"
#include "Log.hh"

#include <cstdlib>
#include <malloc.h>

#ifdef has_feature
# if has_feature( address_sanitizer )
#  define OZ_ADDRESS_SANITIZER
# endif
#endif

namespace oz
{

#ifndef OZ_ADDRESS_SANITIZER

enum AllocMode
{
  OBJECT,
  ARRAY
};

#ifdef OZ_TRACK_ALLOCS

// Holds info about a memory allocation, used to track memory leaks and new/delete mismatches.
// If we deallocate from two different threads at once with OZ_TRACK_ALLOCS, changing the list of
// allocated blocks while iterating it in another thread can result in a SIGSEGV, so list operations
// must be protected my a spin lock.
struct TraceEntry
{
  TraceEntry* next;
  void*       address;
  size_t      size;
  StackTrace  stackTrace;
};

static TraceEntry* volatile firstTraceEntry[2] = { nullptr, nullptr };
static volatile bool        traceLock          = false;

static void addTraceEntry( AllocMode mode, void* ptr, size_t size )
{
  TraceEntry* st = static_cast<TraceEntry*>( malloc( sizeof( TraceEntry ) ) );
  if( st == nullptr ) {
    OZ_ERROR( "TraceEntry allocation failed" );
  }

  st->address    = ptr;
  st->size       = size;
  st->stackTrace = StackTrace::current( 2 );

  while( __sync_lock_test_and_set( &traceLock, true ) ) {
    while( traceLock );
  }

  st->next = firstTraceEntry[mode];
  firstTraceEntry[mode] = st;

  __sync_lock_release( &traceLock );
}

static void eraseTraceEntry( AllocMode mode, void* ptr )
{
  while( __sync_lock_test_and_set( &traceLock, true ) ) {
    while( traceLock );
  }

  TraceEntry* st   = firstTraceEntry[mode];
  TraceEntry* prev = nullptr;

  while( st != nullptr ) {
    if( st->address == ptr ) {
      if( prev == nullptr ) {
        firstTraceEntry[mode] = st->next;
      }
      else {
        prev->next = st->next;
      }

      __sync_lock_release( &traceLock );

      goto backtraceFound;
    }
    prev = st;
    st = st->next;
  }

  __sync_lock_release( &traceLock );

  // Check if allocated as a different kind (object/array)
  st = firstTraceEntry[!mode];

  while( st != nullptr ) {
    if( st->address == ptr ) {
      break;
    }
    st = st->next;
  }

  if( st == nullptr ) {
    OZ_ERROR( mode == OBJECT ? "ALLOC: Freeing object at %p that has not been allocated" :
                               "ALLOC: Freeing array at %p that has not been allocated", ptr );
  }
  else {
    OZ_ERROR( mode == OBJECT ? "ALLOC: new[] -> delete mismatch for block at %p" :
                               "ALLOC: new -> delete[] mismatch for block at %p", ptr );
  }

backtraceFound:

  free( st );
}

#endif // OZ_TRACK_ALLOCS

static void* allocate( AllocMode mode, size_t size )
{
  size += Alloc::alignUp( sizeof( size ) );

#if defined( OZ_SIMD_MATH ) && defined( _WIN32 )
  void* ptr = _aligned_malloc( size, Alloc::ALIGNMENT );
#elif defined( OZ_SIMD_MATH )
  void* ptr = memalign( Alloc::ALIGNMENT, size );
#else
  void* ptr = malloc( size );
#endif

  if( ptr == nullptr ) {
    OZ_ERROR( "Out of memory" );
  }

  ++Alloc::count;
  Alloc::amount += size;

  ++Alloc::sumCount;
  Alloc::sumAmount += size;

  Alloc::maxCount = max<int>( Alloc::count, Alloc::maxCount );
  Alloc::maxAmount = max<size_t>( Alloc::amount, Alloc::maxAmount );

  ptr = static_cast<char*>( ptr ) + Alloc::alignUp( sizeof( size ) );
  static_cast<size_t*>( ptr )[-1] = size;

#ifdef OZ_TRACK_ALLOCS
  addTraceEntry( mode, ptr, size );
#else
  static_cast<void>( mode );
#endif

  return ptr;
}

static void deallocate( AllocMode mode, void* ptr )
{
#ifdef OZ_TRACK_ALLOCS
  eraseTraceEntry( mode, ptr );
#else
  static_cast<void>( mode );
#endif

  size_t size = static_cast<size_t*>( ptr )[-1];
  ptr = static_cast<char*>( ptr ) - Alloc::alignUp( sizeof( size ) );

  --Alloc::count;
  Alloc::amount -= size;

#ifndef NDEBUG
  mSet( ptr, 0xee, size );
#endif

#if defined( OZ_SIMD_MATH ) && defined( _WIN32 )
  _aligned_free( ptr ));
#else
  free( ptr );
#endif
}

#endif // !OZ_ADDRESS_SANITIZER

static_assert( Alloc::ALIGNMENT >= sizeof( void* ) &&
               ( Alloc::ALIGNMENT & ( Alloc::ALIGNMENT - 1 ) ) == 0,
               "Alloc::ALIGNMENT must be at least size of a pointer and a power of two" );

const size_t         Alloc::ALIGNMENT;
#ifdef OZ_ADDRESS_SANITIZER
const bool           Alloc::OVERLOADS_NEW_AND_DELETE = false;
#else
const bool           Alloc::OVERLOADS_NEW_AND_DELETE = true;
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
  Log::println( "current amount     %.2f MiB (%lu B)",
                float( amount ) / ( 1024.0f * 1024.0f ), ulong( amount ) );
  Log::println( "maximum chunks     %d", maxCount );
  Log::println( "maximum amount     %.2f MiB (%lu B)",
                float( maxAmount ) / ( 1024.0f * 1024.0f ), ulong( maxAmount ) );
  Log::println( "cumulative chunks  %d", sumCount );
  Log::println( "cumulative amount  %.2f MiB (%lu B)",
                float( sumAmount ) / ( 1024.0f * 1024.0f ), ulong( sumAmount ) );

  Log::unindent();
  Log::println( "}" );
}

#if defined( OZ_ADDRESS_SANITIZER ) || !defined( OZ_TRACK_ALLOCS )

bool Alloc::printLeaks()
{
  return false;
}

#else

bool Alloc::printLeaks()
{
  bool hasOutput = false;

  const TraceEntry* bt;

  bt = firstTraceEntry[OBJECT];
  while( bt != nullptr ) {
    Log::println( "Leaked object at %p of size %lu B allocated", bt->address, ulong( bt->size ) );
    Log::indent();
    Log::printTrace( bt->stackTrace );
    Log::unindent();

    bt = bt->next;
    hasOutput = true;
  }

  bt = firstTraceEntry[ARRAY];
  while( bt != nullptr ) {
    Log::println( "Leaked array at %p of size %lu B allocated", bt->address, ulong( bt->size ) );
    Log::indent();
    Log::printTrace( bt->stackTrace );
    Log::unindent();

    bt = bt->next;
    hasOutput = true;
  }

  return hasOutput;
}

#endif

}

using namespace oz;

#ifndef OZ_ADDRESS_SANITIZER

void* operator new ( size_t size )
{
  return allocate( OBJECT, size );
}

void* operator new[] ( size_t size )
{
  return allocate( ARRAY, size );
}

void operator delete ( void* ptr ) noexcept
{
  if( ptr == nullptr ) {
    return;
  }
  deallocate( OBJECT, ptr );
}

void operator delete[] ( void* ptr ) noexcept
{
  if( ptr == nullptr ) {
    return;
  }
  deallocate( ARRAY, ptr );
}

void* operator new ( size_t size, const std::nothrow_t& ) noexcept
{
  return allocate( OBJECT, size );
}

void* operator new[] ( size_t size, const std::nothrow_t& ) noexcept
{
  return allocate( ARRAY, size );
}

void operator delete ( void* ptr, const std::nothrow_t& ) noexcept
{
  if( ptr == nullptr ) {
    return;
  }
  deallocate( OBJECT, ptr );
}

void operator delete[] ( void* ptr, const std::nothrow_t& ) noexcept
{
  if( ptr == nullptr ) {
    return;
  }
  deallocate( ARRAY, ptr );
}

#endif
