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
 * @file oz/SVector.hh
 *
 * SVector template class.
 */

#pragma once

#include "arrays.hh"

namespace oz
{

/**
 * Static vector (static array list).
 *
 * Fixed capacity vector with static storage.
 *
 * In contrast with <tt>std::vector</tt> all allocated elements are constructed all the time. This
 * yields slightly better performance and simplifies implementation. However, on element removal its
 * destruction is guaranteed (either explicitly or by via move operation).
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

      private:

        /// Base class type, convenience definition to make code cleaner.
        typedef oz::CIterator<Elem> B;

        /**
         * %Iterator for the given container, points to its first element.
         */
        OZ_ALWAYS_INLINE
        explicit CIterator( const SVector& v ) :
          B( v.data, v.data + v.count )
        {}

      public:

        /**
         * Default constructor, creates an invalid iterator.
         */
        OZ_ALWAYS_INLINE
        CIterator() :
          B( null, null )
        {}

    };

    /**
     * %Iterator with non-constant access to container elements.
     */
    class Iterator : public oz::Iterator<Elem>
    {
      friend class SVector;

      private:

        /// Base class type, convenience definition to make code cleaner.
        typedef oz::Iterator<Elem> B;

        /**
         * %Iterator for the given container, points to its first element.
         */
        OZ_ALWAYS_INLINE
        explicit Iterator( SVector& v ) :
          B( v.data, v.data + v.count )
        {}

      public:

        /**
         * Default constructor, creates an invalid iterator.
         */
        OZ_ALWAYS_INLINE
        Iterator() :
          B( null, null )
        {}

    };

  private:

    Elem data[SIZE]; ///< Element storage.
    int  count;      ///< Number of elements.

  public:

    /**
     * Create an empty vector with capacity SIZE.
     */
    SVector() :
      count( 0 )
    {}

    /**
     * True iff respective elements are equal.
     */
    bool operator == ( const SVector& v ) const
    {
      return count == v.count && aEquals<Elem>( data, v.data, count );
    }

    /**
     * False if respective elements are equal.
     */
    bool operator != ( const SVector& v ) const
    {
      return count != v.count || !aEquals<Elem>( data, v.data, count );
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
      return aContains<Elem>( data, e, count );
    }

    /**
     * Index of the first occurrence of the value or -1 if not found.
     */
    int index( const Elem& e ) const
    {
      return aIndex<Elem>( data, e, count );
    }

    /**
     * Index of the last occurrence of the value or -1 if not found.
     */
    int lastIndex( const Elem& e ) const
    {
      return aLastIndex<Elem>( data, e, count );
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
      pushLast( static_cast<Elem_&&>( e ) );
    }

    /**
     * Add elements from the array to the end.
     */
    void addAll( const Elem* array, int arrayCount )
    {
      int newCount = count + arrayCount;

      hard_assert( uint( newCount ) <= uint( SIZE ) );

      aCopy<Elem>( data + count, array, arrayCount );
      count = newCount;
    }

    /**
     * Add an element to the end if there is no equal element in the vector.
     *
     * @return Position of the inserted or the existing equal element.
     */
    template <typename Elem_>
    int include( Elem_&& e )
    {
      int i = aIndex<Elem>( data, e, count );

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

      aReverseMove<Elem>( data + i + 1, data + i, count - i );
      data[i] = static_cast<Elem_&&>( e );
      ++count;
    }

    /**
     * Remove the last element.
     */
    void remove()
    {
      popLast();
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

      if( i == count ) {
        // When removing the last element, no shift is performed, so its resources are not
        // implicitly destroyed by move operation.
        data[count].~Elem();
        new( data + count ) Elem;
      }
      else {
        aMove<Elem>( data + i, data + i + 1, count - i );
      }
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
     * @return Index of the removed element or -1 if not found.
     */
    int exclude( const Elem& e )
    {
      int i = aIndex<Elem>( data, e, count );

      if( i != -1 ) {
        --count;

        if( i == count ) {
          // When removing the last element, no shift is performed, so its resources are not
          // implicitly destroyed by move operation.
          data[count].~Elem();
          new( data + count ) Elem;
        }
        else {
          aMove<Elem>( data + i, data + i + 1, count - i );
        }
      }
      return i;
    }

    /**
     * Find and remove the first element with the given value from an unordered vector.
     *
     * The last element is moved to its place.
     *
     * @return Index of the removed element or -1 if not found.
     */
    int excludeUO( const Elem& e )
    {
      int i = aIndex<Elem>( data, e, count );

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

      aReverseMove<Elem>( data + 1, data, count );
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
     * Sort elements with quicksort.
     */
    void sort()
    {
      aSort<Elem>( data, count );
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
      aFree<Elem>( data, count );
      clear();
    }

};

}
