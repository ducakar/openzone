/*
 *  Pool.h
 *
 *  Pool memory allocator.
 *  The Type should provide the "next[INDEX]" pointer.
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

namespace oz
{

  template <class Type, int INDEX>
  class Pool
  {
    private:

      static const int BLOCK_SIZE = 1024;

      // no copying
      Pool( const Pool& );
      Pool&operator = ( const Pool& );

      /**
       * Memory block.
       * Block is an array that can hold up to BLOCK_SIZE elements. When we run out of space
       * we simply allocate another block. Once a block is allocated in cannot be freed any
       * more. Anyways, that would be rarely possible due to fragmentation.
       */
      struct Block
      {
        byte  data[BLOCK_SIZE * sizeof( Type )];
        Block *next[1];

        explicit Block()
        {
          for( int i = 0; i < BLOCK_SIZE - 1; i++ ) {
            get( i ).next[INDEX] = &get( i + 1 );
          }
          get( BLOCK_SIZE - 1 ).next[INDEX] = null;
        }

        Type &get( int i )
        {
          return reinterpret_cast<Type*>( data )[i];
        }

        const Type &get( int i ) const
        {
          return reinterpret_cast<const Type*>( data )[i];
        }
      };

      // List of allocated blocks
      List<Block, 0> blocks;
      // Last freed block, null if none
      Type *freeSlot;
      // Size of data blocks
      int  size;
      // Number of used slots in the pool
      int  count;

    public:

      /**
       * Create empty pool with initial capacity BLOCK_SIZE.
       * @param initSize
       */
      explicit Pool() : freeSlot( null ), size( 0 ), count( 0 )
      {}

      /**
       * Destructor.
       */
      ~Pool()
      {
        assert( count == 0 );

        blocks.free();
      }

      /**
       * Allocate a new element.
       * @param e
       */
      void *alloc()
      {
        count++;

        if( freeSlot == null ) {
          blocks << new Block();
          freeSlot = &blocks.first()->get( 1 );
          size += BLOCK_SIZE;
          return &blocks.first()->get( 0 );
        }
        else {
          Type *slot = freeSlot;
          freeSlot = slot->next[INDEX];
          return slot;
        }
      }

      /**
       * Free given element.
       * @param index
       */
      void free( Type *elem )
      {
        assert( count != 0 );

        elem->next[INDEX] = freeSlot;
        freeSlot = elem;
        count--;
      }

      /**
       * @return number of used slots in the pool
       */
      int length() const
      {
        return count;
      }

      /**
       * @return capacity of the pool
       */
      int capacity() const
      {
        return size;
      }

      /**
       * @return true if pool has no used slots
       */
      bool isEmpty() const
      {
        return count == 0;
      }

      /**
       * Free the pool.
       * Frees all blocks allocated. It won't end good if something still uses memory allocated
       * by this pool.
       */
      void free()
      {
        assert( count == 0 );

        blocks.free();
        size = 0;
        count = 0;
        freeSlot = null;
      }

  };

}
