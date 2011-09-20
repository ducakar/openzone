/*
 *  Pool.hpp
 *
 *  Pool memory allocator
 *  It is only used if OZ_POOL_ALLOC is enabled.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "common.hpp"

#ifdef OZ_POOL_ALLOC
/**
 * \def OZ_STATIC_POOL_ALLOC( pool )
 * Implement new and delete operators that (de)allocate objects of that class from the given pool.
 * As new/delete are static functions so has to be the given pool. The derived classes also
 * need to have overloaded new/delete otherwise the ones from the superclass will be used, which
 * will likely result in a crash (as those would allocate wrong amount of memory as size of both
 * classes is unlikely to be the same).
 */
#define OZ_STATIC_POOL_ALLOC( pool ) \
public:\
  void* operator new ( size_t ) { return pool.alloc(); } \
  void  operator delete ( void* ptr ) { pool.dealloc( ptr ); }

#else

#define OZ_STATIC_POOL_ALLOC( pool )

#endif

/**
 * \def OZ_PLACEMENT_POOL_ALLOC( Type, SIZE )
 * Implement placement new operator, while non-placement new and delete are disabled.
 * The pool is given to new operator as an additional parameter. As delete cannot be provided,
 * object should be freed via <code>pool.dealloc( object )</code> and the destructor should be
 * called manually before freeing.
 */
#define OZ_PLACEMENT_POOL_ALLOC( Type, SIZE ) \
public: \
  void* operator new ( size_t, oz::Pool<Type, SIZE>& pool ) { return pool.alloc(); } \
  void  operator delete ( void* ptr, oz::Pool<Type, SIZE>& pool ) { pool.dealloc( ptr ); } \
private: \
  void* operator new ( size_t ); \
  void  operator delete ( void* );

namespace oz
{

  template <class Type, int BLOCK_SIZE = 256>
  class Pool
  {
    static_assert( BLOCK_SIZE >= 2, "Pool block size must be at least 2" );

    private:

      /**
       * Slot that occupies memory for an object. It also provides a pointer to the next slot
       * in a block.
       */
      union Slot
      {
        char  content[ sizeof( Type ) ];
        Slot* nextSlot;
      };

      /**
       * Memory block.
       * Block is an array that can hold up to BLOCK_SIZE elements. When we run out of space
       * we simply allocate another block. Once a block is allocated it cannot be freed any
       * more unless Pool is empty. Anyways, that would be rarely possible due to fragmentation.
       */
      struct Block
      {
        Slot   data[BLOCK_SIZE];
        Block* next;

        explicit Block( Block* next_ ) : next( next_ )
        {
          for( int i = 0; i < BLOCK_SIZE - 1; ++i ) {
            data[i].nextSlot = &data[i + 1];
          }
          data[BLOCK_SIZE - 1].nextSlot = null;
        }
      };

      // List of allocated blocks
      Block* firstBlock;
      // List of freed slots, null if none
      Slot*  freeSlot;
      // Size of data blocks
      int    size;
      // Number of occupied used slots in the pool
      int    count;

    public:

      /**
       * Create empty pool, storage is allocated on first element addition.
       * @param initSize
       */
      Pool() : firstBlock( null ), freeSlot( null ), size( 0 ), count( 0 )
      {}

      /**
       * Destructor.
       */
      ~Pool()
      {
        // there's a memory leak if count != 0
        hard_assert( count == 0 && size == 0 );
      }

    private:

      /**
       * No copying.
       * @param
       */
      Pool( const Pool& );

      /**
       * No copying.
       * @param
       * @return
       */
      Pool& operator = ( const Pool& );

    public:

      /**
       * Allocate a new element.
       * @return
       */
      void* alloc()
      {
#ifdef OZ_POOL_ALLOC
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
#else
        return new char[ sizeof( Type ) ];
#endif
      }

      /**
       * Free given element.
       * @param index
       */
      void dealloc( void* ptr )
      {
#ifdef OZ_POOL_ALLOC
        hard_assert( count != 0 );

        Slot* slot = reinterpret_cast<Slot*>( ptr );

# ifndef NDEBUG
        __builtin_memset( slot, 0xee, sizeof( Slot ) );
# endif

        slot->nextSlot = freeSlot;
        freeSlot = slot;
        --count;
#else
        delete[] reinterpret_cast<char*>( ptr );
#endif
      }

      /**
       * @return number of used slots in the pool
       */
      OZ_ALWAYS_INLINE
      int length() const
      {
        return count;
      }

      /**
       * @return true if pool has no used slots
       */
      OZ_ALWAYS_INLINE
      bool isEmpty() const
      {
        return count == 0;
      }

      /**
       * @return capacity of the pool
       */
      OZ_ALWAYS_INLINE
      int capacity() const
      {
        return size;
      }

      /**
       * Free the pool.
       * Frees all blocks allocated. It won't end good if something still uses memory allocated by
       * the pool.
       */
      void free()
      {
        hard_assert( count == 0 );

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
