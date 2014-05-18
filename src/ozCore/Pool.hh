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
 * @file ozCore/Pool.hh
 *
 * `Pool` class template.
 */

#pragma once

#include "arrays.hh"

/**
 * @def OZ_STATIC_POOL_ALLOC
 * Add this macro to your class definition to define standard non-array `new`/`delete` operators for
 * a class.
 *
 * As `new`/`delete` are static functions so has to be the pool. The derived classes also need to
 * have overloaded `new`/`delete` defined the same way otherwise the ones from the base class will
 * be used, which will not end good.
 *
 * Array versions of `new`/`delete` operator are disabled for the enclosing class.
 */
#define OZ_STATIC_POOL_ALLOC( pool ) \
  void* operator new ( size_t ) { return pool.allocate(); } \
  void* operator new[] ( size_t ) = delete; \
  void  operator delete ( void* ptr ) noexcept { pool.deallocate( ptr ); } \
  void  operator delete[] ( void* ) noexcept = delete; \
  void* operator new ( size_t, const std::nothrow_t& ) noexcept { return pool.allocate(); } \
  void* operator new[] ( size_t, const std::nothrow_t& ) noexcept = delete; \
  void  operator delete ( void* ptr, std::nothrow_t& ) noexcept { pool.deallocate( ptr ); } \
  void  operator delete[] ( void*, std::nothrow_t& ) noexcept = delete;

/**
 * @def OZ_PLACEMENT_POOL_ALLOC
 * Add this macro to your class definition to define a `new` overload that accepts a pool parameter.
 *
 * The pool is given to new operator as an additional parameter. As delete cannot be provided,
 * object should be manually destructed and deallocated via `pool.deallocate( object )`.
 *
 * All standard `new`/`delete` operators are disabled for the enclosing class.
 */
#define OZ_PLACEMENT_POOL_ALLOC( Type ) \
  void* operator new ( size_t ) = delete; \
  void* operator new[] ( size_t ) = delete; \
  void  operator delete ( void* ) noexcept = delete; \
  void  operator delete[] ( void* ) noexcept = delete; \
  void* operator new ( size_t, const std::nothrow_t& ) noexcept = delete; \
  void* operator new[] ( size_t, const std::nothrow_t& ) noexcept = delete; \
  void  operator delete ( void*, const std::nothrow_t& ) noexcept = delete; \
  void  operator delete[] ( void*, const std::nothrow_t& ) noexcept = delete; \
  void* operator new ( size_t, oz::PoolAlloc& pool ) { return pool.allocate(); } \
  void  operator delete ( void* ptr, oz::PoolAlloc& pool ) noexcept { pool.deallocate( ptr ); }

namespace oz
{

/**
 * Memory pool (non-template version).
 *
 * Memory pool consists of a linked list of memory blocks, each an array of uninitialised elements
 * of the specified data type.
 *
 * Unless `NDEBUG` macro is defined, all freed memory is rewritten with 0xee byte values.
 *
 * @sa `oz::Pool`
 */
class PoolAlloc
{
private:

  union  Slot;
  struct Block;

  Block* firstBlock; ///< Linked list of the allocated blocks.
  Slot*  freeSlot;   ///< Linked list of free slots or `nullptr` if none.
  size_t slotSize;   ///< Size of an object.
  int    nSlots;     ///< Number of objects in a memory block.
  int    count;      ///< Number of occupied slots in the pool.
  int    size;       ///< Capacity.

public:

  /**
   * Create an empty pool, storage is allocated when the first allocation is made.
   */
  explicit PoolAlloc( size_t slotSize, int nSlots = 64 );

  /**
   * Destructor.
   */
  ~PoolAlloc();

  /**
   * Move constructor, moves storage.
   */
  PoolAlloc( PoolAlloc&& p );

  /**
   * Move operator, moves storage.
   */
  PoolAlloc& operator = ( PoolAlloc&& p );

  /**
   * Number of used slots in the pool.
   */
  OZ_ALWAYS_INLINE
  int length() const
  {
    return count;
  }

  /**
   * True iff no slots are used.
   */
  OZ_ALWAYS_INLINE
  bool isEmpty() const
  {
    return count == 0;
  }

  /**
   * Number of allocated slots.
   */
  OZ_ALWAYS_INLINE
  int capacity() const
  {
    return size;
  }

  /**
   * Allocate a new object.
   */
  void* allocate();

  /**
   * Free a given object.
   */
  void deallocate( void* ptr );

  /**
   * Deallocate the storage.
   *
   * In the case the pool is not empty it is still cleared but memory is not deallocated. This
   * memory leak is intended to prevent potential crashes and it only happens if you already have
   * a memory leak in your program.
   */
  void free();

};

/**
 * Template wrapper for `PoolAlloc`.
 *
 * @sa `oz::PoolAlloc`
 */
template <class Elem, int BLOCK_SLOTS = 64>
class Pool : public PoolAlloc
{
  static_assert( BLOCK_SLOTS > 0, "Pool block must have at least 1 block." );

public:

  /**
   * Create an empty pool.
   */
  explicit Pool() :
    PoolAlloc( sizeof( Elem ), BLOCK_SLOTS )
  {}

};

}
