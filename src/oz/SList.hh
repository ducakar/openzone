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
 * @file oz/SList.hh
 *
 * SList template class.
 */

#pragma once

#include "arrays.hh"

namespace oz
{

/**
 * %Array list with static storage (fixed capacity).
 *
 * In contrast with <tt>std::vector</tt> all allocated elements are constructed all the time. This
 * yields slightly better performance and simplifies implementation. However, on element removal its
 * destruction is guaranteed (either explicitly or by via move operation).
 *
 * @ingroup oz
 */
template <typename Elem, int SIZE>
class SList
{
  static_assert( SIZE > 0, "SList size must be at least 1" );

  public:

    /**
     * %Iterator with constant access to container elements.
     */
    class CIterator : public oz::CIterator<Elem>
    {
      friend class SList;

      private:

        using oz::CIterator<Elem>::elem;

        /**
         * %Iterator for the given container, points to its first element.
         */
        OZ_ALWAYS_INLINE
        explicit CIterator( const SList& l ) :
          oz::CIterator<Elem>( l.data, l.data + l.count )
        {}

      public:

        /**
         * Default constructor, creates an invalid iterator.
         */
        OZ_ALWAYS_INLINE
        CIterator() :
          oz::CIterator<Elem>( null, null )
        {}

    };

    /**
     * %Iterator with non-constant access to container elements.
     */
    class Iterator : public oz::Iterator<Elem>
    {
      friend class SList;

      private:

        using oz::Iterator<Elem>::elem;

        /**
         * %Iterator for the given container, points to its first element.
         */
        OZ_ALWAYS_INLINE
        explicit Iterator( SList& l ) :
          oz::Iterator<Elem>( l.data, l.data + l.count )
        {}

      public:

        /**
         * Default constructor, creates an invalid iterator.
         */
        OZ_ALWAYS_INLINE
        Iterator() :
          oz::Iterator<Elem>( null, null )
        {}

    };

  private:

    Elem data[SIZE]; ///< Element storage.
    int  count;      ///< Number of elements.

  public:

    /**
     * Create an empty list with capacity SIZE.
     */
    SList() :
      count( 0 )
    {}

    /**
     * True iff respective elements are equal.
     */
    bool operator == ( const SList& l ) const
    {
      return count == l.count && aEquals<Elem>( data, l.data, count );
    }

    /**
     * False iff respective elements are equal.
     */
    bool operator != ( const SList& l ) const
    {
      return count != l.count || !aEquals<Elem>( data, l.data, count );
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
    template <typename Elem_ = Elem>
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
     * Add an element to the end if there is no equal element in the list.
     *
     * @return Position of the inserted or the existing equal element.
     */
    template <typename Elem_ = Elem>
    int include( Elem_&& e )
    {
      int i = aIndex<Elem, Elem>( data, e, count );

      if( i < 0 ) {
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
    template <typename Elem_ = Elem>
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
     * Remove the element at the given position from an unordered list.
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
      int i = aIndex<Elem, Elem>( data, e, count );

      if( i >= 0 ) {
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
     * Find and remove the first element with the given value from an unordered list.
     *
     * The last element is moved to its place.
     *
     * @return Index of the removed element or -1 if not found.
     */
    int excludeUO( const Elem& e )
    {
      int i = aIndex<Elem, Elem>( data, e, count );

      if( i >= 0 ) {
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
    template <typename Elem_ = Elem>
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
    template <typename Elem_ = Elem>
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
     * Empty the list.
     */
    void clear()
    {
      // Ensure destruction of all elements.
      for( int i = 0; i < count; ++i ) {
        data[i].~Elem();
        new( data + i ) Elem;
      }

      count = 0;
    }

    /**
     * Delete all objects referenced by elements and empty the list.
     */
    void free()
    {
      aFree<Elem>( data, count );
      clear();
    }

};

}
