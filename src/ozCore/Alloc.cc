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

#if defined( __SANITIZE_ADDRESS__ )
# define OZ_ADDRESS_SANITIZER
#elif defined( has_feature )
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

static Alloc::ChunkInfo* volatile firstChunkInfo[2] = { nullptr, nullptr };
static volatile bool              chunkInfoLock     = 0;

static void addChunkInfo( AllocMode mode, void* ptr, size_t size )
{
  Alloc::ChunkInfo* ci = static_cast<Alloc::ChunkInfo*>( malloc( sizeof( Alloc::ChunkInfo ) ) );
  if( ci == nullptr ) {
    OZ_ERROR( "ChunkInfo allocation failed" );
  }

  ci->address    = ptr;
  ci->size       = size;
  ci->stackTrace = StackTrace::current( 2 );

  while( __sync_lock_test_and_set( &chunkInfoLock, 1 ) != 0 ) {
    while( chunkInfoLock != 0 );
  }

  ci->next = firstChunkInfo[mode];
  firstChunkInfo[mode] = ci;

  __sync_lock_release( &chunkInfoLock );
}

static void eraseChunkInfo( AllocMode mode, void* ptr )
{
  while( __sync_lock_test_and_set( &chunkInfoLock, 1 ) != 0 ) {
    while( chunkInfoLock != 0 );
  }

  Alloc::ChunkInfo* ci   = firstChunkInfo[mode];
  Alloc::ChunkInfo* prev = nullptr;

  while( ci != nullptr ) {
    if( ci->address == ptr ) {
      if( prev == nullptr ) {
        firstChunkInfo[mode] = ci->next;
      }
      else {
        prev->next = ci->next;
      }

      __sync_lock_release( &chunkInfoLock );

      goto chunkInfoFound;
    }
    prev = ci;
    ci = ci->next;
  }

  __sync_lock_release( &chunkInfoLock );

  // Check if allocated as a different kind (object/array)
  ci = firstChunkInfo[!mode];

  while( ci != nullptr ) {
    if( ci->address == ptr ) {
      break;
    }
    ci = ci->next;
  }
  if( ci == nullptr ) {
    OZ_ERROR( mode == OBJECT ? "ALLOC: Freeing object at %p that has not been allocated" :
                               "ALLOC: Freeing array at %p that has not been allocated", ptr );
  }
  else {
    OZ_ERROR( mode == OBJECT ? "ALLOC: new[] -> delete mismatch for block at %p" :
                               "ALLOC: new -> delete[] mismatch for block at %p", ptr );
  }
  chunkInfoFound:

  free( ci );
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
  addChunkInfo( mode, ptr, size );
#else
  static_cast<void>( mode );
#endif

  return ptr;
}

static void deallocate( AllocMode mode, void* ptr )
{
#ifdef OZ_TRACK_ALLOCS
  eraseChunkInfo( mode, ptr );
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

#if !defined( OZ_ADDRESS_SANITIZER ) && defined( OZ_TRACK_ALLOCS )

Alloc::ChunkCIterator Alloc::objectCIter()
{
  return ChunkCIterator( firstChunkInfo[OBJECT] );
}

Alloc::ChunkCIterator Alloc::arrayCIter()
{
  return ChunkCIterator( firstChunkInfo[ARRAY] );
}

#else

Alloc::ChunkCIterator Alloc::objectCIter()
{
  return ChunkCIterator();
}

Alloc::ChunkCIterator Alloc::arrayCIter()
{
  return ChunkCIterator();
}

#endif

}

#ifndef OZ_ADDRESS_SANITIZER

using namespace oz;

OZ_WEAK
void* operator new ( size_t size )
{
  return allocate( OBJECT, size );
}

OZ_WEAK
void* operator new[] ( size_t size )
{
  return allocate( ARRAY, size );
}

OZ_WEAK
void operator delete ( void* ptr ) noexcept
{
  if( ptr == nullptr ) {
    return;
  }
  deallocate( OBJECT, ptr );
}

OZ_WEAK
void operator delete[] ( void* ptr ) noexcept
{
  if( ptr == nullptr ) {
    return;
  }
  deallocate( ARRAY, ptr );
}

OZ_WEAK
void* operator new ( size_t size, const std::nothrow_t& ) noexcept
{
  return allocate( OBJECT, size );
}

OZ_WEAK
void* operator new[] ( size_t size, const std::nothrow_t& ) noexcept
{
  return allocate( ARRAY, size );
}

OZ_WEAK
void operator delete ( void* ptr, const std::nothrow_t& ) noexcept
{
  if( ptr == nullptr ) {
    return;
  }
  deallocate( OBJECT, ptr );
}

OZ_WEAK
void operator delete[] ( void* ptr, const std::nothrow_t& ) noexcept
{
  if( ptr == nullptr ) {
    return;
  }
  deallocate( ARRAY, ptr );
}

#endif
