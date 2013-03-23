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
 * @file ozCore/List.hh
 *
 * List template class.
 */

#pragma once

#include "arrays.hh"

namespace oz
{

/**
 * %Array list.
 *
 * In contrast with `std::vector` all allocated elements are constructed all the time. This yields
 * slightly better performance and simplifies implementation. However, on element removal its
 * destruction is guaranteed.
 *
 * Memory is allocated when the first element is added.
 *
 * @sa `oz::SList`, `oz::DArray`
 */
template <typename Elem>
class List
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
     * Increase capacity to be able to hold the requested number of elements.
     *
     * Capacity is doubled, if it doesn't suffice, it is set to the least multiple of `GRANULARITY`
     * able to hold the requested number of elements.
     */
    void ensureCapacity( int capacity )
    {
      if( size < capacity ) {
        size *= 2;
        size  = size < capacity ? ( capacity + GRANULARITY - 1 ) / GRANULARITY * GRANULARITY : size;
        data  = aReallocate<Elem>( data, count, size );
      }
    }

  public:

    /**
     * Create an empty list with the given initial capacity.
     */
    explicit List( int capacity = 0 ) :
      data( capacity == 0 ? nullptr : new Elem[capacity] ), count( 0 ), size( capacity )
    {}

    /**
     * Destructor.
     */
    ~List()
    {
      delete[] data;
    }

    /**
     * Copy constructor, copies elements.
     */
    List( const List& l ) :
      data( l.size == 0 ? nullptr : new Elem[l.size] ), count( l.count ), size( l.size )
    {
      aCopy<Elem>( data, l.data, l.count );
    }

    /**
     * Move constructor, moves element storage.
     */
    List( List&& l ) :
      data( l.data ), count( l.count ), size( l.size )
    {
      l.data  = nullptr;
      l.count = 0;
      l.size  = 0;
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

      aCopy<Elem>( data, l.data, l.count );
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
      return count == l.count && aEquals<Elem>( data, l.data, count );
    }

    /**
     * False iff respective elements are equal.
     */
    bool operator != ( const List& l ) const
    {
      return count != l.count || !aEquals<Elem>( data, l.data, count );
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
     * True iff the given value is found in the list.
     */
    bool contains( const Elem& e ) const
    {
      return aContains<Elem, Elem>( data, e, count );
    }

    /**
     * Index of the first occurrence of the value or -1 if not found.
     */
    int index( const Elem& e ) const
    {
      return aIndex<Elem, Elem>( data, e, count );
    }

    /**
     * Index of the last occurrence of the value or -1 if not found.
     */
    int lastIndex( const Elem& e ) const
    {
      return aLastIndex<Elem, Elem>( data, e, count );
    }

    /**
     * Add an element to the end.
     */
    template <typename Elem_ = Elem>
    void add( Elem_&& e = Elem() )
    {
      pushLast<Elem_>( static_cast<Elem_&&>( e ) );
    }

    /**
     * Add (copy) elements from the given array to the end.
     */
    void addAll( const Elem* array, int arrayCount )
    {
      int newCount = count + arrayCount;

      ensureCapacity( newCount );

      aCopy<Elem>( data + count, array, arrayCount );
      count = newCount;
    }

    /**
     * Add (move) elements from the given array to the end.
     */
    void takeAll( Elem* array, int arrayCount )
    {
      int newCount = count + arrayCount;

      ensureCapacity( newCount );

      aMove<Elem>( data + count, array, arrayCount );
      count = newCount;
    }

    /**
     * Add an element to the end if there is no equal element in the list.
     *
     * @return Position of the inserted or the existing equal element.
     */
    template <typename Elem_ = Elem>
    int include( Elem_&& e )
    {
      int i = aIndex<Elem, Elem>( data, e, count );

      if( i < 0 ) {
        ensureCapacity( count + 1 );

        data[count] = static_cast<Elem_&&>( e );
        i = count;
        ++count;
      }
      return i;
    }

    /**
     * Insert an element at the given position.
     *
     * All later elements are shifted to make the gap.
     */
    template <typename Elem_ = Elem>
    void insert( int i, Elem_&& e )
    {
      hard_assert( uint( i ) <= uint( count ) );

      ensureCapacity( count + 1 );

      aMoveBackward<Elem>( data + i + 1, data + i, count - i );
      data[i] = static_cast<Elem_&&>( e );
      ++count;
    }

    /**
     * Remove the last element.
     */
    void erase()
    {
      popLast();
    }

    /**
     * Remove the element at the given position.
     *
     * All later elements are shifted to fill the gap.
     */
    void erase( int i )
    {
      hard_assert( uint( i ) < uint( count ) );

      --count;

      if( i == count ) {
        // When removing the last element, no shift is performed, so its resources are not
        // implicitly destroyed by move operation.
        data[count] = Elem();
      }
      else {
        aMove<Elem>( data + i, data + i + 1, count - i );
      }
    }

    /**
     * Remove the element at the given position from an unordered list.
     *
     * The last element is moved to its place.
     */
    void eraseUnordered( int i )
    {
      hard_assert( uint( i ) < uint( count ) );

      --count;

      if( i == count ) {
        // When removing the last element, move is probably a no-op, so its resources are not
        // implicitly destroyed by move operation.
        data[count] = Elem();
      }
      else {
        data[i] = static_cast<Elem&&>( data[count] );
      }
    }

    /**
     * Find and remove the first element with the given value.
     *
     * @return Index of the removed element or -1 if not found.
     */
    int exclude( const Elem& e )
    {
      int i = aIndex<Elem, Elem>( data, e, count );

      if( i >= 0 ) {
        erase( i );
      }
      return i;
    }

    /**
     * Find and remove the first element with the given value from an unordered list.
     *
     * The last element is moved to its place.
     *
     * @return Index of the removed element or -1 if not found.
     */
    int excludeUnordered( const Elem& e )
    {
      int i = aIndex<Elem, Elem>( data, e, count );

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
    void pushFirst( Elem_&& e )
    {
      ensureCapacity( count + 1 );

      aMoveBackward<Elem>( data + 1, data, count );
      data[0] = static_cast<Elem_&&>( e );
      ++count;
    }

    /**
     * Add an element to the end.
     */
    template <typename Elem_ = Elem>
    void pushLast( Elem_&& e )
    {
      ensureCapacity( count + 1 );

      data[count] = static_cast<Elem_&&>( e );
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
      Elem e = static_cast<Elem&&>( data[0] );

      --count;
      aMove<Elem>( data, data + 1, count );
      return e;
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
     * Reverse elements.
     */
    void reverse()
    {
      aReverse<Elem>( data, count );
    }

    /**
     * Sort elements with quicksort.
     */
    void sort()
    {
      aSort<Elem>( data, count );
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
      for( int i = 0; i < count; ++i ) {
        data[i] = Elem();
      }
      count = 0;
    }

    /**
     * Delete all objects referenced by elements and clear the list.
     */
    void free()
    {
      aFree<Elem>( data, count );
      clear();
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
