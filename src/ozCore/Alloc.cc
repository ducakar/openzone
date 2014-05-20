/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
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

#include "arrays.hh"
#include "System.hh"

#include <cstdlib>
#include <malloc.h>

namespace oz
{

enum AllocMode
{
  OBJECT,
  ARRAY
};

static volatile int allocInfoLock = 0;

#ifdef OZ_TRACK_ALLOCS

static Alloc::ChunkInfo* volatile firstChunkInfo[2] = { nullptr, nullptr };

static void addChunkInfo( AllocMode mode, void* ptr, size_t size )
{
  Alloc::ChunkInfo* ci = static_cast<Alloc::ChunkInfo*>( malloc( sizeof( Alloc::ChunkInfo ) ) );
  if( ci == nullptr ) {
    OZ_ERROR( "oz::Alloc: ChunkInfo allocation failed" );
  }

  ci->address    = ptr;
  ci->size       = size;
  ci->stackTrace = StackTrace::current( 2 );

  while( __sync_lock_test_and_set( &allocInfoLock, 1 ) != 0 ) {
    while( allocInfoLock != 0 );
  }

  ci->next = firstChunkInfo[mode];
  firstChunkInfo[mode] = ci;

  __sync_lock_release( &allocInfoLock );
}

static void eraseChunkInfo( AllocMode mode, void* ptr, size_t size )
{
  while( __sync_lock_test_and_set( &allocInfoLock, 1 ) != 0 ) {
    while( allocInfoLock != 0 );
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

      __sync_lock_release( &allocInfoLock );

      goto chunkInfoFound;
    }
    prev = ci;
    ci = ci->next;
  }

  // Check if allocated as a different kind (object/array)
  ci = firstChunkInfo[!mode];

  while( ci != nullptr ) {
    if( ci->address == ptr ) {
      break;
    }
    ci = ci->next;
  }

  __sync_lock_release( &allocInfoLock );

  if( ci == nullptr ) {
    OZ_ERROR( "oz::Alloc: Freeing unregistered %s block at %p of size %lu",
              mode == OBJECT ? "object" : "array", ptr, ulong( ci->size ) );
  }
  else {
    OZ_ERROR( "oz::Alloc: new[] -> delete mismatch for %s block at %p of size %lu",
              mode == OBJECT ? "object" : "array", ptr, ulong( ci->size ) );
  }
chunkInfoFound:

  if( ci->size != size ) {
    OZ_ERROR( "oz::Alloc: Mismatched size %lu for %s block at %p of size %lu",
              ulong( size ), mode == OBJECT ? "object" : "array", ptr, ulong( ci->size ) );
  }

  free( ci );
}

#endif // OZ_TRACK_ALLOCS

static void* allocate( AllocMode mode, size_t size )
{
  static_cast<void>( mode );

#ifdef OZ_SIMD_MATH
  size = Alloc::alignUp( size )
#endif
  size += Alloc::alignUp( sizeof( size ) );

#if defined( OZ_SIMD_MATH ) && defined( _WIN32 )
  void* ptr = _aligned_malloc( size, Alloc::ALIGNMENT );
#elif defined( OZ_SIMD_MATH )
  void* ptr = alligned_alloc( Alloc::ALIGNMENT, size );
#else
  void* ptr = malloc( size );
#endif

  if( ptr == nullptr ) {
    OZ_ERROR( "oz::Alloc: Out of memory" );
  }

#ifdef OZ_TRACK_ALLOCS
  addChunkInfo( mode, ptr, size );
#endif

  while( __sync_lock_test_and_set( &allocInfoLock, 1 ) != 0 ) {
    while( allocInfoLock != 0 );
  }

  ++Alloc::count;
  Alloc::amount += size;

  ++Alloc::sumCount;
  Alloc::sumAmount += size;

  Alloc::maxCount  = max<int>( Alloc::count, Alloc::maxCount );
  Alloc::maxAmount = max<size_t>( Alloc::amount, Alloc::maxAmount );

  __sync_lock_release( &allocInfoLock );

  ptr = static_cast<char*>( ptr ) + Alloc::alignUp( sizeof( size ) );
  static_cast<size_t*>( ptr )[-1] = size;

  return ptr;
}

static void deallocate( AllocMode mode, void* ptr )
{
  static_cast<void>( mode );

  size_t size = static_cast<size_t*>( ptr )[-1];
  ptr = static_cast<char*>( ptr ) - Alloc::alignUp( sizeof( size ) );

  while( __sync_lock_test_and_set( &allocInfoLock, 1 ) != 0 ) {
    while( allocInfoLock != 0 );
  }

  --Alloc::count;
  Alloc::amount -= size;

  __sync_lock_release( &allocInfoLock );

#ifdef OZ_TRACK_ALLOCS
  eraseChunkInfo( mode, ptr, size );
#endif

#ifndef NDEBUG
  mSet( ptr, 0xee, size );
#endif

#if defined( OZ_SIMD_MATH ) && defined( _WIN32 )
  _aligned_free( ptr ));
#else
  free( ptr );
#endif
}

int    Alloc::count     = 0;
size_t Alloc::amount    = 0;
int    Alloc::sumCount  = 0;
size_t Alloc::sumAmount = 0;
int    Alloc::maxCount  = 0;
size_t Alloc::maxAmount = 0;

#ifdef OZ_TRACK_ALLOCS

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

OZ_WEAK
void* operator new ( size_t size )
{
  return oz::allocate( oz::OBJECT, size );
}

OZ_WEAK
void* operator new[] ( size_t size )
{
  return oz::allocate( oz::ARRAY, size );
}

OZ_WEAK
void operator delete ( void* ptr ) noexcept
{
  if( ptr == nullptr ) {
    return;
  }
  oz::deallocate( oz::OBJECT, ptr );
}

OZ_WEAK
void operator delete[] ( void* ptr ) noexcept
{
  if( ptr == nullptr ) {
    return;
  }
  oz::deallocate( oz::ARRAY, ptr );
}

OZ_WEAK
void* operator new ( size_t size, const std::nothrow_t& ) noexcept
{
  return oz::allocate( oz::OBJECT, size );
}

OZ_WEAK
void* operator new[] ( size_t size, const std::nothrow_t& ) noexcept
{
  return oz::allocate( oz::ARRAY, size );
}

OZ_WEAK
void operator delete ( void* ptr, const std::nothrow_t& ) noexcept
{
  if( ptr == nullptr ) {
    return;
  }
  oz::deallocate( oz::OBJECT, ptr );
}

OZ_WEAK
void operator delete[] ( void* ptr, const std::nothrow_t& ) noexcept
{
  if( ptr == nullptr ) {
    return;
  }
  oz::deallocate( oz::ARRAY, ptr );
}
