/*
 * ozCore - OpenZone Core Library.
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
 * @file ozCore/HashMap.hh
 *
 * Hashtable template class.
 */

#pragma once

#include "Pool.hh"

namespace oz
{

/**
 * Chaining hashtable implementation.
 *
 * Memory is allocated when the first element is added.
 *
 * @sa `oz::HashSet`, `oz::Map`
 */
template <typename Key, typename Value, int SIZE = 256>
class HashMap
{
  static_assert( SIZE > 0, "HashMap size must be at least 1" );

  public:

    /**
     * Key-value element entry.
     */
    class Elem
    {
      friend class HashMap;

      private:

        Elem* next;  ///< Next element in a slot.

      public:

        Key   key;   ///< Key.
        Value value; ///< Value.

      private:

        /**
         * Initialise a new element.
         */
        template <typename Key_ = Key, typename Value_ = Value>
        OZ_ALWAYS_INLINE
        explicit Elem( Elem* next_, Key_&& key_, Value_&& value_ ) :
          next( next_ ), key( static_cast<Key_&&>( key_ ) ),
          value( static_cast<Value_&&>( value_ ) )
        {}

        OZ_PLACEMENT_POOL_ALLOC( Elem, SIZE )
    };

  private:

    /**
     * Hashtable iterator.
     */
    template <class IterElem>
    class HashIterator : public IteratorBase<IterElem>
    {
      private:

        using IteratorBase<IterElem>::elem;

        IterElem* const* data;  ///< Pointer to hashtable slots.
        int              index; ///< Index of the current slot.

      public:

        /**
         * Default constructor, creates an invalid iterator.
         */
        OZ_ALWAYS_INLINE
        explicit HashIterator() :
          IteratorBase<IterElem>( nullptr ), data( nullptr ), index( 0 )
        {}

        /**
         * Create hashtable iterator, initially pointing to the first hashtable element.
         *
         * @param data_ internal array of linked lists of a hastable.
         */
        explicit HashIterator( IterElem* const* data_ ) :
          IteratorBase<IterElem>( data_[0] ), data( data_ ), index( 0 )
        {
          while( elem == nullptr && index < SIZE - 1 ) {
            ++index;
            elem = data[index];
          }
        }

        /**
         * Advance to the next element.
         */
        HashIterator& operator ++ ()
        {
          hard_assert( elem != nullptr );

          if( elem->next != nullptr ) {
            elem = elem->next;
          }
          else if( index == SIZE - 1 ) {
            elem = nullptr;
          }
          else {
            do {
              ++index;
              elem = data[index];
            }
            while( elem == nullptr && index < SIZE - 1 );
          }
          return *this;
        }

    };

  public:

    /**
     * %Iterator with constant access to container elements.
     */
    typedef HashIterator<const Elem> CIterator;

    /**
     * %Iterator with non-constant access to container elements.
     */
    typedef HashIterator<Elem> Iterator;

  private:

    Elem*            data[SIZE]; ///< %Array of linked lists.
    Pool<Elem, SIZE> pool;       ///< Memory pool for elements.

    /**
     * True iff chains have same length and respective elements are equal.
     */
    static bool areChainsEqual( const Elem* chainA, const Elem* chainB )
    {
      while( chainA != nullptr && chainB != nullptr ) {
        if( chainA->key != chainB->key || chainA->value != chainB->value ) {
          return false;
        }

        chainA = chainA->next;
        chainB = chainB->next;
      }
      // At least one is nullptr.
      return chainA == chainB;
    }

    /**
     * Allocate and make a copy of the given chain.
     */
    Elem* cloneChain( const Elem* chain )
    {
      Elem* newChain = nullptr;

      while( chain != nullptr ) {
        newChain = new( pool ) Elem( newChain, chain->key, chain->value );
        chain = chain->next;
      }
      return newChain;
    }

    /**
     * Delete all elements in the given chain.
     */
    void clearChain( Elem* chain )
    {
      while( chain != nullptr ) {
        Elem* next = chain->next;

        chain->~Elem();
        pool.deallocate( chain );

        chain = next;
      }
    }

    /**
     * Delete all elements and referenced objects in the given chain.
     */
    void freeChain( Elem* chain )
    {
      while( chain != nullptr ) {
        Elem* next = chain->next;

        delete chain->value;
        chain->~Elem();
        pool.deallocate( chain );

        chain = next;
      }
    }

  public:

    /**
     * Create an empty hashtable.
     */
    explicit HashMap()
    {
      aFill<Elem*, Elem*>( data, nullptr, SIZE );
    }

    /**
     * Destructor.
     */
    ~HashMap()
    {
      clear();
      deallocate();
    }

    /**
     * Copy constructor, copies elements and storage.
     */
    HashMap( const HashMap& hm )
    {
      for( int i = 0; i < SIZE; ++i ) {
        data[i] = cloneChain( hm.data[i] );
      }
    }

    /**
     * Move constructor, moves storage.
     */
    HashMap( HashMap&& hm ) :
      pool( static_cast< Pool<Elem, SIZE>&& >( hm.pool ) )
    {
      aCopy<Elem*>( data, hm.data, SIZE );
      aFill<Elem*, Elem*>( hm.data, nullptr, SIZE );
    }

    /**
     * Copy operator, copies elements and storage.
     */
    HashMap& operator = ( const HashMap& hm )
    {
      if( &hm == this ) {
        return *this;
      }

      for( int i = 0; i < SIZE; ++i ) {
        clearChain( data[i] );
        data[i] = cloneChain( hm.data[i] );
      }
      return *this;
    }

    /**
     * Move operator, moves storage.
     */
    HashMap& operator = ( HashMap&& hm )
    {
      if( &hm == this ) {
        return *this;
      }

      clear();

      aCopy<Elem*>( data, hm.data, SIZE );
      aFill<Elem*, Elem*>( hm.data, nullptr, SIZE );
      pool = static_cast< Pool<Elem, SIZE>&& >( hm.pool );

      return *this;
    }

    /**
     * True iff respective elements are equal (including chain order).
     */
    bool operator == ( const HashMap& hs ) const
    {
      if( pool.length() != hs.pool.length() ) {
        return false;
      }

      for( int i = 0; i < SIZE; ++i ) {
        if( !areChainsEqual( data[i], hs.data[i] ) ) {
          return false;
        }
      }
      return true;
    }

    /**
     * False iff respective elements are equal (including chain order!).
     */
    bool operator != ( const HashMap& hm ) const
    {
      if( pool.length() != hm.pool.length() ) {
        return true;
      }

      for( int i = 0; i < SIZE; ++i ) {
        if( !areChainsEqual( data[i], hm.data[i] ) ) {
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
      return CIterator( data );
    }

    /**
     * %Iterator with non-constant access, initially points to the first element.
     */
    OZ_ALWAYS_INLINE
    Iterator iter() const
    {
      return Iterator( data );
    }

    /**
     * STL-compatible constant begin iterator.
     */
    OZ_ALWAYS_INLINE
    CIterator begin() const
    {
      return CIterator( data );
    }

    /**
     * STL-compatible begin iterator.
     */
    OZ_ALWAYS_INLINE
    Iterator begin()
    {
      return Iterator( data );
    }

    /**
     * STL-compatible constant end iterator.
     */
    OZ_ALWAYS_INLINE
    CIterator end() const
    {
      return CIterator();
    }

    /**
     * STL-compatible end iterator.
     */
    OZ_ALWAYS_INLINE
    Iterator end()
    {
      return Iterator();
    }

    /**
     * Number of elements.
     */
    OZ_ALWAYS_INLINE
    int length() const
    {
      return pool.length();
    }

    /**
     * True iff empty.
     */
    OZ_ALWAYS_INLINE
    bool isEmpty() const
    {
      return pool.isEmpty();
    }

    /**
     * Current size of memory pool for elements.
     */
    OZ_ALWAYS_INLINE
    int capacity() const
    {
      return pool.capacity();
    }

    /**
     * Number of elements divided by hashtable index size.
     */
    float loadFactor() const
    {
      return float( pool.length() ) / float( SIZE );
    }

    /**
     * True iff the given key is found in the hashtable.
     */
    bool contains( const Key& key ) const
    {
      uint  i = uint( hash( key ) ) % uint( SIZE );
      Elem* e = data[i];

      while( e != nullptr ) {
        if( e->key == key ) {
          return true;
        }

        e = e->next;
      }
      return false;
    }

    /**
     * If the key exists, return constant pointer to its value, otherwise return `nullptr`.
     */
    const Value* find( const Key& key ) const
    {
      uint  i = uint( hash( key ) ) % uint( SIZE );
      Elem* e = data[i];

      while( e != nullptr ) {
        if( e->key == key ) {
          return &e->value;
        }

        e = e->next;
      }
      return nullptr;
    }

    /**
     * If the key exists, return pointer to its value, otherwise return `nullptr`.
     */
    Value* find( const Key& key )
    {
      uint  i = uint( hash( key ) ) % uint( SIZE );
      Elem* e = data[i];

      while( e != nullptr ) {
        if( e->key == key ) {
          return &e->value;
        }

        e = e->next;
      }
      return nullptr;
    }

    /**
     * Add a new element, if the key already exists in the hashtable overwrite existing element.
     *
     * @return Reference to the value of the inserted element.
     */
    template <typename Key_ = Key, typename Value_ = Value>
    Value& add( Key_&& key, Value_&& value )
    {
      uint  i = uint( hash( key ) ) % uint( SIZE );
      Elem* e = data[i];

      while( e != nullptr ) {
        if( e->key == key ) {
          e->key   = static_cast<Key_&&>( key );
          e->value = static_cast<Value_&&>( value );
          return e->value;
        }

        e = e->next;
      }

      data[i] = new( pool ) Elem( data[i], static_cast<Key_&&>( key ),
                                  static_cast<Value_&&>( value ) );
      soft_assert( loadFactor() < 0.75f );

      return data[i]->value;
    }

    /**
     * Add a new element if the key does not exist in the hashtable.
     *
     * @return Reference to the value of the inserted or the existing element with the same key.
     */
    template <typename Key_ = Key, typename Value_ = Value>
    Value& include( Key_&& key, Value_&& value )
    {
      uint  i = uint( hash( key ) ) % uint( SIZE );
      Elem* e = data[i];

      while( e != nullptr ) {
        if( e->key == key ) {
          return e->value;
        }

        e = e->next;
      }

      data[i] = new( pool ) Elem( data[i], static_cast<Key_&&>( key ),
                                  static_cast<Value_&&>( value ) );
      soft_assert( loadFactor() < 0.75f );

      return data[i]->value;
    }

    /**
     * Remove element with the given key.
     *
     * @return True iff the key was found (and removed).
     */
    bool exclude( const Key& key )
    {
      uint   i    = uint( hash( key ) ) % uint( SIZE );
      Elem*  e    = data[i];
      Elem** prev = &data[i];

      while( e != nullptr ) {
        if( e->key == key ) {
          *prev = e->next;

          e->~Elem();
          pool.deallocate( e );

          return true;
        }

        prev = &e->next;
        e = e->next;
      }
      return false;
    }

    /**
     * Clear the hashtable.
     */
    void clear()
    {
      for( int i = 0; i < SIZE; ++i ) {
        clearChain( data[i] );
        data[i] = nullptr;
      }
    }

    /**
     * Delete all objects referenced by element values and clear the hashtable.
     */
    void free()
    {
      for( int i = 0; i < SIZE; ++i ) {
        freeChain( data[i] );
        data[i] = nullptr;
      }
    }

    /**
     * Deallocate pool memory of an empty hashtable.
     */
    void deallocate()
    {
      hard_assert( pool.isEmpty() );

      pool.free();
    }

};

}
