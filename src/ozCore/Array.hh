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
 * @file ozCore/Array.hh
 *
 * `Array` class template
 */

#pragma once

#include "arrays.hh"

namespace oz
{

/**
 * Static array.
 *
 * The advantage over C++ arrays is it that is has bounds checking and iterator.
 *
 * @sa `oz::DArray`, `oz::SList`
 */
template <typename Elem, int COUNT>
class Array
{
  static_assert( COUNT > 0, "Array size must be at least 1" );

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

    Elem data[COUNT]; ///< %Array of elements.

  public:

    /**
     * Create uninitialised array.
     */
    explicit Array() = default;

    /**
     * Initialise form a C++ array.
     */
    explicit Array( const Elem* array )
    {
      aCopy<Elem>( array, COUNT, data );
    }

    /**
     * True iff respective elements are equal.
     */
    bool operator == ( const Array& a ) const
    {
      return aEquals<Elem>( data, COUNT, a.data );
    }

    /**
     * False iff respective elements are equal.
     */
    bool operator != ( const Array& a ) const
    {
      return !aEquals<Elem>( data, COUNT, a.data );
    }

    /**
     * %Iterator with constant access, initially points to the first element.
     */
    OZ_ALWAYS_INLINE
    CIterator citer() const
    {
      return CIterator( data, data + COUNT );
    }

    /**
     * %Iterator with non-constant access, initially points to the first element.
     */
    OZ_ALWAYS_INLINE
    Iterator iter()
    {
      return Iterator( data, data + COUNT );
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
      return data + COUNT;
    }

    /**
     * STL-compatible end iterator.
     */
    OZ_ALWAYS_INLINE
    Elem* end()
    {
      return data + COUNT;
    }

    /**
     * Number of elements.
     */
    OZ_ALWAYS_INLINE
    int length() const
    {
      return COUNT;
    }

    /**
     * Always false since static array cannot have zero elements.
     */
    OZ_ALWAYS_INLINE
    bool isEmpty() const
    {
      return false;
    }

    /**
     * Constant reference to the `i`-th element.
     */
    OZ_ALWAYS_INLINE
    const Elem& operator [] ( int i ) const
    {
      hard_assert( uint( i ) < uint( COUNT ) );

      return data[i];
    }

    /**
     * Reference the `i`-th element.
     */
    OZ_ALWAYS_INLINE
    Elem& operator [] ( int i )
    {
      hard_assert( uint( i ) < uint( COUNT ) );

      return data[i];
    }

    /**
     * Constant reference to the first element.
     */
    OZ_ALWAYS_INLINE
    const Elem& first() const
    {
      return data[0];
    }

    /**
     * Reference to the first element.
     */
    OZ_ALWAYS_INLINE
    Elem& first()
    {
      return data[0];
    }

    /**
     * Constant reference to the last element.
     */
    OZ_ALWAYS_INLINE
    const Elem& last() const
    {
      return data[COUNT - 1];
    }

    /**
     * Reference to the last element.
     */
    OZ_ALWAYS_INLINE
    Elem& last()
    {
      return data[COUNT - 1];
    }

    /**
     * True iff a given value is found in the array.
     */
    bool contains( const Elem& e ) const
    {
      return aContains<Elem, Elem>( data, COUNT, e );
    }

    /**
     * Index of the first occurrence of the value or -1 if not found.
     */
    int index( const Elem& e ) const
    {
      return aIndex<Elem, Elem>( data, COUNT, e );
    }

    /**
     * Index of the last occurrence of the value or -1 if not found.
     */
    int lastIndex( const Elem& e ) const
    {
      return aLastIndex<Elem, Elem>( data, COUNT, e );
    }

    /**
     * Reverse elements.
     */
    void reverse()
    {
      aReverse<Elem>( data, COUNT );
    }

    /**
     * Sort elements with quicksort.
     */
    void sort()
    {
      aSort<Elem>( data, COUNT );
    }

    /**
     * Delete objects referenced by elements and set all elements to `nullptr`.
     */
    void free()
    {
      aFree<Elem>( data, COUNT );
    }

};

}
