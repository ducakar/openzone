/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file ozCore/HashSet.hh
 *
 * `HashSet` class template.
 */

#pragma once

#include "System.hh"
#include "Pool.hh"

namespace oz
{

/**
 * Chaining hashtable implementation, containing only keys without values.
 *
 * Memory is allocated when the first element is added.
 *
 * @sa `oz::HashMap`, `oz::Set`
 */
template <typename Key>
class HashSet
{
private:

  /// Granularity for automatic storage allocations.
  static const int GRANULARITY = 8;

private:

  /**
   * Key-only element entry.
   */
  struct Elem
  {
    Elem* next;   ///< Next element in a slot.
    int   hash;   ///< Cached hash.
    Key   key;    ///< Key.

    OZ_PLACEMENT_POOL_ALLOC( Elem, 256 )
  };

  /**
   * Hashtable iterator.
   */
  template <class IterElem, class KeyType>
  class HashIterator : public IteratorBase<IterElem>
  {
  private:

    using IteratorBase<IterElem>::elem;

    const HashSet* table; ///< Hashtable that is being iterated.
    int            index; ///< Index of the current bucket.

  public:

    /**
     * Default constructor, creates an invalid iterator.
     */
    OZ_ALWAYS_INLINE
    explicit HashIterator() :
      IteratorBase<IterElem>( nullptr ), table( nullptr ), index( 0 )
    {}

    /**
     * Create hashtable iterator, initially pointing to the first hashtable element.
     */
    explicit HashIterator( const HashSet& ht ) :
      IteratorBase<IterElem>( ht.size == 0 ? nullptr : ht.data[0] ), table( &ht ), index( 0 )
    {
      while( elem == nullptr && index < table->size - 1 ) {
        ++index;
        elem = table->data[index];
      }
    }

    /**
     * Pointer to the current key.
     */
    OZ_ALWAYS_INLINE
    operator KeyType* () const
    {
      return &elem->key;
    }

    /**
     * Reference to the current key.
     */
    OZ_ALWAYS_INLINE
    KeyType& operator * () const
    {
      return elem->key;
    }

    /**
     * Access to the current key's member.
     */
    OZ_ALWAYS_INLINE
    KeyType* operator -> () const
    {
      return &elem->key;
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
      else if( index == table->size - 1 ) {
        elem = nullptr;
      }
      else {
        do {
          ++index;
          elem = table->data[index];
        }
        while( elem == nullptr && index < table->size - 1 );
      }
      return *this;
    }

  };

public:

  /**
   * %Iterator with constant access to elements.
   */
  typedef HashIterator<const Elem, const Key> CIterator;

  /**
   * %Iterator with non-constant access to elements.
   */
  typedef HashIterator<Elem, Key> Iterator;

private:

  Pool<Elem> pool; ///< Memory pool for elements.
  Elem**     data; ///< %Array of buckets, each containing a linked list.
  int        size; ///< Number of buckets.

  /**
   * True iff a bucket chains have the same length and contain equal elements.
   */
  static bool areChainsEqual( const Elem* chainA, const Elem* chainB )
  {
    const Elem* firstB = chainB;

    while( chainA != nullptr && chainB != nullptr ) {
      Elem* i = firstB;
      do {
        if( i->key == chainA->key ) {
          goto nextElem;
        }
        i = i->next;
      }
      while( i != nullptr );

      return false;

nextElem:
      chainA = chainA->next;
      chainB = chainB->next;
    }
    // At least one is nullptr, so this is true iff both chains have the same length.
    return chainA == chainB;
  }

  /**
   * Allocate and make a copy of a given bucket chain.
   */
  Elem* cloneChain( const Elem* chain )
  {
    Elem* newChain = nullptr;

    while( chain != nullptr ) {
      newChain = new( pool ) Elem { newChain, chain->hash, chain->key };
      chain = chain->next;
    }
    return newChain;
  }

  /**
   * Delete all elements in a given bucket chain.
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
   * Resize bucket array and rebuild hashtable.
   */
  void resize( int newSize )
  {
    Elem** newData = new Elem*[newSize] {};

    // Rebuild hashtable.
    for( int i = 0; i < size; ++i ) {
      Elem* e    = data[i];
      Elem* next = nullptr;

      while( e != nullptr ) {
        uint index = uint( e->hash ) % uint( newSize );

        next = e->next;
        e->next = newData[index];
        newData[index] = e;

        e = next;
      }
    }

    delete[] data;

    data = newData;
    size = newSize;
  }

  /**
   * Ensure a given bucket array size.
   *
   * Size is doubled if neccessary. If that doesn't suffice it is set to the least multiple of
   * `GRANULARITY` greater or equal to the requested size.
   */
  void ensureCapacity( int capacity )
  {
    if( capacity < 0 ) {
      OZ_ERROR( "oz::HashMap: Capacity overflow" );
    }
    else if( size < capacity ) {
      int newSize = size * 2;
      if( newSize < capacity ) {
        newSize = ( capacity + GRANULARITY - 1 ) & ~( GRANULARITY - 1 );
      }
      if( newSize <= 0 ) {
        OZ_ERROR( "oz::HashMap: Capacity overflow" );
      }

      resize( newSize );
    }
  }

public:

  /**
   * Create an empty hashtable.
   */
  explicit HashSet() :
    data( nullptr ), size( 0 )
  {}

  /**
   * Initialise from an initialiser list.
   */
  HashSet( InitialiserList<Key> l ) :
    data( nullptr ), size( 0 )
  {
    for( const Key& key : l ) {
      add( key );
    }
  }

  /**
   * Destructor.
   */
  ~HashSet()
  {
    clear();
    deallocate();
  }

  /**
   * Copy constructor, copies elements and storage.
   */
  HashSet( const HashSet& ht ) :
    data( new Elem*[ht.size] ), size( ht.size )
  {
    for( int i = 0; i < ht.size; ++i ) {
      data[i] = cloneChain( ht.data[i] );
    }
  }

  /**
   * Move constructor, moves storage.
   */
  HashSet( HashSet&& ht ) :
    pool( static_cast<Pool<Elem>&&>( ht.pool ) ), data( ht.data ), size( ht.size )
  {
    ht.data = nullptr;
    ht.size = 0;
  }

  /**
   * Copy operator, copies elements and storage.
   */
  HashSet& operator = ( const HashSet& ht )
  {
    if( &ht == this ) {
      return *this;
    }

    clear();

    if( size < ht.pool.count ) {
      delete[] data;

      data = new Elem[ht.size];
      size = ht.size;
    }

    for( int i = 0; i < size; ++i ) {
      data[i] = i < ht.size ? cloneChain( ht.data[i] ) : nullptr;
    }

    return *this;
  }

  /**
   * Move operator, moves storage.
   */
  HashSet& operator = ( HashSet&& ht )
  {
    if( &ht == this ) {
      return *this;
    }

    clear();
    delete[] data;

    pool = static_cast< Pool<Elem>&& >( ht.pool );
    data = ht.data;
    size = ht.size;

    ht.data = nullptr;
    ht.size = 0;

    return *this;
  }

  /**
   * True iff contained elements are equal.
   */
  bool operator == ( const HashSet& ht ) const
  {
    if( pool.length() != ht.pool.length() ) {
      return false;
    }

    for( int i = 0; i < size; ++i ) {
      if( !areChainsEqual( data[i], ht.data[i] ) ) {
        return false;
      }
    }
    return true;
  }

  /**
   * False iff contained elements are equal.
   */
  bool operator != ( const HashSet& ht ) const
  {
    return !operator == ( ht );
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
  Iterator iter()
  {
    return Iterator( *this );
  }

  /**
   * STL-compatible constant begin iterator.
   */
  OZ_ALWAYS_INLINE
  CIterator begin() const
  {
    return CIterator( *this );
  }

  /**
   * STL-compatible begin iterator.
   */
  OZ_ALWAYS_INLINE
  Iterator begin()
  {
    return Iterator( *this );
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
    return float( pool.length() ) / float( size );
  }

  /**
   * True iff a given key is found in the hashtable.
   */
  template <typename Key_ = Key>
  bool contains( const Key_& key ) const
  {
    if( size == 0 ) {
      return false;
    }

    int   h = hash( key );
    uint  i = uint( h ) % uint( size );
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
   * Add a new element, if the key already exists in the hashtable overwrite existing element.
   */
  template <typename Key_ = Key>
  void add( Key_&& key )
  {
    ensureCapacity( pool.length() + 1 );

    int   h = hash( key );
    uint  i = uint( h ) % uint( size );
    Elem* e = data[i];

    while( e != nullptr ) {
      if( e->key == key ) {
        e->key = static_cast<Key_&&>( key );
        return;
      }
      e = e->next;
    }

    data[i] = new( pool ) Elem { data[i], h, static_cast<Key_&&>( key ) };
  }

  /**
   * Add a new element if the key does not exist in the hashtable.
   */
  template <typename Key_ = Key>
  void include( Key_&& key )
  {
    ensureCapacity( pool.length() + 1 );

    int   h = hash( key );
    uint  i = uint( h ) % uint( size );
    Elem* e = data[i];

    while( e != nullptr ) {
      if( e->key == key ) {
        return;
      }
      e = e->next;
    }

    data[i] = new( pool ) Elem { data[i], h, static_cast<Key_&&>( key ) };
  }

  /**
   * Remove element with a given key.
   *
   * @return True iff the key was found (and removed).
   */
  bool exclude( const Key& key )
  {
    if( size == 0 ) {
      return nullptr;
    }

    uint   i     = uint( hash( key ) ) % uint( size );
    Elem*  e     = data[i];
    Elem** pPrev = &data[i];

    while( e != nullptr ) {
      if( e->key == key ) {
        *pPrev = e->next;

        e->~Elem();
        pool.deallocate( e );

        return true;
      }

      pPrev = &e->next;
      e = e->next;
    }
    return false;
  }

  /**
   * Trim the bucket array size to 4/3 of the current number of elements.
   */
  void trim()
  {
    int capacity = pool.count * 4 / 3;

    if( size > capacity ) {
      ensureCapacity( capacity );
    }
  }

  /**
   * Clear the hashtable.
   */
  void clear()
  {
    for( int i = 0; i < size; ++i ) {
      clearChain( data[i] );
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
