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
 * @file ozCore/Set.hh
 *
 * `Set` class template.
 */

#pragma once

#include "arrays.hh"
#include "System.hh"

namespace oz
{

/**
 * Sorted array list.
 *
 * %Set is implemented as a sorted array list.
 * Better worst case performance than a hashtable; however, for large sets `HashSet` is preferred as
 * it is asymptotically faster in average case.
 *
 * Like in `List` all allocated elements are constructed all the time and a removed element's
 * destruction is still guaranteed.
 *
 * Memory is allocated when the first element is added.
 *
 * @sa `oz::Map`, `oz::HashSet`
 */
template <typename Elem>
class Set
{
private:

  /// Granularity for automatic storage allocations.
  static const int GRANULARITY = 8;

public:

  /**
   * %Iterator with constant access to elements.
   */
  typedef ArrayIterator<const Elem> CIterator;

  /**
   * %Iterator with non-constant access to elements.
   */
  typedef ArrayIterator<Elem> Iterator;

private:

  Elem* data;  ///< Element storage.
  int   count; ///< Number of elements.
  int   size;  ///< Capacity, number of elements in storage.

  /**
   * Ensure a given capacity.
   *
   * Capacity is doubled if neccessary. If that doesn't suffice it is set to the least multiple of
   * `GRANULARITY` able to hold the requested number of elements.
   */
  void ensureCapacity( int capacity )
  {
    if( capacity < 0 ) {
      OZ_ERROR( "oz::Set: Capacity overflow" );
    }
    else if( size < capacity ) {
      size *= 2;
      size  = size < capacity ? ( capacity + GRANULARITY - 1 ) & ~( GRANULARITY - 1 ) : size;

      if( size <= 0 ) {
        OZ_ERROR( "oz::Set: Capacity overflow" );
      }

      data = aReallocate<Elem>( data, count, size );
    }
  }

public:

  /**
   * Create an empty set with a given initial capacity.
   */
  explicit Set( int capacity = 0 ) :
    data( capacity == 0 ? nullptr : new Elem[capacity] ), count( 0 ), size( capacity )
  {}

  /**
   * Initialise from an initialiser list.
   */
  Set( InitialiserList<Elem> l ) :
    data( new Elem[ l.size() ] ), count( int( l.size() ) ), size( int( l.size() ) )
  {
    aCopy<Elem>( l.begin(), int( l.size() ), data );
    aSort<Elem>( data, count );
  }

  /**
   * Destructor.
   */
  ~Set()
  {
    delete[] data;
  }

  /**
   * Copy constructor, copies elements.
   */
  Set( const Set& s ) :
    data( s.size == 0 ? nullptr : new Elem[s.size] ), count( s.count ), size( s.size )
  {
    aCopy<Elem>( s.data, s.count, data );
  }

  /**
   * Move constructor, moves element storage.
   */
  Set( Set&& s ) :
    data( s.data ), count( s.count ), size( s.size )
  {
    s.data  = nullptr;
    s.count = 0;
    s.size  = 0;
  }

  /**
   * Copy operator, copies elements.
   *
   * Existing storage is reused if it suffices.
   */
  Set& operator = ( const Set& s )
  {
    if( &s == this ) {
      return *this;
    }

    if( size < s.count ) {
      delete[] data;

      data = new Elem[s.size];
      size = s.size;
    }

    aCopy<Elem>( s.data, s.count, data );
    count = s.count;

    return *this;
  }

  /**
   * Move operator, moves element storage.
   */
  Set& operator = ( Set&& s )
  {
    if( &s == this ) {
      return *this;
    }

    delete[] data;

    data    = s.data;
    count   = s.count;
    size    = s.size;

    s.data  = nullptr;
    s.count = 0;
    s.size  = 0;

    return *this;
  }

  /**
   * True iff respective elements are equal.
   */
  bool operator == ( const Set& s ) const
  {
    return count == s.count && aEquals<Elem>( data, count, s.data );
  }

  /**
   * False iff respective elements are equal.
   */
  bool operator != ( const Set& s ) const
  {
    return !operator == ( s );
  }

  /**
   * %Iterator with constant access, initially points to the first element.
   */
  OZ_ALWAYS_INLINE
  CIterator citer() const
  {
    return CIterator( data, data + count );
  }

  /**
   * %Iterator with non-constant access, initially points to the first element.
   */
  OZ_ALWAYS_INLINE
  Iterator iter()
  {
    return Iterator( data, data + count );
  }

  /**
   * STL-compatible constant begin iterator.
   */
  OZ_ALWAYS_INLINE
  const Elem* begin() const
  {
    return data;
  }

  /**
   * STL-compatible begin iterator.
   */
  OZ_ALWAYS_INLINE
  Elem* begin()
  {
    return data;
  }

  /**
   * STL-compatible constant end iterator.
   */
  OZ_ALWAYS_INLINE
  const Elem* end() const
  {
    return data + count;
  }

  /**
   * STL-compatible end iterator.
   */
  OZ_ALWAYS_INLINE
  Elem* end()
  {
    return data + count;
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
    return size;
  }

  /**
   * Constant reference to the `i`-th element.
   */
  OZ_ALWAYS_INLINE
  const Elem& operator [] ( int i ) const
  {
    hard_assert( uint( i ) < uint( count ) );

    return data[i];
  }

  /**
   * Reference to the `i`-th element.
   */
  OZ_ALWAYS_INLINE
  Elem& operator [] ( int i )
  {
    hard_assert( uint( i ) < uint( count ) );

    return data[i];
  }

  /**
   * Constant reference to the first element.
   */
  OZ_ALWAYS_INLINE
  const Elem& first() const
  {
    hard_assert( count != 0 );

    return data[0];
  }

  /**
   * Reference to the first element.
   */
  OZ_ALWAYS_INLINE
  Elem& first()
  {
    hard_assert( count != 0 );

    return data[0];
  }

  /**
   * Constant reference to the last element.
   */
  OZ_ALWAYS_INLINE
  const Elem& last() const
  {
    hard_assert( count != 0 );

    return data[count - 1];
  }

  /**
   * Reference to the last element.
   */
  OZ_ALWAYS_INLINE
  Elem& last()
  {
    hard_assert( count != 0 );

    return data[count - 1];
  }

  /**
   * True iff a given key is found in the set.
   */
  template <typename Key = Elem>
  bool contains( const Key& key ) const
  {
    int i = aBisection<Elem, Key>( data, count, key );
    return i >= 0 && data[i] == key;
  }

  /**
   * Index of the element with a given value or -1 if not found.
   */
  template <typename Key = Elem>
  int index( const Key& key ) const
  {
    int i = aBisection<Elem, Key>( data, count, key );
    return i >= 0 && data[i] == key ? i : -1;
  }

  /**
   * Add an element overriding any existing equal element.
   *
   * @return Position of the inserted element.
   */
  template <typename Elem_ = Elem>
  int add( Elem_&& elem )
  {
    int i = aBisection<Elem, Elem>( data, count, elem );

    if( i >= 0 && data[i] == elem ) {
      data[i] = static_cast<Elem_&&>( elem );
      return i;
    }
    else {
      insert<Elem_>( i + 1, static_cast<Elem_&&>( elem ) );
      return i + 1;
    }
  }

  /**
   * Add an element if if there is no equal element in the set.
   *
   * @return Position of the inserted or the existing element.
   */
  template <typename Elem_ = Elem>
  int include( Elem_&& elem )
  {
    int i = aBisection<Elem, Elem>( data, count, elem );

    if( i >= 0 && data[i] == elem ) {
      return i;
    }
    else {
      insert<Elem_>( i + 1, static_cast<Elem_&&>( elem ) );
      return i + 1;
    }
  }

  /**
   * Insert an element at a given position.
   *
   * All later elements are shifted to make a gap.
   * Use only when you are sure you are inserting at the right position to preserve order of the
   * element.
   */
  template <typename Elem_ = Elem>
  void insert( int i, Elem_&& elem )
  {
    hard_assert( uint( i ) <= uint( count ) );

    ensureCapacity( count + 1 );

    aMoveBackward<Elem>( data + i, count - i, data + i + 1 );
    data[i] = static_cast<Elem_&&>( elem );

    ++count;
  }

  /**
   * Remove the element at a given position.
   *
   * All later elements are shifted to fill the gap.
   */
  void erase( int i )
  {
    hard_assert( uint( i ) < uint( count ) );

    --count;

    if( i == count ) {
      // When removing the last element, no shift is performed, so it is not implicitly destroyed by
      // the move operation.
      data[count] = Elem();
    }
    else {
      aMove<Elem>( data + i + 1, count - i, data + i );
    }
  }

  /**
   * Find and remove the element with a given value.
   *
   * @return Index of the removed element or -1 if not found.
   */
  int exclude( const Elem& elem )
  {
    int i = aBisection<Elem, Elem>( data, count, elem );

    if( i >= 0 && data[i] == elem ) {
      erase( i );
      return i;
    }
    return -1;
  }

  /**
   * Resize the set to the specified number of elements.
   */
  void resize( int newCount )
  {
    if( newCount > count ) {
      ensureCapacity( newCount );
    }
    else {
      // Ensure destruction of removed elements.
      for( int i = newCount; i < count; ++i ) {
        data[i] = Elem();
      }
    }
    count = newCount;
  }

  /**
   * Trim capacity to the current number of elements.
   */
  void trim()
  {
    if( count < size ) {
      size = count;
      data = aReallocate<Elem>( data, count, size );
    }
  }

  /**
   * Clear the set.
   */
  void clear()
  {
    // Ensure destruction of all elements.
    aFill<Elem, Elem>( data, count, Elem() );
    count = 0;
  }

  /**
   * Delete all objects referenced by elements (must be pointers) and clear the set.
   */
  void free()
  {
    aFree<Elem>( data, count );
    count = 0;
  }

  /**
   * For an empty set with no allocated storage, allocate capacity for `capacity` elements.
   */
  void allocate( int capacity )
  {
    hard_assert( size == 0 && capacity > 0 );

    data = new Elem[capacity];
    size = capacity;
  }

  /**
   * Deallocate storage of an empty set.
   */
  void deallocate()
  {
    hard_assert( count == 0 );

    delete[] data;

    data = nullptr;
    size = 0;
  }

};

}
