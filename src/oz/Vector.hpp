/*
 *  Vector.hpp
 *
 *  Vector
 *  It can also be used as a stack or a small set.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "arrays.hpp"
#include "Alloc.hpp"

namespace oz
{

  template <typename Type, int GRANULARITY = 8>
  class Vector
  {
    static_assert( GRANULARITY > 0, "GRANULARITY must be at least 1" );

    public:

      /**
       * Constant Vector iterator.
       */
      class CIterator : public oz::CIterator<Type>
      {
        private:

          typedef oz::CIterator<Type> B;

        public:

          /**
           * Default constructor returns an invalid iterator
           */
          explicit CIterator() : B( null, null )
          {}

          /**
           * Make iterator for given vector. After creation it points to first element.
           * @param v
           */
          explicit CIterator( const Vector& v ) : B( v.data, v.data + v.count )
          {}

      };

      /**
       * Vector iterator.
       */
      class Iterator : public oz::Iterator<Type>
      {
        private:

          typedef oz::Iterator<Type> B;

        public:

          /**
           * Default constructor returns an invalid iterator
           */
          explicit Iterator() : B( null, null )
          {}

          /**
           * Make iterator for given vector. After creation it points to first element.
           * @param v
           */
          explicit Iterator( const Vector& v ) : B( v.data, v.data + v.count )
          {}

      };

    private:

      // Pointer to data array
      Type* data;
      // Size of data array
      int   size;
      // Number of elements in the vector
      int   count;

      /**
       * Enlarge capacity by two times if there's not enough space to add another element.
       */
      void ensureCapacity()
      {
        if( size == count ) {
          if( size == 0 ) {
            size = GRANULARITY;
            data = Alloc::alloc<Type>( size );
          }
          else {
            size *= 2;
            data = Alloc::realloc( data, count, size );
          }
        }
      }

      /**
       * Enlarge capacity to the smallest power of 2, greater or equal to desired capacity.
       */
      void ensureCapacity( int desiredSize )
      {
        if( size < desiredSize ) {
          size = size == 0 ? GRANULARITY : size;

          do {
            size *= 2;
          }
          while( size < desiredSize );

          if( data == null ) {
            data = Alloc::alloc<Type>( size );
          }
          else {
            data = Alloc::realloc( data, count, size );
          }
        }
      }

    public:

      /**
       * Create empty vector with initial capacity 8.
       */
      explicit Vector() : data( null ), size( 0 ), count( 0 )
      {}

      /**
       * Create empty vector with given initial capacity.
       * @param initSize
       */
      explicit Vector( int initSize ) : data( initSize == 0 ? null : Alloc::alloc<Type>( initSize ) ),
          size( initSize ), count( 0 )
      {}

      /**
       * Copy constructor.
       * @param v
       */
      Vector( const Vector& v ) : data( v.size == 0 ? null : Alloc::alloc<Type>( v.size ) ),
          size( v.size ), count( v.count )
      {
        aConstruct( data, v.data, v.count );
      }

      /**
       * Destructor.
       */
      ~Vector()
      {
        aDestruct( data, count );
        Alloc::dealloc( data );
      }

      /**
       * Copy operator.
       * @param v
       * @return
       */
      Vector& operator = ( const Vector& v )
      {
        assert( &v != this );

        aDestruct( data, count );

        if( size < v.count ) {
          Alloc::dealloc( data );

          data = Alloc::alloc<Type>( v.size );
          size = v.size;
        }

        aConstruct( data, v.data, v.count );
        count = v.count;
        return *this;
      }

      /**
       * Equality operator. Capacity of vectors doesn't matter.
       * @param v
       * @return true if all elements in both vectors are equal
       */
      bool operator == ( const Vector& v ) const
      {
        return count == v.count && aEquals( data, v.data, count );
      }

      /**
       * Inequality operator. Capacity of vectors doesn't matter.
       * @param v
       * @return false if all elements in both vectors are equal
       */
      bool operator != ( const Vector& v ) const
      {
        return count != v.count || !aEquals( data, v.data, count );
      }

      /**
       * @return constant iterator for this vector
       */
      OZ_ALWAYS_INLINE
      CIterator citer() const
      {
        return CIterator( *this );
      }

      /**
       * @return iterator for this vector
       */
      OZ_ALWAYS_INLINE
      Iterator iter() const
      {
        return Iterator( *this );
      }

      /**
       * Get pointer to <code>data</code> array. Use with caution, since you can easily make buffer
       * overflows if you don't check the size of <code>data</code> array.
       * @return constant pointer to data array
       */
      OZ_ALWAYS_INLINE
      operator const Type* () const
      {
        return data;
      }

      /**
       * Get pointer to <code>data</code> array. Use with caution, since you can easily make buffer
       * overflows if you don't check the size of <code>data</code> array.
       * @return non-constant pointer to data array
       */
      OZ_ALWAYS_INLINE
      operator Type* ()
      {
        return data;
      }

      /**
       * @return number of elements in the vector
       */
      OZ_ALWAYS_INLINE
      int length() const
      {
        return count;
      }

      /**
       * @return true if vector has no elements
       */
      OZ_ALWAYS_INLINE
      bool isEmpty() const
      {
        return count == 0;
      }

      /**
       * @return capacity of the vector
       */
      OZ_ALWAYS_INLINE
      int capacity() const
      {
        return size;
      }

      /**
       * @param e
       * @return true if the element is found in the vector
       */
      bool contains( const Type& e ) const
      {
        return aContains( data, e, count );
      }

      /**
       * @param i
       * @return constant reference i-th element
       */
      OZ_ALWAYS_INLINE
      const Type& operator [] ( int i ) const
      {
        assert( uint( i ) < uint( count ) );

        return data[i];
      }

      /**
       * @param i
       * @return reference i-th element
       */
      OZ_ALWAYS_INLINE
      Type& operator [] ( int i )
      {
        assert( uint( i ) < uint( count ) );

        return data[i];
      }

      /**
       * @return constant reference to first element
       */
      OZ_ALWAYS_INLINE
      const Type& first() const
      {
        assert( count != 0 );

        return data[0];
      }

      /**
       * @return reference to first element
       */
      OZ_ALWAYS_INLINE
      Type& first()
      {
        assert( count != 0 );

        return data[0];
      }

      /**
       * @return constant reference to last element
       */
      OZ_ALWAYS_INLINE
      const Type& last() const
      {
        assert( count != 0 );

        return data[count - 1];
      }

      /**
       * @return reference to last element
       */
      OZ_ALWAYS_INLINE
      Type& last()
      {
        assert( count != 0 );

        return data[count - 1];
      }

      /**
       * Find the first occurrence of an element.
       * @param e
       * @return index of first occurrence, -1 if not found
       */
      int index( const Type& e ) const
      {
        return aIndex( data, e, count );
      }

      /**
       * Find the last occurrence of an element.
       * @param e
       * @return index of last occurrence, -1 if not found
       */
      int lastIndex( const Type& e ) const
      {
        return aLastIndex( data, e, count );
      }

      /**
       * Create slot for a new element at the end.
       */
      void add()
      {
        ensureCapacity();

        new( data + count ) Type;
        ++count;
      }

      /**
       * Add an element to the end.
       * @param e
       */
      void add( const Type& e )
      {
        ensureCapacity();

        new( data + count ) Type( e );
        ++count;
      }

      /**
       * Add all elements from an array to the end.
       * @param array
       * @param arrayCount
       */
      void addAll( const Type* array, int arrayCount )
      {
        int newCount = count + arrayCount;

        ensureCapacity( newCount );

        aConstruct( data + count, array, arrayCount );
        count = newCount;
      }

      /**
       * Add an element to the end, but only if there's no any equal element in the vector.
       * This function is useful if you plan to use vector as a set.
       * @param e
       * @return position of the inserted element or an existing one if it was not inserted
       */
      int include( const Type& e )
      {
        int i = aIndex( data, e, count );

        if( i == -1 ) {
          ensureCapacity();

          new( data + count ) Type( e );
          i = count;
          ++count;
        }
        return i;
      }

      /**
       * Insert an element at given position. All later elements are shifted to make a gap
       * for the new element.
       * @param e
       * @param i
       */
      void insert( int i, const Type& e )
      {
        assert( uint( i ) <= uint( count ) );

        ensureCapacity();

        if( i == count ) {
          new( data + count ) Type( e );
        }
        else {
          new( data + count ) Type( data[count - 1] );
          aReverseCopy( data + i + 1, data + i, count - i - 1 );
          data[i] = e;
        }
        ++count;
      }

      /**
       * Remove last element.
       */
      void remove()
      {
        assert( count != 0 );

        --count;
        data[count].~Type();
        return *this;
      }

      /**
       * Remove the element at given position. All later element are shifted to fill the gap.
       * @param i
       */
      void remove( int i )
      {
        assert( uint( i ) < uint( count ) );

        --count;
        aCopy( data + i, data + i + 1, count - i );
        data[count].~Type();
      }

      /**
       * Remove the element at given position from unordered vector. The last element is moved to
       * the position to fill the gap.
       * @param i
       */
      void removeUO( int i )
      {
        assert( uint( i ) < uint( count ) );

        --count;
        if( i != count ) {
          data[i] = data[count];
        }
        data[count].~Type();
      }

      /**
       * Find and remove the given element.
       * @param e
       * @return
       */
      int exclude( const Type& e )
      {
        int i = aIndex( data, e, count );

        if( i != -1 ) {
          --count;
          aCopy( data + i, data + i + 1, count - i );
          data[count].~Type();
        }
        return i;
      }

      /**
       * Find and remove the given element from unordered vector (last element is moved to fill
       * the gap.)
       * @param e
       * @return
       */
      int excludeUO( const Type& e )
      {
        int i = aIndex( data, e, count );

        if( i != -1 ) {
          --count;
          if( i != count ) {
            data[i] = data[count];
          }
          data[count].~Type();
        }
        return i;
      }

      /**
       * Add an element to the beginning.
       * @param e
       */
      void pushFirst( const Type& e )
      {
        ensureCapacity();

        if( count == 0 ) {
          new( data + 0 ) Type( e );
          ++count;
        }
        else {
          new( data + count ) Type( data[count - 1] );
          aReverseCopy( data + 1, data, count - 1 );
          data[0] = e;
          ++count;
        }
      }

      /**
       * Add an element to the end.
       * @param e
       */
      void pushLast( const Type& e )
      {
        ensureCapacity();

        new( data + count ) Type( e );
        ++count;
      }

      /**
       * Remove first element
       * @return value of removed element
       */
      Type popFirst()
      {
        Type e = data[0];

        --count;
        aCopy( data, data + 1, count );
        data[count].~Type();

        return e;
      }

      /**
       * Remove last element
       * @return value of removed element
       */
      Type popLast()
      {
        assert( count != 0 );

        --count;
        Type e = data[count];
        data[count].~Type();

        return e;
      }

      /**
       * Sort elements with quicksort algorithm (last element as pivot).
       */
      void sort()
      {
        aSort( data, count );
      }

      /**
       * Empty the vector but don't delete the elements.
       */
      void clear()
      {
        aDestruct( data, count );
        Alloc::dealloc( data );

        data = null;
        size = 0;
        count = 0;
      }

      /**
       * Empty the vector and delete all elements - take care of memory management. Use this
       * function only with vector of pointers that you want to be deleted.
       */
      void free()
      {
        aFree( data, count );
        clear();
      }

      /**
       * Trim vector, leave at most <code>left</code> elements/capacity.
       * @param left
       */
      void trim()
      {
        int newSize = ( ( count - 1 ) / GRANULARITY + 1 ) * GRANULARITY;

        if( newSize < size ) {
          size = newSize;
          data = Alloc::realloc( data, count, size );
        }
      }

  };

}
