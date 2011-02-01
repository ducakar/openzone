/*
 *  SVector.hpp
 *
 *  Static-size vector
 *  Type should be a POD type as C++0x is not yet fully supported by GCC.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "arrays.hpp"

namespace oz
{

  template <typename Type, int SIZE>
  class SVector
  {
    static_assert( SIZE > 0, "SVector size must be at least 1" );

    public:

      /**
       * Constant SVector iterator.
       */
      class CIterator : public oz::CIterator<Type>
      {
        private:

          typedef oz::CIterator<Type> B;

        public:

          /**
           * Default constructor returns an invalid iterator
           */
          OZ_ALWAYS_INLINE
          explicit CIterator() : B( null, null )
          {}

          /**
           * Make iterator for given vector. After creation it points to first element.
           * @param v
           */
          OZ_ALWAYS_INLINE
          explicit CIterator( const SVector& v ) : B( v.data, v.data + v.count )
          {}

      };

      /**
       * SVector iterator.
       */
      class Iterator : public oz::Iterator<Type>
      {
        private:

          typedef oz::Iterator<Type> B;

        public:

          /**
           * Default constructor returns an invalid iterator
           */
          OZ_ALWAYS_INLINE
          explicit Iterator() : B( null, null )
          {}

          /**
           * Make iterator for given vector. After creation it points to first element.
           * @param v
           */
          OZ_ALWAYS_INLINE
          explicit Iterator( SVector& v ) : B( v.data, v.data + v.count )
          {}

      };

    private:

      // data array
      union
      {
        char cdata[ SIZE * sizeof( Type ) ];
        Type data[SIZE];
      };
      // Number of elements in vector
      int count;

    public:

      /**
       * Create empty vector with capacity SIZE.
       */
      explicit SVector() : count( 0 )
      {}

      /**
       * Copy constructor.
       * @param v
       */
      SVector( const SVector& v ) : count( v.count )
      {
        aConstruct( data, v.data, v.count );
      }

      /**
       * Destructor.
       */
      ~SVector()
      {
        aDestruct( data, count );
      }

      /**
       * Copy operator.
       * @param v
       * @return
       */
      SVector& operator = ( const SVector& v )
      {
        assert( &v != this );

        aCopy( data, v.data, v.count );
        count = v.count;
        return *this;
      }

      /**
       * Equality operator.
       * @param v
       * @return true if all elements in both vectors are equal
       */
      bool operator == ( const SVector& v ) const
      {
        return count == v.count && aEquals( data, v.data, count );
      }

      /**
       * Inequality operator.
       * @param v
       * @return false if all elements in both vectors are equal
       */
      bool operator != ( const SVector& v ) const
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
      Iterator iter()
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
        return SIZE;
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
        assert( count < SIZE );

        new( data + count ) Type;
        ++count;
      }

      /**
       * Add an element to the end.
       * @param e
       */
      void add( const Type& e )
      {
        assert( count < SIZE );

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

        assert( SIZE >= newCount );

        for( int i = 0; i < arrayCount; ++i ) {
          aConstruct( data + count, array, arrayCount );
        }
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
          assert( count < SIZE );

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
        assert( count < SIZE );

        if( i == count ) {
          new( data + count ) Type( e );
        }
        else {
          new( data + count ) Type( move( data[count - 1] ) );
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
        assert( count < SIZE );

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
        assert( count < SIZE );

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
        count = 0;
      }

      /**
       * Empty the vector and delete all elements - take care of memory management. Use this
       * function only with vector of pointer that you want to be deleted.
       */
      void free()
      {
        aFree( data, count );
        clear();
      }

  };

}
