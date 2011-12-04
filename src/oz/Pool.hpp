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
 * @file oz/Pool.hpp
 */

#pragma once

#include "Alloc.hpp"

/**
 * @def OZ_STATIC_POOL_ALLOC( pool )
 *
 * Define overloaded <tt>new</code> and <tt>delete</tt> for a class that use the given pool.
 *
 * As <tt>new</tt>/<tt>delete</tt> are static functions so has to be the given pool.
 * The derived classes also need to have overloaded <tt>new</tt>/<tt>delete</tt> defined the
 * same way otherwise the ones from the base class will be used, which will not end good.
 *
 * @ingroup oz
 */
#define OZ_STATIC_POOL_ALLOC( pool ) \
  public:\
    void* operator new ( size_t ) { return pool.alloc(); } \
    void  operator delete ( void* ptr ) { if( ptr != null ) pool.dealloc( ptr ); }

/**
 * @def OZ_PLACEMENT_POOL_ALLOC( Type, SIZE )
 *
 * Define placement <tt>new</tt> for allocation from a pool and disable non-placement
 * <tt>new</tt> and <tt>delete</tt> for the class.
 *
 * The pool is given to new operator as an additional parameter. As delete cannot be provided,
 * object should be manually destructed and deallocated via <code>pool.dealloc( object )</code>.
 *
 * @ingroup oz
 */
#define OZ_PLACEMENT_POOL_ALLOC( Type, SIZE ) \
  public: \
    void* operator new ( size_t, oz::Pool<Type, SIZE>& pool ) { return pool.alloc(); } \
    void  operator delete ( void* ptr, oz::Pool<Type, SIZE>& pool ) { pool.dealloc( ptr ); } \
    void* operator new ( size_t ) = delete; \
    void  operator delete ( void* ) = delete;

namespace oz
{

/**
 * Memory pool.
 *
 * Memory pool consists of a linked list of memory blocks, each an array of uninitialised elements
 * of the specified data type.
 *
 * Unless <tt>NDEBUG</tt> macro is defined, all freed memory is rewritten with 0xee byte values.
 *
 * @ingroup oz
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
     * %Block is an array that can hold up to <tt>BLOCK_SIZE</tt> elements. When we run out of
     * space we simply allocate another block. Once a block is allocated it cannot be freed any
     * more unless Pool is empty. That would be rarely possible due to fragmentation anyway.
     */
    struct Block
    {
      Slot   data[BLOCK_SIZE]; ///< Slots.
      Block* next;             ///< Pointer to the next block.

      /**
       * Create a new block.
       */
      explicit Block( Block* next_ ) : next( next_ )
      {
        for( int i = 0; i < BLOCK_SIZE - 1; ++i ) {
          data[i].nextSlot = &data[i + 1];
        }
        data[BLOCK_SIZE - 1].nextSlot = null;
      }
    };

    Block* firstBlock; ///< List of the allocated blocks.
    Slot*  freeSlot;   ///< List of free slots or null if none.
    int    size;       ///< Capacity.
    int    count;      ///< Number of occupied slots in the pool.

  public:

    /**
     * Create an empty pool, storage is allocated when the first allocation is made.
     */
    Pool() : firstBlock( null ), freeSlot( null ), size( 0 ), count( 0 )
    {}

    /**
     * Destructor.
     */
    ~Pool()
    {
      free();
    }

    /**
     * No copying.
     */
    Pool( const Pool& ) = delete;

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
     * No copying.
     */
    Pool& operator = ( const Pool& ) = delete;

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
    void* alloc() throw( std::bad_alloc )
    {
      hard_assert( !Alloc::isLocked );

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
    void dealloc( void* ptr ) throw()
    {
      hard_assert( !Alloc::isLocked );
      hard_assert( count != 0 );

      Slot* slot = reinterpret_cast<Slot*>( ptr );

#ifndef NDEBUG
      char* slotBytes = reinterpret_cast<char*>( ptr );

      for( size_t i = 0; i < sizeof( Slot ); ++i ) {
        slotBytes[i] = char( 0xee );
      }
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
     * Frees all blocks allocated. It will not end good if something still uses memory allocated by
     * the pool.
     */
    void free()
    {
      if( firstBlock == null ) {
        return;
      }

      hard_assert( !Alloc::isLocked );
      soft_assert( count == 0 );

      Block* block = firstBlock;

      while( block != null ) {
        Block* next = block->next;
        delete block;

        block = next;
      }

      firstBlock = null;
      freeSlot   = null;
      size       = 0;
      count      = 0;
    }

};

}
