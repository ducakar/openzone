/*
 *  SVector.hpp
 *
 *  Fixed capacity vector with static storage.
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
          CIterator() : B( null, null )
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
          Iterator() : B( null, null )
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
      Type data[SIZE];
      // Number of elements in vector
      int  count;

    public:

      /**
       * Create empty vector with capacity SIZE.
       */
      SVector() : count( 0 )
      {}

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
        hard_assert( uint( i ) < uint( count ) );

        return data[i];
      }

      /**
       * @param i
       * @return reference i-th element
       */
      OZ_ALWAYS_INLINE
      Type& operator [] ( int i )
      {
        hard_assert( uint( i ) < uint( count ) );

        return data[i];
      }

      /**
       * @return constant reference to first element
       */
      OZ_ALWAYS_INLINE
      const Type& first() const
      {
        hard_assert( count != 0 );

        return data[0];
      }

      /**
       * @return reference to first element
       */
      OZ_ALWAYS_INLINE
      Type& first()
      {
        hard_assert( count != 0 );

        return data[0];
      }

      /**
       * @return constant reference to last element
       */
      OZ_ALWAYS_INLINE
      const Type& last() const
      {
        hard_assert( count != 0 );

        return data[count - 1];
      }

      /**
       * @return reference to last element
       */
      OZ_ALWAYS_INLINE
      Type& last()
      {
        hard_assert( count != 0 );

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
        hard_assert( uint( count ) < uint( SIZE ) );

        ++count;
      }

      /**
       * Add an element to the end.
       * @param e
       */
      void add( const Type& e )
      {
        hard_assert( uint( count ) < uint( SIZE ) );

        data[count] = e;
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

        hard_assert( uint( newCount ) <= uint( SIZE ) );

        for( int i = 0; i < arrayCount; ++i ) {
          aCopy( data + count, array, arrayCount );
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
          hard_assert( uint( count ) < uint( SIZE ) );

          data[count] = e;
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
        hard_assert( uint( i ) <= uint( count ) );
        hard_assert( uint( count ) < uint( SIZE ) );

        if( i == count ) {
          data[count] = e;
        }
        else {
          aReverseCopy( data + i + 1, data + i, count - i );
          data[i] = e;
        }
        ++count;
      }

      /**
       * Remove last element.
       */
      void remove()
      {
        hard_assert( count != 0 );

        --count;
      }

      /**
       * Remove the element at given position. All later element are shifted to fill the gap.
       * @param i
       */
      void remove( int i )
      {
        hard_assert( uint( i ) < uint( count ) );

        --count;
        aCopy( data + i, data + i + 1, count - i );
      }

      /**
       * Remove the element at given position from unordered vector. The last element is moved to
       * the position to fill the gap.
       * @param i
       */
      void removeUO( int i )
      {
        hard_assert( uint( i ) < uint( count ) );

        --count;
        data[i] = data[count];
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
          data[i] = data[count];
        }
        return i;
      }

      /**
       * Add an element to the beginning.
       * @param e
       */
      void pushFirst( const Type& e )
      {
        hard_assert( uint( count ) < uint( SIZE ) );

        if( count == 0 ) {
          data[0] = e;
          ++count;
        }
        else {
          aReverseCopy( data + 1, data, count );
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
        hard_assert( uint( count ) < uint( SIZE ) );

        data[count] = e;
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

        return e;
      }

      /**
       * Remove last element
       * @return value of removed element
       */
      Type popLast()
      {
        hard_assert( count != 0 );

        --count;
        Type e = data[count];

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
