/*
 *  PoolAlloc.h
 *
 *  Pool allocator
 *  Base class for allocation of memory for same-size elements. All classes which inherit from
 *  PoolAlloc will have overloaded new and delete (BUT NOT new[] and delete[]) operators. Before
 *  you use it, you should create Pool<Type> for each class.
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  template <class Type, int INDEX>
  class PoolAlloc
  {
    public:

      /**
       * Internal class for memory management
       * GCC wants that to be a template, otherwise it reports syntax error when defining static
       * member storage (PoolAlloc::pool).
       */
      template <class Dummy>
      class Pool
      {
        friend class PoolAlloc;

        private:

          static const int BLOCK_SIZE = 1024;

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
              return ( (Type*) data )[i];
            }

            const Type &get( int i ) const
            {
              return ( (const Type*) data )[i];
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

        public:

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

      static Pool<void> pool;

#ifdef OZ_USE_POOLALLOC
      /**
       * Get an empty slot from pool.
       * @param
       * @return
       */
      void *operator new ( uint )
      {
        return pool.alloc();
      }

      /**
       * No placement new.
       * @param
       * @param
       * @return
       */
      void *operator new ( uint, void* )
      {
        assert( false );

        return null;
      }

      /**
       * Delete object
       * @param object
       */
      void operator delete ( void *ptr )
      {
        pool.free( (Type*) ptr );
      }
#endif

  };

  template <class Type, int INDEX>
  PoolAlloc<Type, INDEX>::Pool<void> PoolAlloc<Type, INDEX>::pool;

}
