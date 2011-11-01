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
 * @file oz/SVector.hpp
 */

#pragma once

#include "arrays.hpp"

namespace oz
{

/**
 * Static vector (static array list).
 *
 * Fixed capacity vector with static storage.
 *
 * @ingroup oz
 */
template <typename Elem, int SIZE>
class SVector
{
  static_assert( SIZE > 0, "SVector size must be at least 1" );

  public:

    /**
     * %Iterator with constant access to container elements.
     */
    class CIterator : public oz::CIterator<Elem>
    {
      friend class SVector;

      OZ_RANGE_ITERATOR( CIterator )

      private:

        /// Base class type, convenience definition to make code cleaner.
        typedef oz::CIterator<Elem> B;

        /**
         * %Iterator for the given container, points to its first element.
         */
        OZ_ALWAYS_INLINE
        explicit CIterator( const SVector& v ) : B( v.data, v.data + v.count )
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
      friend class SVector;

      OZ_RANGE_ITERATOR( Iterator )

      private:

        /// Base class type, convenience definition to make code cleaner.
        typedef oz::Iterator<Elem> B;

        /**
         * %Iterator for the given container, points to its first element.
         */
        OZ_ALWAYS_INLINE
        explicit Iterator( SVector& v ) : B( v.data, v.data + v.count )
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

    Elem data[SIZE]; ///< Element storage.
    int  count;      ///< Number of elements.

  public:

    /**
     * Create an empty vector with capacity SIZE.
     */
    SVector() : count( 0 )
    {}

    /**
     * True iff all elements are equal.
     */
    bool operator == ( const SVector& v ) const
    {
      return count == v.count && aEquals( data, v.data, count );
    }

    /**
     * False if all elements are equal.
     */
    bool operator != ( const SVector& v ) const
    {
      return count != v.count || !aEquals( data, v.data, count );
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
     * Size of storage.
     */
    OZ_ALWAYS_INLINE
    int capacity() const
    {
      return SIZE;
    }

    /**
     * Constant reference to the i-th element.
     */
    OZ_ALWAYS_INLINE
    const Elem& operator [] ( int i ) const
    {
      hard_assert( uint( i ) < uint( count ) );

      return data[i];
    }

    /**
     * Reference to the i-th element.
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
     * True iff the given value is found in the vector.
     */
    bool contains( const Elem& e ) const
    {
      return aContains( data, e, count );
    }

    /**
     * Index of the first occurrence of the value or -1 if not found.
     */
    int index( const Elem& e ) const
    {
      return aIndex( data, e, count );
    }

    /**
     * Index of the last occurrence of the value or -1 if not found.
     */
    int lastIndex( const Elem& e ) const
    {
      return aLastIndex( data, e, count );
    }

    /**
     * Create slot for a new element at the end.
     */
    void add()
    {
      hard_assert( uint( count ) < uint( SIZE ) );

      ++count;
    }

    /**
     * Add an element to the end.
     */
    template <typename Elem_>
    void add( Elem_&& e )
    {
      hard_assert( uint( count ) < uint( SIZE ) );

      data[count] = static_cast<Elem_&&>( e );
      ++count;
    }

    /**
     * Add elements from the array to the end.
     */
    void addAll( const Elem* array, int arrayCount )
    {
      int newCount = count + arrayCount;

      hard_assert( uint( newCount ) <= uint( SIZE ) );

      aCopy( data + count, array, arrayCount );
      count = newCount;
    }

    /**
     * Add an element to the end if there is no equal element in the vector.
     *
     * @return position of the inserted or the existing equal element.
     */
    template <typename Elem_>
    int include( Elem_&& e )
    {
      int i = aIndex( data, e, count );

      if( i == -1 ) {
        hard_assert( uint( count ) < uint( SIZE ) );

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
    template <typename Elem_>
    void insert( int i, Elem_&& e )
    {
      hard_assert( uint( i ) <= uint( count ) );
      hard_assert( uint( count ) < uint( SIZE ) );

      aReverseMove( data + i + 1, data + i, count - i );
      data[i] = static_cast<Elem_&&>( e );
      ++count;
    }

    /**
     * Remove the last element.
     */
    void remove()
    {
      hard_assert( count != 0 );

      --count;
    }

    /**
     * Remove the element at the given position.
     *
     * All later elements are shifted to fill the gap.
     */
    void remove( int i )
    {
      hard_assert( uint( i ) < uint( count ) );

      --count;
      aMove( data + i, data + i + 1, count - i );
    }

    /**
     * Remove the element at the given position from an unordered vector.
     *
     * The last element is moved to its place.
     */
    void removeUO( int i )
    {
      hard_assert( uint( i ) < uint( count ) );

      --count;
      data[i] = static_cast<Elem&&>( data[count] );
    }

    /**
     * Find and remove the first element with the given value.
     *
     * @return index of the removed element or -1 if not found.
     */
    int exclude( const Elem& e )
    {
      int i = aIndex( data, e, count );

      if( i != -1 ) {
        --count;
        aMove( data + i, data + i + 1, count - i );
      }
      return i;
    }

    /**
     * Find and remove the first element with the given value from an unordered vector.
     *
     * The last element is moved to its place.
     *
     * @return index of the removed element or -1 if not found.
     */
    int excludeUO( const Elem& e )
    {
      int i = aIndex( data, e, count );

      if( i != -1 ) {
        --count;
        data[i] = static_cast<Elem&&>( data[count] );
      }
      return i;
    }

    /**
     * Add an element to the beginning.
     *
     * All elements are shifted to make a gap.
     */
    template <typename Elem_>
    void pushFirst( Elem_&& e )
    {
      hard_assert( uint( count ) < uint( SIZE ) );

      aReverseMove( data + 1, data, count );
      data[0] = static_cast<Elem_&&>( e );
      ++count;
    }

    /**
     * Add an element to the end.
     */
    template <typename Elem_>
    void pushLast( Elem_&& e )
    {
      hard_assert( uint( count ) < uint( SIZE ) );

      data[count] = static_cast<Elem_&&>( e );
      ++count;
    }

    /**
     * Remove the first element.
     *
     * All elements are shifted to fill the gap.
     *
     * @return value of the removed element.
     */
    Elem popFirst()
    {
      Elem e = static_cast<Elem&&>( data[0] );

      --count;
      aMove( data, data + 1, count );

      return e;
    }

    /**
     * Remove the last element.
     *
     * @return value of the removed element.
     */
    Elem popLast()
    {
      hard_assert( count != 0 );

      --count;

      return static_cast<Elem&&>( data[count] );
    }

    /**
     * Sort elements with quicksort.
     */
    void sort()
    {
      aSort( data, count );
    }

    /**
     * Empty the vector.
     */
    void clear()
    {
      count = 0;
    }

    /**
     * Delete all objects referenced by elements and empty the vector.
     */
    void free()
    {
      aFree( data, count );
      clear();
    }

};

}
