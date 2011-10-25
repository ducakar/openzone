/*
 *  DArray.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * @file oz/DArray.hpp
 */

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

      private:

        /// Base class type, convenience definition to make code cleaner.
        typedef oz::CIterator<Elem> B;

        /**
         * %Iterator for the given container, points to its first element.
         */
        OZ_ALWAYS_INLINE
        explicit CIterator( const DArray& a ) : B( a.data, a.data + a.count )
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

      private:

        /// Base class type, convenience definition to make code cleaner.
        typedef oz::Iterator<Elem> B;

        /**
         * %Iterator for the given container, points to its first element.
         */
        OZ_ALWAYS_INLINE
        explicit Iterator( const DArray& a ) : B( a.data, a.data + a.count )
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

    Elem* data;  ///< %Array of elements.
    int   count; ///< Number of elements.

  public:

    /**
     * Create an empty array.
     */
    DArray() : data( null ), count( 0 )
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
    DArray( const DArray& a ) : data( a.count == 0 ? null : new Elem[a.count] ), count( a.count )
    {
      aCopy( data, a.data, a.count );
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

      if( count != a.count ) {
        delete[] data;

        data  = a.count == 0 ? null : new Elem[a.count];
        count = a.count;
      }

      aCopy( data, a.data, a.count );

      return *this;
    }

    /**
     * Create an array with the given size.
     */
    explicit DArray( int size ) : data( size == 0 ? null : new Elem[size] ), count( size )
    {}

    /**
     * Initialise from a C++ array.
     */
    explicit DArray( const Elem* array, int size ) : data( size == 0 ? null : new Elem[size] ),
        count( size )
    {
      aCopy( data, array, count );
    }

    /**
     * True iff the same size and respective elements are equal.
     */
    bool operator == ( const DArray& a ) const
    {
      return count == a.count && aEquals( data, a.data, count );
    }

    /**
     * False iff the same size and respective elements are equal.
     */
    bool operator != ( const DArray& a ) const
    {
      return count != a.count || !aEquals( data, a.data, count );
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
      hard_assert( count > 0 );

      return data;
    }

    /**
     * Pointer to the first element.
     */
    OZ_ALWAYS_INLINE
    operator Elem* ()
    {
      hard_assert( count > 0 );

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
     * True iff empty (no storage is allocated).
     */
    OZ_ALWAYS_INLINE
    bool isEmpty() const
    {
      return count == 0;
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

      return aContains( data, e, count );
    }

    /**
     * Index of the first occurrence of the value or -1 if not found.
     */
    int index( const Elem& e ) const
    {
      hard_assert( count > 0 );

      return aIndex( data, e, count );
    }

    /**
     * Index of the last occurrence of the value or -1 if not found.
     */
    int lastIndex( const Elem& e ) const
    {
      hard_assert( count > 0 );

      return aLastIndex( data, e, count );
    }

    /**
     * Sort elements with quicksort.
     */
    void sort()
    {
      hard_assert( count > 0 );

      aSort( data, count );
    }

    /**
     * Delete objects referenced by elements and set all elements to null.
     */
    void free()
    {
      aFree( data, count );
    }

    /**
     * For an empty array, allocate <tt>initSize</tt> elements.
     */
    void alloc( int initSize )
    {
      hard_assert( count == 0 && initSize > 0 );

      data  = new Elem[initSize];
      count = initSize;
    }

    /**
     * Deallocate data.
     */
    void dealloc()
    {
      delete[] data;

      data  = null;
      count = 0;
    }

};

}
