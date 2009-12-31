/*
 *  Pool.h
 *
 *  Pool memory allocator
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#ifdef OZ_POOL_ALLOC
/**
 * \def OZ_STATIC_POOL_ALLOC( pool )
 * Implement new and delete operators that (de)allocate for objects of that class from given pool.
 * As new/delete are static functions so has to be the given pool. The derived classes also
 * need to have overloaded new/delete otherwise the ones from the superclass will be used, which
 * will likely result in a crash (as those would allocate wrong amount of memory as size of both
 * classes is unlikely to be the same).
 */
#define OZ_STATIC_POOL_ALLOC( pool ) \
public:\
void* operator new ( uint ) { return pool.malloc(); } \
void operator delete ( void* ptr ) { pool.free( ptr ); }

#else

#define OZ_STATIC_POOL_ALLOC( pool )

#endif

/**
 * \def OZ_PLACEMENT_POOL_ALLOC( Type, INDEX )
 * Implement placement new operator, while non-placement new and delete are disabled.
 * The pool is given to new operator as an additional parameter. As delete cannot be provided,
 * object should be freed via <code>pool.free( object)</code> and a destructor should be called
 * manually before freeing.
 */
#define OZ_PLACEMENT_POOL_ALLOC( Type, INDEX, SIZE ) \
private: \
void* operator new ( uint ); \
public: \
void* operator new ( uint, Pool<Type, INDEX, SIZE> &pool ) { return pool.malloc(); } \
void operator delete ( void* );

namespace oz
{

  template <class Type, int INDEX = 0, int BLOCK_SIZE = 256>
  struct Pool
  {
    private:

      // no copying
      Pool( const Pool& );
      Pool& operator = ( const Pool& );

      /**
       * Memory block.
       * Block is an array that can hold up to BLOCK_SIZE elements. When we run out of space
       * we simply allocate another block. Once a block is allocated it cannot be freed any
       * more unless Pool is empty. Anyways, that would be rarely possible due to fragmentation.
       */
      struct Block
      {
        byte   data[BLOCK_SIZE * sizeof( Type )];
        Block* next[1];

        explicit Block()
        {
          for( int i = 0; i < BLOCK_SIZE - 1; i++ ) {
            get( i )->next[INDEX] = get( i + 1 );
          }
          get( BLOCK_SIZE - 1 )->next[INDEX] = null;
        }

        Type* get( int i )
        {
          return reinterpret_cast<Type*>( data ) + i;
        }

        const Type* get( int i ) const
        {
          return reinterpret_cast<const Type*>( data ) + i;
        }
      };

      // List of allocated blocks
      List<Block> blocks;
      // Last freed block, null if none
      Type*       freeSlot;
      // Size of data blocks
      int         size;
      // Number of used slots in the pool
      int         count;

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
      void* malloc()
      {
#ifdef OZ_POOL_ALLOC
        count++;

        if( freeSlot == null ) {
          blocks << new Block();
          freeSlot = blocks.first()->get( 1 );
          size += BLOCK_SIZE;
          return blocks.first()->get( 0 );
        }
        else {
          Type* slot = freeSlot;
          // static_cast to make it work with derived objects
          freeSlot = static_cast<Type*>( slot->next[INDEX] );
          return slot;
        }
#else
        return Alloc::malloc( sizeof( Type ) );
#endif
      }

      /**
       * Free given element.
       * @param index
       */
      void free( void* ptr )
      {
#ifdef OZ_POOL_ALLOC
        assert( count != 0 );

        Type* elem = reinterpret_cast<Type*>( ptr );
        elem->next[INDEX] = freeSlot;
        freeSlot = elem;
        count--;
#else
        Alloc::free( ptr );
#endif
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
