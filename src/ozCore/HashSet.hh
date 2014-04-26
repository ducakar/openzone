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
template <typename Elem>
class HashSet
{
protected:

  /// Granularity for automatic storage allocations.
  static const int GRANULARITY = 8;

protected:

  /**
   * Bucket.
   */
  struct Bucket
  {
    Bucket* next;   ///< Next bucket in a slot.
    int     hash;   ///< Cached hash.
    Elem    elem;   ///< Element/key.

    OZ_PLACEMENT_POOL_ALLOC( Bucket, 256 )
  };

  /**
   * Hashtable iterator.
   */
  template <class BucketType, class ElemType>
  class HashIterator : public IteratorBase<BucketType>
  {
  private:

    using IteratorBase<BucketType>::elem;

    const HashSet* table; ///< Hashtable that is being iterated.
    int            index; ///< Index of the current bucket.

  public:

    /**
     * Default constructor, creates an invalid iterator.
     */
    OZ_ALWAYS_INLINE
    explicit HashIterator() :
      IteratorBase<BucketType>( nullptr ), table( nullptr ), index( 0 )
    {}

    /**
     * Create hashtable iterator, initially pointing to the first hashtable element.
     */
    explicit HashIterator( const HashSet& ht ) :
      IteratorBase<BucketType>( ht.size == 0 ? nullptr : ht.data[0] ), table( &ht ), index( 0 )
    {
      while( elem == nullptr && index < table->size - 1 ) {
        ++index;
        elem = table->data[index];
      }
    }

    /**
     * Pointer to the current element.
     */
    OZ_ALWAYS_INLINE
    operator ElemType* () const
    {
      return &elem->elem;
    }

    /**
     * Reference to the current element.
     */
    OZ_ALWAYS_INLINE
    ElemType& operator * () const
    {
      return elem->elem;
    }

    /**
     * Access to the current element's member.
     */
    OZ_ALWAYS_INLINE
    ElemType* operator -> () const
    {
      return &elem->elem;
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

    /**
     * STL-style begin iterator.
     */
    OZ_ALWAYS_INLINE
    HashIterator begin() const
    {
      return *this;
    }

    /**
     * STL-style end iterator.
     */
    OZ_ALWAYS_INLINE
    HashIterator end() const
    {
      return HashIterator();
    }

  };

public:

  /**
   * %Iterator with constant access to elements.
   */
  typedef HashIterator<const Bucket, const Elem> CIterator;

  /**
   * %Iterator with non-constant access to elements.
   */
  typedef HashIterator<Bucket, Elem> Iterator;

protected:

  Pool<Bucket> pool; ///< Memory pool for elements.
  Bucket**     data; ///< %Array of buckets, each containing a linked list.
  int          size; ///< Number of buckets.

protected:

  /**
   * True iff a bucket chains have the same length and contain equal elements.
   */
  static bool areChainsEqual( const Bucket* chainA, const Bucket* chainB )
  {
    const Bucket* firstB = chainB;

    while( chainA != nullptr && chainB != nullptr ) {
      const Bucket* i = firstB;
      do {
        if( i->elem == chainA->elem ) {
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
  Bucket* cloneChain( const Bucket* chain )
  {
    Bucket* newChain = nullptr;

    while( chain != nullptr ) {
      newChain = new( pool ) Bucket { newChain, chain->hash, chain->elem };
      chain = chain->next;
    }
    return newChain;
  }

  /**
   * Delete all elements in a given bucket chain.
   */
  void clearChain( Bucket* chain )
  {
    while( chain != nullptr ) {
      Bucket* next = chain->next;

      chain->~Bucket();
      pool.deallocate( chain );

      chain = next;
    }
  }

  /**
   * Resize bucket array and rebuild hashtable.
   */
  void resize( int newSize )
  {
    Bucket** newData = new Bucket*[newSize] {};

    // Rebuild hashtable.
    for( int i = 0; i < size; ++i ) {
      Bucket* e    = data[i];
      Bucket* next = nullptr;

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
  HashSet() :
    data( nullptr ), size( 0 )
  {}

  /**
   * Initialise from an initialiser list.
   */
  HashSet( InitialiserList<Elem> l ) :
    data( nullptr ), size( 0 )
  {
    for( const Elem& e : l ) {
      add( e );
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
    data( new Bucket*[ht.size] ), size( ht.size )
  {
    for( int i = 0; i < ht.size; ++i ) {
      data[i] = cloneChain( ht.data[i] );
    }
  }

  /**
   * Move constructor, moves storage.
   */
  HashSet( HashSet&& ht ) :
    pool( static_cast< Pool<Bucket>&& >( ht.pool ) ), data( ht.data ), size( ht.size )
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

      data = new Bucket[ht.size];
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

    pool = static_cast< Pool<Bucket>&& >( ht.pool );
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
   * STL-style constant begin iterator.
   */
  OZ_ALWAYS_INLINE
  CIterator begin() const
  {
    return CIterator( *this );
  }

  /**
   * STL-style begin iterator.
   */
  OZ_ALWAYS_INLINE
  Iterator begin()
  {
    return Iterator( *this );
  }

  /**
   * STL-style constant end iterator.
   */
  OZ_ALWAYS_INLINE
  CIterator end() const
  {
    return CIterator();
  }

  /**
   * STL-style end iterator.
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
   * True iff a given element is found in the hashtable.
   */
  template <typename Elem_ = Elem>
  bool contains( const Elem_& elem ) const
  {
    if( size == 0 ) {
      return false;
    }

    int     h = hash( elem );
    uint    i = uint( h ) % uint( size );
    Bucket* b = data[i];

    while( b != nullptr ) {
      if( b->elem == elem ) {
        return true;
      }

      b = b->next;
    }
    return false;
  }

  /**
   * Add a new element, if the element already exists in the hashtable overwrite the existing one.
   */
  template <typename Elem_ = Elem>
  void add( Elem_&& elem )
  {
    ensureCapacity( pool.length() + 1 );

    int     h = hash( elem );
    uint    i = uint( h ) % uint( size );
    Bucket* b = data[i];

    while( b != nullptr ) {
      if( b->elem == elem ) {
        b->elem = static_cast<Elem_&&>( elem );
        return;
      }
      b = b->next;
    }

    data[i] = new( pool ) Bucket { data[i], h, static_cast<Elem_&&>( elem ) };
  }

  /**
   * Add a new element if it does not exist in the hashtable.
   */
  template <typename Elem_ = Elem>
  void include( Elem_&& elem )
  {
    ensureCapacity( pool.length() + 1 );

    int     h = hash( elem );
    uint    i = uint( h ) % uint( size );
    Bucket* b = data[i];

    while( b != nullptr ) {
      if( b->elem == elem ) {
        return;
      }
      b = b->next;
    }

    data[i] = new( pool ) Bucket { data[i], h, static_cast<Elem_&&>( elem ) };
  }

  /**
   * Remove an element.
   *
   * @return True iff the element was found (and removed).
   */
  bool exclude( const Elem& elem )
  {
    if( size == 0 ) {
      return nullptr;
    }

    uint     i     = uint( hash( elem ) ) % uint( size );
    Bucket*  b     = data[i];
    Bucket** bPrev = &data[i];

    while( b != nullptr ) {
      if( b->elem == elem ) {
        *bPrev = b->next;

        b->~Bucket();
        pool.deallocate( b );

        return true;
      }

      bPrev = &b->next;
      b = b->next;
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
    delete[] data;

    data = nullptr;
    size = 0;
  }

};

}
