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
 * @file oz/HashIndex.hh
 *
 * HashIndex template class.
 */

#pragma once

#include "arrays.hh"
#include "Pool.hh"

namespace oz
{

/**
 * Chaining hashtable implementation with integer key.
 *
 * A prime number is recommended as size unless key distribution is truly "random".
 *
 * Memory is allocated when the first element is added.
 *
 * @ingroup oz
 */
template <typename Value = nullptr_t, int SIZE = 253>
class HashIndex
{
  static_assert( SIZE > 0, "HashIndex size must be at least 1" );

  private:

    /**
     * Internal class for key/value elements.
     */
    struct Elem
    {
      const int key;   ///< Key.
      Value     value; ///< Value.
      Elem*     next;  ///< Next element in a slot.

      /**
       * Initialise a new element.
       */
      template <typename Value_>
      OZ_ALWAYS_INLINE
      explicit Elem( int key_, Value_&& value_, Elem* next_ ) :
        key( key_ ), value( static_cast<Value_&&>( value_ ) ), next( next_ )
      {}

      OZ_PLACEMENT_POOL_ALLOC( Elem, SIZE )
    };

  public:

    /**
     * %Iterator with constant access to container elements.
     */
    class CIterator : public CIteratorBase<Elem>
    {
      friend class HashIndex;

      private:

        /// Base class type, convenience definition to make code cleaner.
        typedef CIteratorBase<Elem> B;

        const Elem* const* data;  ///< Pointer to hashtable slots.
        int                index; ///< Index of the current slot.

        /**
         * %Iterator for the given container, points to its first element.
         */
        explicit CIterator( const HashIndex& t ) :
          B( t.data[0] ), data( t.data ), index( 0 )
        {
          while( B::elem == null && index < SIZE - 1 ) {
            ++index;
            B::elem = data[index];
          }
        }

      public:

        /**
         * Default constructor, creates an invalid iterator.
         */
        OZ_ALWAYS_INLINE
        CIterator() :
          B( null )
        {}

        /**
         * Constant pointer to the current element's key.
         */
        OZ_ALWAYS_INLINE
        operator const int* () const
        {
          return &B::elem->key;
        }

        /**
         * Constant reference to the current element's key.
         */
        OZ_ALWAYS_INLINE
        const int& operator * () const
        {
          return B::elem->key;
        }

        /**
         * Constant reference to the current element's key.
         */
        OZ_ALWAYS_INLINE
        const int& key() const
        {
          return B::elem->key;
        }

        /**
         * Constant reference to the current element's value.
         */
        OZ_ALWAYS_INLINE
        const Value& value() const
        {
          return B::elem->value;
        }

        /**
         * Advance to the next element.
         */
        CIterator& operator ++ ()
        {
          hard_assert( B::elem != null );

          if( B::elem->next != null ) {
            B::elem = B::elem->next;
          }
          else if( index == SIZE - 1 ) {
            B::elem = null;
          }
          else {
            do {
              ++index;
              B::elem = data[index];
            }
            while( B::elem == null && index < SIZE - 1 );
          }
          return *this;
        }

    };

    /**
     * %Iterator with non-constant access to container elements.
     */
    class Iterator : public IteratorBase<Elem>
    {
      friend class HashIndex;

      private:

        /// Base class type, convenience definition to make code cleaner.
        typedef IteratorBase<Elem> B;

        Elem* const* data;  ///< Pointer to hashtable slots.
        int          index; ///< Index of the current slot.

        /**
         * %Iterator for the given container, points to its first element.
         */
        explicit Iterator( const HashIndex& t ) :
          B( t.data[0] ), data( t.data ), index( 0 )
        {
          while( B::elem == null && index < SIZE - 1 ) {
            ++index;
            B::elem = data[index];
          }
        }

      public:

        /**
         * Default constructor, creates an invalid iterator.
         */
        OZ_ALWAYS_INLINE
        Iterator() :
          B( null )
        {}

        /**
         * Constant pointer to the current element's key.
         */
        OZ_ALWAYS_INLINE
        operator const int* () const
        {
          return &B::elem->key;
        }

        /**
         * Constant reference to the current element's key.
         */
        OZ_ALWAYS_INLINE
        const int& operator * () const
        {
          return B::elem->key;
        }

        /**
         * Constant reference to the current element's key.
         */
        OZ_ALWAYS_INLINE
        const int& key() const
        {
          return B::elem->key;
        }

        /**
         * Constant reference to the current element's value.
         */
        OZ_ALWAYS_INLINE
        const Value& value() const
        {
          return B::elem->value;
        }

        /**
         * Reference to the current element's value.
         */
        OZ_ALWAYS_INLINE
        Value& value()
        {
          return B::elem->value;
        }

        /**
         * Advance to the next element.
         */
        Iterator& operator ++ ()
        {
          hard_assert( B::elem != null );

          if( B::elem->next != null ) {
            B::elem = B::elem->next;
          }
          else if( index == SIZE - 1 ) {
            B::elem = null;
          }
          else {
            do {
              ++index;
              B::elem = data[index];
            }
            while( B::elem == null && index < SIZE - 1 );
          }
          return *this;
        }

    };

  private:

    Elem*            data[SIZE]; ///< %Array of lists.
    Pool<Elem, SIZE> pool;       ///< Memory pool for elements.
    int              count;      ///< Number of elements.

    /**
     * True iff chains have same length and respective elements are equal.
     */
    static bool areChainsEqual( const Elem* chainA, const Elem* chainB )
    {
      while( chainA != null && chainB != null ) {
        if( chainA->key != chainB->key || chainA->value != chainB->value ) {
          return false;
        }

        chainA = chainA->next;
        chainB = chainB->next;
      }
      // At least one is null.
      return chainA == chainB;
    }

    /**
     * Allocate and make a copy of the given chain.
     */
    Elem* cloneChain( const Elem* chain )
    {
      Elem* newChain = null;

      while( chain != null ) {
        newChain = new( pool ) Elem( chain->key, chain->value, newChain );
        chain = chain->next;
      }
      return newChain;
    }

    /**
     * Delete all elements in the given chain.
     */
    void clearChain( Elem* chain )
    {
      while( chain != null ) {
        Elem* next = chain->next;

        chain->~Elem();
        pool.dealloc( chain );

        chain = next;
      }
    }

    /**
     * Delete all elements and referenced objects in the given chain.
     */
    void freeChain( Elem* chain )
    {
      while( chain != null ) {
        Elem* next = chain->next;

        delete chain->value;
        chain->~Elem();
        pool.dealloc( chain );

        chain = next;
      }
    }

  public:

    /**
     * Create an empty hashtable.
     */
    HashIndex() :
      count( 0 )
    {
      aSet<Elem*>( data, null, SIZE );
    }

    /**
     * Destructor.
     */
    ~HashIndex()
    {
      clear();
      dealloc();
    }

    /**
     * Copy constructor, copies elements and storage.
     */
    HashIndex( const HashIndex& t ) :
      count( t.count )
    {
      for( int i = 0; i < SIZE; ++i ) {
        data[i] = cloneChain( t.data[i] );
      }
    }

    /**
     * Move constructor, moves storage.
     */
    HashIndex( HashIndex&& t ) :
      pool( static_cast< Pool<Elem, SIZE>&& >( t.pool ) ), count( t.count )
    {
      aCopy<Elem*>( data, t.data, SIZE );

      aSet<Elem*>( t.data, null, SIZE );
      t.count = 0;
    }

    /**
     * Copy operator, copies elements and storage.
     */
    HashIndex& operator = ( const HashIndex& t )
    {
      if( &t == this ) {
        return *this;
      }

      for( int i = 0; i < SIZE; ++i ) {
        clearChain( data[i] );
        data[i] = cloneChain( t.data[i] );
      }
      count = t.count;

      return *this;
    }

    /**
     * Move operator, moves storage.
     */
    HashIndex& operator = ( HashIndex&& t )
    {
      if( &t == this ) {
        return *this;
      }

      clear();

      aCopy<Elem*>( data, t.data, SIZE );
      pool  = static_cast< Pool<Elem, SIZE>&& >( t.pool );
      count = t.count;

      aSet<Elem*>( t.data, null, SIZE );
      t.count = 0;

      return *this;
    }

    /**
     * True iff respective elements are equal (including chain order).
     */
    bool operator == ( const HashIndex& t ) const
    {
      if( count != t.count ) {
        return false;
      }
      for( int i = 0; i < SIZE; ++i ) {
        if( !areChainsEqual( data[i], t.data[i] ) ) {
          return false;
        }
      }
      return true;
    }

    /**
     * False iff respective elements are equal (including chain order!).
     */
    bool operator != ( const HashIndex& t ) const
    {
      if( count != t.count ) {
        return true;
      }
      for( int i = 0; i < SIZE; ++i ) {
        if( !areChainsEqual( data[i], t.data[i] ) ) {
          return true;
        }
      }
      return false;
    }

    /**
     * %Iterator with constant access, initially points to the first element.
     */
    OZ_ALWAYS_INLINE
    CIterator citer() const
    {
      return CIterator( *this );
    }

    /**
     * %Iterator with non-constant access, initially points to the first element.
     */
    OZ_ALWAYS_INLINE
    Iterator iter() const
    {
      return Iterator( *this );
    }

    /**
     * Number of elements.
     */
    OZ_ALWAYS_INLINE
    int length() const
    {
      return count;
    }

    /**
     * True iff empty.
     */
    OZ_ALWAYS_INLINE
    bool isEmpty() const
    {
      return count == 0;
    }

    /**
     * Number of allocated elements.
     */
    OZ_ALWAYS_INLINE
    int capacity() const
    {
      return SIZE;
    }

    /**
     * Length divided by capacity.
     */
    float loadFactor() const
    {
      return float( count ) / float( SIZE );
    }

    /**
     * True iff the given key is found in the hashtable.
     */
    bool contains( int key ) const
    {
      uint  i = uint( key ) % uint( SIZE );
      Elem* p = data[i];

      while( p != null ) {
        if( p->key == key ) {
          return true;
        }

        p = p->next;
      }
      return false;
    }

    /**
     * If the key exists, return constant pointer to its value, otherwise return <tt>null</tt>.
     */
    const Value* find( int key ) const
    {
      uint  i = uint( key ) % uint( SIZE );
      Elem* p = data[i];

      while( p != null ) {
        if( p->key == key ) {
          return &p->value;
        }

        p = p->next;
      }
      return null;
    }

    /**
     * If the key exists, return pointer to its value, otherwise return <tt>null</tt>.
     */
    Value* find( int key )
    {
      uint  i = uint( key ) % uint( SIZE );
      Elem* p = data[i];

      while( p != null ) {
        if( p->key == key ) {
          return &p->value;
        }

        p = p->next;
      }
      return null;
    }

    /**
     * Add a new element, if the key already exists in the hashtable overwrite existing element.
     *
     * @return Pointer to the value of the inserted element.
     */
    template <typename Value_ = Value>
    Value* add( int key, Value_&& value = Value() )
    {
      uint  i = uint( key ) % uint( SIZE );
      Elem* p = data[i];

      while( p != null ) {
        if( p->key == key ) {
          p->value = static_cast<Value_&&>( value );
          return &p->value;
        }

        p = p->next;
      }

      data[i] = new( pool ) Elem( key, static_cast<Value_&&>( value ), data[i] );
      ++count;

      soft_assert( loadFactor() < 0.75f );

      return &data[i]->value;
    }

    /**
     * Add a new element if the key does not exist in the hashtable.
     *
     * @return Pointer to the value of the inserted or the existing element with the same key.
     */
    template <typename Value_ = Value>
    Value* include( int key, Value_&& value = Value() )
    {
      uint  i = uint( key ) % uint( SIZE );
      Elem* p = data[i];

      while( p != null ) {
        if( p->key == key ) {
          return &p->value;
        }

        p = p->next;
      }

      data[i] = new( pool ) Elem( key, static_cast<Value_&&>( value ), data[i] );
      ++count;

      soft_assert( loadFactor() < 0.75f );

      return &data[i]->value;
    }

    /**
     * Remove element with the given key.
     *
     * @return True iff the key was found (and removed).
     */
    bool exclude( int key )
    {
      uint   i    = uint( key ) % uint( SIZE );
      Elem*  p    = data[i];
      Elem** prev = &data[i];

      while( p != null ) {
        if( p->key == key ) {
          *prev = p->next;
          --count;

          p->~Elem();
          pool.dealloc( p );

          return true;
        }

        prev = &p->next;
        p = p->next;
      }
      return false;
    }

    /**
     * Empty the hashtable.
     */
    void clear()
    {
      for( int i = 0; i < SIZE; ++i ) {
        clearChain( data[i] );
        data[i] = null;
      }
      count = 0;
    }

    /**
     * Delete all objects referenced by element values and empty the hashtable.
     */
    void free()
    {
      for( int i = 0; i < SIZE; ++i ) {
        freeChain( data[i] );
        data[i] = null;
      }
      count = 0;
    }

    /**
     * Deallocate pool memory of an empty hastable.
     */
    void dealloc()
    {
      hard_assert( count == 0 );

      pool.free();
    }

};

}
