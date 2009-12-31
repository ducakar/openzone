/*
 *  Array.h
 *
 *  Static-size array.
 *  The advantage over C++ arrays it that is has bounds checking and an iterator.
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

namespace oz
{

  template <class Type, int SIZE>
  struct Array
  {
    public:

      /**
       * Array iterator.
       */
      struct Iterator : oz::Iterator<Type>
      {
        private:

          // base class
          typedef oz::Iterator<Type> B;

        public:

          /**
           * Default constructor returns a dummy passed iterator
           * @return
           */
          explicit Iterator() : B( null, null )
          {}

          /**
           * Make iterator for given array. After creation it points to first element.
           * @param v
           */
          explicit Iterator( Array& a ) : B( a.data, a.data + SIZE )
          {}

      };

    private:

      Type data[SIZE];

    public:

      /**
       * Copy operator.
       * @param a
       * @return
       */
      Array& operator = ( const Array& a )
      {
        assert( &a != this );

        aCopy( data, a.data, SIZE );
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
      Iterator iterator()
      {
        return Iterator( *this );
      }

      /**
       * Get pointer to <code>data</code> array. Use with caution, since you can easily make buffer
       * overflows if you don't check the size of <code>data</code> array.
       * @return non-constant pointer to data array
       */
      operator Type* ()
      {
        return data;
      }

      /**
       * Get pointer to <code>data</code> array. Use with caution, since you can easily make buffer
       * overflows if you don't check the size of <code>data</code> array.
       * @return constant pointer to data array
       */
      operator const Type* () const
      {
        return data;
      }

      /**
       * @return number of elements in the array
       */
      int length() const
      {
        return SIZE;
      }

      /**
       * @param e
       * @return true if the element is found in the array
       */
      bool contains( const Type& e ) const
      {
        for( int i = 0; i < SIZE; i++ ) {
          if( data[i] == e ) {
            return true;
          }
        }
        return false;
      }

      /**
       * @param i
       * @return reference i-th element
       */
      Type& operator [] ( int i )
      {
        assert( 0 <= i && i < SIZE );

        return data[i];
      }

      /**
       * @param i
       * @return constant reference i-th element
       */
      const Type& operator [] ( int i ) const
      {
        assert( 0 <= i && i < SIZE );

        return data[i];
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
       * @return reference to first element
       */
      Type& first()
      {
        return data[0];
      }

      /**
       * @return constant reference to first element
       */
      const Type& first() const
      {
        return data[0];
      }

      /**
       * @return reference to last element
       */
      Type& last()
      {
        return data[SIZE - 1];
      }

      /**
       * @return constant reference to last element
       */
      const Type& last() const
      {
        return data[SIZE - 1];
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
