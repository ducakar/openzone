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
 * @file oz/DArray.hh
 *
 * DArray template class.
 */

#pragma once

#include "arrays.hh"

namespace oz
{

/**
 * Dynamic array.
 *
 * The advantage over C++ arrays is it has bounds checking, iterator and it frees allocated memory
 * when destructed.
 */
template <typename Elem>
class DArray
{
  public:

    /**
     * %Iterator with constant access to container elements.
     */
    typedef oz::Iterator<const Elem> CIterator;

    /**
     * %Iterator with non-constant access to container elements.
     */
    typedef oz::Iterator<Elem> Iterator;

  private:

    Elem* data;  ///< %Array of elements.
    int   count; ///< Number of elements.

  public:

    /**
     * Create an empty array.
     */
    DArray() :
      data( null ), count( 0 )
    {}

    /**
     * Destructor.
     */
    ~DArray()
    {
      delete[] data;
    }

    /**
     * Copy constructor, copies elements.
     */
    DArray( const DArray& a ) :
      data( a.count == 0 ? null : new Elem[a.count] ), count( a.count )
    {
      aCopy<Elem>( data, a.data, a.count );
    }

    /**
     * Move constructor, moves element storage.
     */
    DArray( DArray&& a ) :
      data( a.data ), count( a.count )
    {
      a.data  = null;
      a.count = 0;
    }

    /**
     * Copy operator, copies elements.
     *
     * Reuse existing storage only if it the size matches.
     */
    DArray& operator = ( const DArray& a )
    {
      if( &a == this ) {
        return *this;
      }

      if( count != a.count ) {
        delete[] data;

        data  = a.count == 0 ? null : new Elem[a.count];
        count = a.count;
      }

      aCopy<Elem>( data, a.data, a.count );

      return *this;
    }

    /**
     * Move operator, moves element storage.
     */
    DArray& operator = ( DArray&& a )
    {
      if( &a == this ) {
        return *this;
      }

      delete[] data;

      data  = a.data;
      count = a.count;

      a.data  = null;
      a.count = 0;

      return *this;
    }

    /**
     * Create an array with the given size.
     */
    explicit DArray( int count_ ) :
      data( count_ == 0 ? null : new Elem[count_] ), count( count_ )
    {}

    /**
     * Initialise from a C++ array.
     */
    explicit DArray( const Elem* array, int count_ ) :
      data( count_ == 0 ? null : new Elem[count_] ), count( count_ )
    {
      aCopy<Elem>( data, array, count );
    }

    /**
     * True iff respective elements are equal.
     */
    bool operator == ( const DArray& a ) const
    {
      return count == a.count && aEquals<Elem>( data, a.data, count );
    }

    /**
     * False iff respective elements are equal.
     */
    bool operator != ( const DArray& a ) const
    {
      return count != a.count || !aEquals<Elem>( data, a.data, count );
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
    Iterator iter() const
    {
      return Iterator( data, data + count );
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
     * True iff empty (no storage is allocated).
     */
    OZ_ALWAYS_INLINE
    bool isEmpty() const
    {
      return count == 0;
    }

    /**
     * Constant pointer to the first element.
     */
    OZ_ALWAYS_INLINE
    operator const Elem* () const
    {
      return data;
    }

    /**
     * Pointer to the first element.
     */
    OZ_ALWAYS_INLINE
    operator Elem* ()
    {
      return data;
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
      hard_assert( count > 0 );

      return data[0];
    }

    /**
     * Reference to the first element.
     */
    OZ_ALWAYS_INLINE
    Elem& first()
    {
      hard_assert( count > 0 );

      return data[0];
    }

    /**
     * Constant reference to the last element.
     */
    OZ_ALWAYS_INLINE
    const Elem& last() const
    {
      hard_assert( count > 0 );

      return data[count - 1];
    }

    /**
     * Reference to the last element.
     */
    OZ_ALWAYS_INLINE
    Elem& last()
    {
      hard_assert( count > 0 );

      return data[count - 1];
    }

    /**
     * True iff the given value is found in the array.
     */
    bool contains( const Elem& e ) const
    {
      hard_assert( count > 0 );

      return aContains<Elem, Elem>( data, e, count );
    }

    /**
     * Index of the first occurrence of the value or -1 if not found.
     */
    int index( const Elem& e ) const
    {
      hard_assert( count > 0 );

      return aIndex<Elem, Elem>( data, e, count );
    }

    /**
     * Index of the last occurrence of the value or -1 if not found.
     */
    int lastIndex( const Elem& e ) const
    {
      hard_assert( count > 0 );

      return aLastIndex<Elem, Elem>( data, e, count );
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
     * Resize the array.
     */
    void resize( int newCount )
    {
      if( newCount != count ) {
        data  = aRealloc( data, count, newCount );
        count = newCount;
      }
    }

    /**
     * Clear the array.
     */
    void clear()
    {
      delete[] data;

      data  = null;
      count = 0;
    }

    /**
     * Delete objects referenced by elements and clear the array.
     */
    void free()
    {
      aFree<Elem>( data, count );
      clear();
    }

};

}
