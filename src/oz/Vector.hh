/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file oz/Vector.hh
 */

#pragma once

#include "arrays.hh"

namespace oz
{

/**
 * %Vector (array list).
 *
 * It can also be used as a stack or a small set.
 *
 * Memory is allocated when the first element is added.
 *
 * @ingroup oz
 */
template <typename Elem>
class Vector
{
  private:

    /// Granularity for automatic capacity allocations.
    static const int GRANULARITY = 8;

  public:

    /**
     * %Iterator with constant access to container elements.
     */
    class CIterator : public oz::CIterator<Elem>
    {
      friend class Vector;

      OZ_RANGE_ITERATOR( CIterator )

      private:

        /// Base class type, convenience definition to make code cleaner.
        typedef oz::CIterator<Elem> B;

        /**
         * %Iterator for the given container, points to its first element.
         */
        OZ_ALWAYS_INLINE
        explicit CIterator( const Vector& v ) :
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
      friend class Vector;

      OZ_RANGE_ITERATOR( Iterator )

      private:

        /// Base class type, convenience definition to make code cleaner.
        typedef oz::Iterator<Elem> B;

        /**
         * %Iterator for the given container, points to its first element.
         */
        OZ_ALWAYS_INLINE
        explicit Iterator( const Vector& v ) :
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

    Elem* data;  ///< Element storage.
    int   size;  ///< Capacity, number of elements in storage.
    int   count; ///< Number of elements.

    /**
     * Double capacity if there is not enough space to add another element.
     */
    void ensureCapacity()
    {
      if( size == count ) {
        size = size == 0 ? GRANULARITY : 2 * size;
        data = aRealloc( data, count, size );
      }
    }

    /**
     * Enlarge capacity to the smallest multiple of GRANULARITY able to hold requested number of
     * elements.
     */
    void ensureCapacity( int desiredSize )
    {
      if( size < desiredSize ) {
        size = ( ( desiredSize - 1 ) / GRANULARITY + 1 ) * GRANULARITY;
        data = aRealloc( data, count, size );
      }
    }

  public:

    /**
     * Create an empty vector.
     */
    Vector() :
      data( null ), size( 0 ), count( 0 )
    {}

    /**
     * Destructor.
     */
    ~Vector()
    {
      delete[] data;
    }

    /**
     * Copy constructor, copies elements.
     */
    Vector( const Vector& v ) :
      data( v.size == 0 ? null : new Elem[v.size] ), size( v.size ), count( v.count )
    {
      aCopy( data, v.data, v.count );
    }

    /**
     * Move constructor, moves element storage.
     */
    Vector( Vector&& v ) :
      data( v.data ), size( v.size ), count( v.count )
    {
      v.data  = null;
      v.size  = 0;
      v.count = 0;
    }

    /**
     * Copy operator, copies elements.
     *
     * Reuse existing storage if it suffices.
     */
    Vector& operator = ( const Vector& v )
    {
      if( &v == this ) {
        return *this;
      }

      if( size < v.count ) {
        delete[] data;

        data = new Elem[v.size];
        size = v.size;
      }

      aCopy( data, v.data, v.count );
      count = v.count;

      return *this;
    }

    /**
     * Move operator, moves element storage.
     */
    Vector& operator = ( Vector&& v )
    {
      if( &v == this ) {
        return *this;
      }

      delete[] data;

      data  = v.data;
      size  = v.size;
      count = v.count;

      v.data  = null;
      v.size  = 0;
      v.count = 0;

      return *this;
    }

    /**
     * Create an empty vector with the given initial capacity.
     */
    explicit Vector( int size_ ) :
      data( new Elem[size_] ), size( size_ ), count( 0 )
    {}

    /**
     * True iff respective elements are equal.
     */
    bool operator == ( const Vector& v ) const
    {
      return count == v.count && aEquals( data, v.data, count );
    }

    /**
     * False iff respective elements are equal.
     */
    bool operator != ( const Vector& v ) const
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
     * Number of allocated elements.
     */
    OZ_ALWAYS_INLINE
    int capacity() const
    {
      return size;
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
      ensureCapacity();

      ++count;
    }

    /**
     * Add an element to the end.
     */
    template <typename Elem_>
    void add( Elem_&& e )
    {
      ensureCapacity();

      data[count] = static_cast<Elem_&&>( e );
      ++count;
    }

    /**
     * Add elements from the array to the end.
     */
    void addAll( const Elem* array, int arrayCount )
    {
      int newCount = count + arrayCount;

      ensureCapacity( newCount );

      aCopy( data + count, array, arrayCount );
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
      int i = aIndex( data, e, count );

      if( i == -1 ) {
        ensureCapacity();

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

      ensureCapacity();

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
     * @return Index of the removed element or -1 if not found.
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
     * @return Index of the removed element or -1 if not found.
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
      ensureCapacity();

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
      ensureCapacity();

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
      aMove( data, data + 1, count );

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

    /**
     * For an empty vector with no allocated storage, allocate capacity for <tt>size_</tt> elements.
     */
    void alloc( int size_ )
    {
      hard_assert( size == 0 && size_ > 0 );

      data = new Elem[size_];
      size = size_;
    }

    /**
     * Deallocate storage of an empty vector.
     */
    void dealloc()
    {
      hard_assert( count == 0 );

      delete[] data;

      data = null;
      size = 0;
    }

    /**
     * Trim vector capacity to the least multiple of <tt>GRANULARITY</tt> that can hold the
     * elements.
     */
    void trim()
    {
      int newSize = ( ( count - 1 ) / GRANULARITY + 1 ) * GRANULARITY;

      if( newSize < size ) {
        size = newSize;
        data = aRealloc( data, count, size );
      }
    }

};

}
