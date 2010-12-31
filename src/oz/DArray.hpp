/*
 *  DArray.hpp
 *
 *  Dynamic array.
 *  The advantage over C++ arrays is it has bounds checking, iterator and it frees allocated
 *  memory when destroyed.
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "arrays.hpp"

namespace oz
{

  template <typename Type>
  class DArray
  {
    public:

      /**
       * Constant DArray iterator.
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
           * Make iterator for given array. After creation it points to first element.
           * @param v
           */
          explicit CIterator( const DArray& a ) : B( a.data, a.data + a.count )
          {}

      };

      /**
       * DArray iterator.
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
           * Make iterator for given array. After creation it points to first element.
           * @param v
           */
          explicit Iterator( const DArray& a ) : B( a.data, a.data + a.count )
          {}

      };

    private:

      Type* data;
      int   count;

    public:

      /**
       * Create a null array.
       */
      explicit DArray() : data( null ), count( 0 )
      {}

      /**
       * Create an empty array with the given size.
       * @param size
       */
      explicit DArray( int size ) : data( new Type[size] ), count( size )
      {}

      /**
       * Initialise from a C++ array.
       * @param array
       * @param size
       */
      explicit DArray( const Type* array, int size ) : data( new Type[size] ), count( size )
      {
        aCopy( data, array, size );
      }

      /**
       * Copy constructor.
       * @param a
       */
      DArray( const DArray& a ) : data( new Type[a.count] ), count( a.count )
      {
        aCopy( data, a.data, count );
      }

      /**
       * Destructor.
       */
      ~DArray()
      {
        delete[] data;
      }

      /**
       * Copy operator.
       * @param a
       * @return
       */
      DArray& operator = ( const DArray& a )
      {
        assert( &a != this );
        assert( a.count != 0 );

        if( count != a.count ) {
          delete[] data;
          data = new Type[a.count];
          count = a.count;
        }

        aCopy( data, a.data, a.count );
        return *this;
      }

      /**
       * Equality operator.
       * @param a
       * @return true if all elements in both arrays are equal
       */
      bool operator == ( const DArray& a ) const
      {
        return count == a.count && aEquals( data, a.data, count );
      }

      /**
       * Inequality operator.
       * @param a
       * @return false if all elements in both arrays are equal
       */
      bool operator != ( const DArray& a ) const
      {
        return count != a.count || !aEquals( data, a.data, count );
      }

      /**
       * @return constant iterator for this array
       */
      CIterator citer() const
      {
        return CIterator( *this );
      }

      /**
       * @return iterator for this array
       */
      Iterator iter() const
      {
        return Iterator( *this );
      }

      /**
       * Get pointer to <code>data</code> array. Use with caution, since you can easily make buffer
       * overflows if you don't check the size of <code>data</code> array.
       * @return constant pointer to data array
       */
      operator const Type* () const
      {
        assert( count > 0 );

        return data;
      }

      /**
       * Get pointer to <code>data</code> array. Use with caution, since you can easily make buffer
       * overflows if you don't check the size of <code>data</code> array.
       * @return non-constant pointer to data array
       */
      operator Type* ()
      {
        assert( count > 0 );

        return data;
      }

      /**
       * @return number of elements in the array
       */
      int length() const
      {
        return count;
      }

      /**
       * @return true if memory is NOT allocated (i.e. count == 0)
       */
      bool isEmpty() const
      {
        return count == 0;
      }

      /**
       * Allocate memory
       * It does not delete the allocated memory, it assumes it hasn't been allocated yet. It's
       * meant to be called after DArray( void ) ctor, when the size becomes known.
       * @param size new size
       */
      void setSize( int size )
      {
        assert( size > 0 );

        delete[] data;

        data  = new Type[size];
        count = size;
      }

      /**
       * @param e
       * @return true if the element is found in the array
       */
      bool contains( const Type& e ) const
      {
        assert( count > 0 );

        return aContains( data, e, count );
      }

      /**
       * @param i
       * @return constant reference i-th element
       */
      const Type& operator [] ( int i ) const
      {
        assert( uint( i ) < uint( count ) );

        return data[i];
      }

      /**
       * @param i
       * @return reference i-th element
       */
      Type& operator [] ( int i )
      {
        assert( uint( i ) < uint( count ) );

        return data[i];
      }

      /**
       * Find the first occurrence of an element.
       * @param e
       * @return index of first occurrence, -1 if not found
       */
      int index( const Type& e ) const
      {
        assert( count > 0 );

        return aIndex( data, e, count );
      }

      /**
       * Find the last occurrence of an element.
       * @param e
       * @return index of last occurrence, -1 if not found
       */
      int lastIndex( const Type& e ) const
      {
        assert( count > 0 );

        return aLastIndex( data, e, count );
      }

      /**
       * @return constant reference to first element
       */
      const Type& first() const
      {
        assert( count > 0 );

        return data[0];
      }

      /**
       * @return reference to first element
       */
      Type& first()
      {
        assert( count > 0 );

        return data[0];
      }

      /**
       * @return constant reference to last element
       */
      const Type& last() const
      {
        assert( count > 0 );

        return data[count - 1];
      }

      /**
       * @return reference to last element
       */
      Type& last()
      {
        assert( count > 0 );

        return data[count - 1];
      }

      /**
       * Sort elements with quicksort algorithm (last element as pivot).
       */
      void sort()
      {
        assert( count > 0 );

        aSort( data, count );
      }

      /**
       * Delete the array but don't delete the elements.
       */
      void clear()
      {
        delete[] data;

        data  = null;
        count = 0;
      }

      /**
       * Delete the array and delete all elements - take care of memory management. Use this
       * function only with array of pointers that you want to be deleted.
       */
      void free()
      {
        aFree( data, count );
        clear();
      }

  };

}
