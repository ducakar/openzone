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
 * @file ozCore/List.hh
 *
 * `List` class template.
 */

#pragma once

#include "System.hh"
#include "DArray.hh"

namespace oz
{

/**
 * %Array list.
 *
 * In contrast with `std::vector` all allocated elements are constructed all the time. This yields
 * slightly better performance and simplifies implementation. However, on element removal its
 * destruction is still guaranteed.
 *
 * Memory is allocated when the first element is added.
 *
 * @sa `oz::SList`, `oz::DArray`
 */
template <typename Elem>
class List : protected DArray<Elem>
{
protected:

  /// Granularity for automatic storage allocations.
  static const int GRANULARITY = 8;

public:

  /**
   * %Iterator with constant access to elements.
   */
  typedef typename DArray<Elem>::CIterator CIterator;

  /**
   * %Iterator with non-constant access to elements.
   */
  typedef typename DArray<Elem>::Iterator Iterator;

protected:

  using DArray<Elem>::data;
  using DArray<Elem>::count;

  int size; ///< Capacity, number of elements in storage.

protected:

  /**
   * Ensure a given capacity.
   *
   * Capacity is doubled if neccessary. If that doesn't suffice it is set to the least multiple of
   * `GRANULARITY` able to hold the requested number of elements.
   */
  void ensureCapacity( int capacity )
  {
    if( capacity < 0 ) {
      OZ_ERROR( "oz::List: Capacity overflow" );
    }
    else if( size < capacity ) {
      size *= 2;
      size  = size < capacity ? ( capacity + GRANULARITY - 1 ) & ~( GRANULARITY - 1 ) : size;

      if( size <= 0 ) {
        OZ_ERROR( "oz::List: Capacity overflow" );
      }

      data = aReallocate<Elem>( data, count, size );
    }
  }

public:

  /**
   * Create an empty list.
   */
  List() :
    size( 0 )
  {}

  /**
   * Initialise from an initialiser list.
   */
  List( InitialiserList<Elem> l ) :
    DArray<Elem>( l ), size( int( l.size() ) )
  {}

  /**
   * Copy constructor, copies elements.
   */
  List( const List& l ) :
    DArray<Elem>( l.size ), size( l.size )
  {
    count = l.count;

    aCopy<Elem>( l.data, l.count, data );
  }

  /**
   * Move constructor, moves element storage.
   */
  List( List&& l ) :
    DArray<Elem>( static_cast<List&&>( l ) ), size( l.size )
  {
    l.size = 0;
  }

  /**
   * Copy operator, copies elements.
   *
   * Existing storage is reused if it suffices.
   */
  List& operator = ( const List& l )
  {
    if( &l == this ) {
      return *this;
    }

    if( size < l.count ) {
      delete[] data;

      data = new Elem[l.size];
      size = l.size;
    }

    aCopy<Elem>( l.data, l.count, data );
    count = l.count;

    return *this;
  }

  /**
   * Move operator, moves element storage.
   */
  List& operator = ( List&& l )
  {
    if( &l == this ) {
      return *this;
    }

    delete[] data;

    data    = l.data;
    count   = l.count;
    size    = l.size;

    l.data  = nullptr;
    l.count = 0;
    l.size  = 0;

    return *this;
  }

  /**
   * True iff respective elements are equal.
   */
  bool operator == ( const List& l ) const
  {
    return DArray<Elem>::operator == ( l );
  }

  /**
   * False iff respective elements are equal.
   */
  bool operator != ( const List& l ) const
  {
    return DArray<Elem>::operator != ( l );
  }

  using DArray<Elem>::citer;
  using DArray<Elem>::iter;
  using DArray<Elem>::begin;
  using DArray<Elem>::end;
  using DArray<Elem>::length;
  using DArray<Elem>::isEmpty;
  using DArray<Elem>::operator [];
  using DArray<Elem>::first;
  using DArray<Elem>::last;
  using DArray<Elem>::contains;
  using DArray<Elem>::index;
  using DArray<Elem>::lastIndex;
  using DArray<Elem>::reverse;
  using DArray<Elem>::sort;

  /**
   * Number of allocated elements.
   */
  OZ_ALWAYS_INLINE
  int capacity() const
  {
    return size;
  }

  /**
   * Add an element to the end.
   */
  template <typename Elem_ = Elem>
  void add( Elem_&& elem )
  {
    pushLast<Elem_>( static_cast<Elem_&&>( elem ) );
  }

  /**
   * Add (copy) elements from a given array to the end.
   */
  void addAll( const Elem* array, int arrayCount )
  {
    int newCount = count + arrayCount;

    ensureCapacity( newCount );

    aCopy<Elem>( array, arrayCount, data + count );
    count = newCount;
  }

  /**
   * Add (move) elements from a given array to the end.
   */
  void takeAll( Elem* array, int arrayCount )
  {
    int newCount = count + arrayCount;

    ensureCapacity( newCount );

    aMove<Elem>( array, arrayCount, data + count );
    count = newCount;
  }

  /**
   * Add an element to the end if there is no equal element in the list.
   *
   * @return Position of the inserted or the existing equal element.
   */
  template <typename Elem_ = Elem>
  int include( Elem_&& elem )
  {
    int i = aIndex<Elem, Elem>( data, count, elem );

    if( i >= 0 ) {
      return i;
    }
    else {
      pushLast<Elem_>( static_cast<Elem_&&>( elem ) );
      return count - 1;
    }
  }

  /**
   * Insert an element at a given position.
   *
   * All later elements are shifted to make the gap.
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
   * Remove the element at a given position from an unordered list.
   *
   * The last element is moved to its place.
   */
  void eraseUnordered( int i )
  {
    hard_assert( uint( i ) < uint( count ) );

    --count;

    if( i == count ) {
      // When removing the last element, no shift is performed, so it is not implicitly destroyed by
      // the move operation.
      data[count] = Elem();
    }
    else {
      data[i] = static_cast<Elem&&>( data[count] );
    }
  }

  /**
   * Find and remove the first element with a given value.
   *
   * @return Index of the removed element or -1 if not found.
   */
  template <typename Elem_ = Elem>
  int exclude( const Elem_& elem )
  {
    int i = aIndex<Elem, Elem_>( data, count, elem );

    if( i >= 0 ) {
      erase( i );
    }
    return i;
  }

  /**
   * Find and remove the first element with a given value from an unordered list.
   *
   * The last element is moved to its place.
   *
   * @return Index of the removed element or -1 if not found.
   */
  template <typename Elem_ = Elem>
  int excludeUnordered( const Elem_& elem )
  {
    int i = aIndex<Elem, Elem_>( data, count, elem );

    if( i >= 0 ) {
      eraseUnordered( i );
    }
    return i;
  }

  /**
   * Add an element to the beginning.
   *
   * All elements are shifted to make a gap.
   */
  template <typename Elem_ = Elem>
  void pushFirst( Elem_&& elem )
  {
    ensureCapacity( count + 1 );

    aMoveBackward<Elem>( data, count, data + 1 );
    data[0] = static_cast<Elem_&&>( elem );
    ++count;
  }

  /**
   * Add an element to the end.
   */
  template <typename Elem_ = Elem>
  void pushLast( Elem_&& elem )
  {
    ensureCapacity( count + 1 );

    data[count] = static_cast<Elem_&&>( elem );
    ++count;
  }

  /**
   * Remove the first element.
   *
   * All elements are shifted to fill the gap.
   *
   * @return Value of the removed element.
   */
  Elem popFirst()
  {
    Elem elem = static_cast<Elem&&>( data[0] );

    --count;
    aMove<Elem>( data + 1, count, data );
    return elem;
  }

  /**
   * Remove the last element.
   *
   * @return Value of the removed element.
   */
  Elem popLast()
  {
    hard_assert( count != 0 );

    --count;
    return static_cast<Elem&&>( data[count] );
  }

  /**
   * Resize the list to the specified number of elements.
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
   * Clear the list.
   */
  void clear()
  {
    // Ensure destruction of all elements.
    aFill<Elem, Elem>( data, count, Elem() );
    count = 0;
  }

  /**
   * Delete all objects referenced by elements (must be pointers) and clear the list.
   */
  void free()
  {
    aFree<Elem>( data, count );
    count = 0;
  }

  /**
   * For an empty list with no allocated storage, allocate capacity for `capacity` elements.
   */
  void allocate( int capacity )
  {
    hard_assert( size == 0 && capacity > 0 );

    data = new Elem[capacity];
    size = capacity;
  }

  /**
   * Deallocate storage of an empty list.
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
