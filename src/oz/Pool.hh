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
 * @file oz/Pool.hh
 *
 * Pool template class.
 */

#pragma once

#include "common.hh"

/**
 * @def OZ_STATIC_POOL_ALLOC( pool )
 *
 * Define overloaded `new` and `delete` for a class that use the given pool.
 *
 * As `new`/`delete` are static functions so has to be the given pool. The derived classes also need
 * to have overloaded `new`/`delete` defined the same way otherwise the ones from the base class
 * will be used, which will not end good.
 */
#define OZ_STATIC_POOL_ALLOC( pool ) \
  void* operator new ( size_t ) { return pool.alloc(); } \
  void  operator delete ( void* ptr ) noexcept { if( ptr != null ) pool.dealloc( ptr ); } \
  void* operator new ( size_t, const std::nothrow_t& ) noexcept = delete; \
  void  operator delete ( void*, std::nothrow_t& ) noexcept = delete;

/**
 * @def OZ_PLACEMENT_POOL_ALLOC( Type, SIZE )
 *
 * Define placement `new` for allocation from a pool and disable non-placement `new` and `delete`
 * for the class.
 *
 * The pool is given to new operator as an additional parameter. As delete cannot be provided,
 * object should be manually destructed and deallocated via `pool.dealloc( object )`.
 */
#define OZ_PLACEMENT_POOL_ALLOC( Type, SIZE ) \
  void* operator new ( size_t ) = delete; \
  void  operator delete ( void* ) noexcept = delete; \
  void* operator new ( size_t, const std::nothrow_t& ) noexcept = delete; \
  void  operator delete ( void*, const std::nothrow_t& ) noexcept = delete; \
  void* operator new ( size_t, oz::Pool<Type, SIZE>& pool ) { return pool.alloc(); } \
  void  operator delete ( void* ptr, oz::Pool<Type, SIZE>& pool ) noexcept { pool.dealloc( ptr ); }

namespace oz
{

/**
 * Memory pool.
 *
 * Memory pool consists of a linked list of memory blocks, each an array of uninitialised elements
 * of the specified data type.
 *
 * Unless `NDEBUG` macro is defined, all freed memory is rewritten with 0xee byte values.
 */
template <class Type, int BLOCK_SIZE = 256>
class Pool
{
  static_assert( BLOCK_SIZE >= 2, "Pool block size must be at least 2" );

  private:

    /**
     * %Slot that occupies memory for an object.
     */
    union Slot
    {
      char  content[ sizeof( Type ) ]; ///< Uninitialised memory for an object.
      Slot* nextSlot;                  ///< For an empty slot, a pointer to the next empty slot.
    };

    /**
     * Memory block.
     *
     * `Block` is an array that can hold up to `BLOCK_SIZE` elements. When we run out of space we
     * simply allocate another block. Once a block is allocated it cannot be freed any more unless
     * Pool is empty. That would be rarely possible due to fragmentation anyway.
     */
    struct Block
    {
      Slot   data[BLOCK_SIZE]; ///< Slots.
      Block* next;             ///< Pointer to the next block.

      /**
       * Create a new block.
       */
      explicit Block( Block* next_ ) :
        next( next_ )
      {
        for( int i = 0; i < BLOCK_SIZE - 1; ++i ) {
          data[i].nextSlot = &data[i + 1];
        }
        data[BLOCK_SIZE - 1].nextSlot = null;
      }
    };

    Block* firstBlock; ///< Linked list of the allocated blocks.
    Slot*  freeSlot;   ///< Linked list of free slots or null if none.
    int    size;       ///< Capacity.
    int    count;      ///< Number of occupied slots in the pool.

  public:

    /**
     * Create an empty pool, storage is allocated when the first allocation is made.
     */
    Pool() :
      firstBlock( null ), freeSlot( null ), size( 0 ), count( 0 )
    {}

    /**
     * Destructor.
     */
    ~Pool()
    {
      free();
    }

    /**
     * Move constructor, moves storage.
     */
    Pool( Pool&& p ) :
      firstBlock( p.firstBlock ), freeSlot( p.freeSlot ), size( p.size ), count( p.count )
    {
      p.firstBlock = null;
      p.freeSlot   = null;
      p.size       = 0;
      p.count      = 0;
    }

    /**
     * Move operator, moves storage.
     */
    Pool& operator = ( Pool&& p )
    {
      if( &p == this ) {
        return *this;
      }

      hard_assert( count == 0 );

      free();

      firstBlock = p.firstBlock;
      freeSlot   = p.freeSlot;
      size       = p.size;
      count      = p.count;

      p.firstBlock = null;
      p.freeSlot   = null;
      p.size       = 0;
      p.count      = 0;

      return *this;
    }

    /**
     * Allocate a new object.
     */
    void* alloc()
    {
      ++count;

      if( freeSlot == null ) {
        firstBlock = new Block( firstBlock );
        freeSlot = &firstBlock->data[1];
        size += BLOCK_SIZE;
        return firstBlock->data[0].content;
      }
      else {
        Slot* slot = freeSlot;
        freeSlot = slot->nextSlot;
        return slot;
      }
    }

    /**
     * Free the given object.
     */
    void dealloc( void* ptr )
    {
      hard_assert( count != 0 );

      Slot* slot = static_cast<Slot*>( ptr );

#ifndef NDEBUG
      __builtin_memset( slot, 0xee, sizeof( Slot ) );
#endif

      slot->nextSlot = freeSlot;
      freeSlot = slot;
      --count;
    }

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
     * Deallocate the storage.
     *
     * In the case the pool is not empty it is still cleared but memory is not deallocated. This
     * memory leak is intended to prevent potential crashes and it only happens if you already have
     * a memory leak in your program.
     */
    void free()
    {
      if( firstBlock == null ) {
        return;
      }

      soft_assert( count == 0 );

      if( count == 0 ) {
        Block* block = firstBlock;

        while( block != null ) {
          Block* next = block->next;
          delete block;

          block = next;
        }
      }

      firstBlock = null;
      freeSlot   = null;
      size       = 0;
      count      = 0;
    }

};

}
