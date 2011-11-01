/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file oz/DArray.hpp
 */

#pragma once

#include "arrays.hpp"

namespace oz
{

/**
 * Dynamic array.
 *
 * The advantage over C++ arrays is it has bounds checking, iterator and it frees allocated memory
 * when destroyed.
 *
 * @ingroup oz
 */
template <typename Elem>
class DArray
{
  public:

    /**
     * %Iterator with constant access to container elements.
     */
    class CIterator : public oz::CIterator<Elem>
    {
      friend class DArray;

      OZ_RANGE_ITERATOR( CIterator )

      private:

        /// Base class type, convenience definition to make code cleaner.
        typedef oz::CIterator<Elem> B;

        /**
         * %Iterator for the given container, points to its first element.
         */
        OZ_ALWAYS_INLINE
        explicit CIterator( const DArray& a ) : B( a.data, a.data + a.size )
        {}

      public:

        /**
         * Default constructor, creates an invalid iterator.
         */
        OZ_ALWAYS_INLINE
        CIterator() : B( null, null )
        {}

    };

    /**
     * %Iterator with non-constant access to container elements.
     */
    class Iterator : public oz::Iterator<Elem>
    {
      friend class DArray;

      OZ_RANGE_ITERATOR( Iterator )

      private:

        /// Base class type, convenience definition to make code cleaner.
        typedef oz::Iterator<Elem> B;

        /**
         * %Iterator for the given container, points to its first element.
         */
        OZ_ALWAYS_INLINE
        explicit Iterator( const DArray& a ) : B( a.data, a.data + a.size )
        {}

      public:

        /**
         * Default constructor, creates an invalid iterator.
         */
        OZ_ALWAYS_INLINE
        Iterator() : B( null, null )
        {}

    };

  private:

    Elem* data; ///< %Array of elements.
    int   size; ///< Number of elements.

  public:

    /**
     * Create an empty array.
     */
    DArray() : data( null ), size( 0 )
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
    DArray( const DArray& a ) : data( a.size == 0 ? null : new Elem[a.size] ), size( a.size )
    {
      aCopy( data, a.data, a.size );
    }

    /**
     * Move constructor, moves element storage.
     */
    DArray( DArray&& a ) : data( a.data ), size( a.size )
    {
      a.data = null;
      a.size = 0;
    }

    /**
     * Copy operator, copies elements.
     *
     * Reuse existing storage only if it the size matches.
     */
    DArray& operator = ( const DArray& a )
    {
      if( &a == this ) {
        soft_assert( &a != this );
        return *this;
      }

      if( size != a.size ) {
        delete[] data;

        data = a.size == 0 ? null : new Elem[a.size];
        size = a.size;
      }

      aCopy( data, a.data, a.count );

      return *this;
    }

    /**
     * Move operator, moves element storage.
     */
    DArray& operator = ( DArray&& a )
    {
      if( &a == this ) {
        soft_assert( &a != this );
        return *this;
      }

      delete[] data;

      data = a.data;
      size = a.size;

      a.data = null;
      a.size = 0;

      return *this;
    }

    /**
     * Create an array with the given size.
     */
    explicit DArray( int size_ ) : data( size_ == 0 ? null : new Elem[size_] ), size( size_ )
    {}

    /**
     * Initialise from a C++ array.
     */
    explicit DArray( const Elem* array, int size_ ) : data( size_ == 0 ? null : new Elem[size_] ),
        size( size_ )
    {
      aCopy( data, array, size );
    }

    /**
     * True iff the same size and respective elements are equal.
     */
    bool operator == ( const DArray& a ) const
    {
      return size == a.size && aEquals( data, a.data, size );
    }

    /**
     * False iff the same size and respective elements are equal.
     */
    bool operator != ( const DArray& a ) const
    {
      return size != a.size || !aEquals( data, a.data, size );
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
     * Constant pointer to the first element.
     */
    OZ_ALWAYS_INLINE
    operator const Elem* () const
    {
      hard_assert( size > 0 );

      return data;
    }

    /**
     * Pointer to the first element.
     */
    OZ_ALWAYS_INLINE
    operator Elem* ()
    {
      hard_assert( size > 0 );

      return data;
    }

    /**
     * Number of elements.
     */
    OZ_ALWAYS_INLINE
    int length() const
    {
      return size;
    }

    /**
     * True iff empty (no storage is allocated).
     */
    OZ_ALWAYS_INLINE
    bool isEmpty() const
    {
      return size == 0;
    }

    /**
     * Constant reference to the i-th element.
     */
    OZ_ALWAYS_INLINE
    const Elem& operator [] ( int i ) const
    {
      hard_assert( uint( i ) < uint( size ) );

      return data[i];
    }

    /**
     * Reference to the i-th element.
     */
    OZ_ALWAYS_INLINE
    Elem& operator [] ( int i )
    {
      hard_assert( uint( i ) < uint( size ) );

      return data[i];
    }

    /**
     * Constant reference to the first element.
     */
    OZ_ALWAYS_INLINE
    const Elem& first() const
    {
      hard_assert( size > 0 );

      return data[0];
    }

    /**
     * Reference to the first element.
     */
    OZ_ALWAYS_INLINE
    Elem& first()
    {
      hard_assert( size > 0 );

      return data[0];
    }

    /**
     * Constant reference to the last element.
     */
    OZ_ALWAYS_INLINE
    const Elem& last() const
    {
      hard_assert( size > 0 );

      return data[size - 1];
    }

    /**
     * Reference to the last element.
     */
    OZ_ALWAYS_INLINE
    Elem& last()
    {
      hard_assert( size > 0 );

      return data[size - 1];
    }

    /**
     * True iff the given value is found in the array.
     */
    bool contains( const Elem& e ) const
    {
      hard_assert( size > 0 );

      return aContains( data, e, size );
    }

    /**
     * Index of the first occurrence of the value or -1 if not found.
     */
    int index( const Elem& e ) const
    {
      hard_assert( size > 0 );

      return aIndex( data, e, size );
    }

    /**
     * Index of the last occurrence of the value or -1 if not found.
     */
    int lastIndex( const Elem& e ) const
    {
      hard_assert( size > 0 );

      return aLastIndex( data, e, size );
    }

    /**
     * Sort elements with quicksort.
     */
    void sort()
    {
      hard_assert( size > 0 );

      aSort( data, size );
    }

    /**
     * Delete objects referenced by elements and set all elements to null.
     */
    void free()
    {
      aFree( data, size );
    }

    /**
     * For an empty array, allocate <tt>initSize</tt> elements.
     */
    void alloc( int initSize )
    {
      hard_assert( size == 0 && initSize > 0 );

      data = new Elem[initSize];
      size = initSize;
    }

    /**
     * Deallocate data.
     */
    void dealloc()
    {
      delete[] data;

      data = null;
      size = 0;
    }

};

}
