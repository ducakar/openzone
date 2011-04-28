/*
 *  Array.hpp
 *
 *  Static-size array.
 *  The advantage over C++ arrays it that is has bounds checking and an iterator.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "arrays.hpp"

namespace oz
{

  template <typename Type, int SIZE>
  class Array
  {
    static_assert( SIZE > 0, "Array size must be at least 1" );

    public:

      /**
       * Constant Array iterator.
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
           * Make iterator for given array. After creation it points to first element.
           * @param v
           */
          OZ_ALWAYS_INLINE
          explicit CIterator( const Array& a ) : B( a.data, a.data + SIZE )
          {}

      };

      /**
       * Array iterator.
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
           * Make iterator for given array. After creation it points to first element.
           * @param v
           */
          OZ_ALWAYS_INLINE
          explicit Iterator( Array& a ) : B( a.data, a.data + SIZE )
          {}

      };

    private:

      Type data[SIZE];

    public:

      /**
       * Default constructor.
       */
      Array() = default;

      /**
       * Initialise from a C++ array.
       * @param array
       */
      explicit Array( const Type* array )
      {
        aCopy( data, array, SIZE );
      }

      /**
       * Initialise from an initialiser list.
       * @param l
       */
      Array( initializer_list<Type> l )
      {
        hard_assert( int( l.size() ) == SIZE );

        aCopy( data, l.begin(), SIZE );
      }

      /**
       * Copy from an initialiser list.
       * @param l
       * @return
       */
      Array& operator = ( initializer_list<Type> l )
      {
        hard_assert( int( l.size() ) == SIZE );

        aCopy( data, l.begin(), SIZE );
        return *this;
      }

      /**
       * Equality operator.
       * @param a
       * @return true if all elements in both arrays are equal
       */
      bool operator == ( const Array& a ) const
      {
        return aEquals( data, a.data, SIZE );
      }

      /**
       * Inequality operator.
       * @param a
       * @return false if all elements in both arrays are equal
       */
      bool operator != ( const Array& a ) const
      {
        return !aEquals( data, a.data, SIZE );
      }

      /**
       * @return iterator for this array
       */
      OZ_ALWAYS_INLINE
      CIterator citer() const
      {
        return CIterator( *this );
      }

      /**
       * @return iterator for this array
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
       * @return number of elements in the array
       */
      OZ_ALWAYS_INLINE
      int length() const
      {
        return SIZE;
      }

      /**
       * @return true if vector has no elements
       */
      OZ_ALWAYS_INLINE
      bool isEmpty() const
      {
        return SIZE == 0;
      }

      /**
       * @param e
       * @return true if the element is found in the array
       */
      bool contains( const Type& e ) const
      {
        return aContains( data, e, SIZE );
      }

      /**
       * @param i
       * @return constant reference i-th element
       */
      OZ_ALWAYS_INLINE
      const Type& operator [] ( int i ) const
      {
        hard_assert( uint( i ) < uint( SIZE ) );

        return data[i];
      }

      /**
       * @param i
       * @return reference i-th element
       */
      OZ_ALWAYS_INLINE
      Type& operator [] ( int i )
      {
        hard_assert( uint( i ) < uint( SIZE ) );

        return data[i];
      }

      /**
       * @return constant reference to first element
       */
      OZ_ALWAYS_INLINE
      const Type& first() const
      {
        return data[0];
      }

      /**
       * @return reference to first element
       */
      OZ_ALWAYS_INLINE
      Type& first()
      {
        return data[0];
      }

      /**
       * @return constant reference to last element
       */
      OZ_ALWAYS_INLINE
      const Type& last() const
      {
        return data[SIZE - 1];
      }

      /**
       * @return reference to last element
       */
      OZ_ALWAYS_INLINE
      Type& last()
      {
        return data[SIZE - 1];
      }

      /**
       * Find the first occurrence of an element.
       * @param e
       * @return index of first occurrence, -1 if not found
       */
      int index( const Type& e ) const
      {
        return aIndex( data, e, SIZE );
      }

      /**
       * Find the last occurrence of an element.
       * @param e
       * @return index of last occurrence, -1 if not found
       */
      int lastIndex( const Type& e ) const
      {
        return aLastIndex( data, e, SIZE );
      }

      /**
       * Sort elements with quicksort algorithm (last element as pivot).
       */
      void sort()
      {
        aSort( data, SIZE );
      }

      /**
       * Empty the list and delete all elements - take care of memory management. Use this function
       * only with array of pointer that you want to be deleted.
       */
      void free()
      {
        aFree( data, SIZE );
      }

  };

}
