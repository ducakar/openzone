/*
 *  DArray.hpp
 *
 *  Dynamic array.
 *  The advantage over C++ arrays is it has bounds checking, iterator and it frees allocated
 *  memory when destroyed.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "arrays.hpp"
#include "Alloc.hpp"

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
          OZ_ALWAYS_INLINE
          CIterator() : B( null, null )
          {}

          /**
           * Make iterator for given array. After creation it points to first element.
           * @param v
           */
          OZ_ALWAYS_INLINE
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
          OZ_ALWAYS_INLINE
          Iterator() : B( null, null )
          {}

          /**
           * Make iterator for given array. After creation it points to first element.
           * @param v
           */
          OZ_ALWAYS_INLINE
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
       * Copy constructor.
       * @param a
       */
      DArray( const DArray& a ) : data( a.count == 0 ? null : new Type[a.count] ), count( a.count )
      {
        aCopy( data, a.data, a.count );
      }

      /**
       * Move constructor.
       * @param a
       */
      DArray( DArray&& a ) : data( a.data ), count( a.count )
      {
        a.data  = null;
        a.count = 0;
      }

      /**
       * Copy operator.
       * @param a
       * @return
       */
      DArray& operator = ( const DArray& a )
      {
        if( &a == this ) {
          soft_assert( &a != this );
          return *this;
        }

        if( count != a.count ) {
          delete[] data;

          data  = a.count == 0 ? null : new Type[a.count];
          count = a.count;
        }

        aCopy( data, a.data, a.count );
        return *this;
      }

      /**
       * Move operator.
       * @param a
       * @return
       */
      DArray& operator = ( DArray&& a )
      {
        if( &a == this ) {
          soft_assert( &a != this );
          return *this;
        }

        delete[] data;

        data  = a.data;
        count = a.count;

        a.data  = null;
        a.count = 0;

        return *this;
      }

      /**
       * Create an array with the given size.
       * @param size
       */
      explicit DArray( int size ) : data( size == 0 ? null : new Type[size] ), count( size )
      {}

      /**
       * Initialise from a C++ array.
       * @param array
       * @param size
       */
      explicit DArray( const Type* array, int size ) : data( size == 0 ? null : new Type[size] ),
          count( size )
      {
        aCopy( data, array, count );
      }

      /**
       * Initialise from an initialiser list.
       * @param l
       */
      DArray( initializer_list<Type> l ) : data( new Type[ l.size() ] ), count( int( l.size() ) )
      {
        aCopy( data, l.begin(), count );
      }

      /**
       * Copy from an initialiser list.
       * @param l
       * @return
       */
      DArray& operator = ( initializer_list<Type> l )
      {
        if( count != int( l.size() ) ) {
          delete[] data;
          data  = new Type[ l.size() ];
          count = int( l.size() );
        }

        aCopy( data, l.begin(), count );
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
      OZ_ALWAYS_INLINE
      CIterator citer() const
      {
        return CIterator( *this );
      }

      /**
       * @return iterator for this array
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
        hard_assert( count > 0 );

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
        hard_assert( count > 0 );

        return data;
      }

      /**
       * @return number of elements in the array
       */
      OZ_ALWAYS_INLINE
      int length() const
      {
        return count;
      }

      /**
       * @return true if memory is NOT allocated (i.e. count == 0)
       */
      OZ_ALWAYS_INLINE
      bool isEmpty() const
      {
        return count == 0;
      }

      /**
       * @param e
       * @return true if the element is found in the array
       */
      bool contains( const Type& e ) const
      {
        hard_assert( count > 0 );

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
        hard_assert( count > 0 );

        return data[0];
      }

      /**
       * @return reference to first element
       */
      OZ_ALWAYS_INLINE
      Type& first()
      {
        hard_assert( count > 0 );

        return data[0];
      }

      /**
       * @return constant reference to last element
       */
      OZ_ALWAYS_INLINE
      const Type& last() const
      {
        hard_assert( count > 0 );

        return data[count - 1];
      }

      /**
       * @return reference to last element
       */
      OZ_ALWAYS_INLINE
      Type& last()
      {
        hard_assert( count > 0 );

        return data[count - 1];
      }

      /**
       * Find the first occurrence of an element.
       * @param e
       * @return index of first occurrence, -1 if not found
       */
      int index( const Type& e ) const
      {
        hard_assert( count > 0 );

        return aIndex( data, e, count );
      }

      /**
       * Find the last occurrence of an element.
       * @param e
       * @return index of last occurrence, -1 if not found
       */
      int lastIndex( const Type& e ) const
      {
        hard_assert( count > 0 );

        return aLastIndex( data, e, count );
      }

      /**
       * Sort elements with quicksort algorithm (last element as pivot).
       */
      void sort()
      {
        hard_assert( count > 0 );

        aSort( data, count );
      }

      /**
       * Delete the array and delete all elements - take care of memory management. Use this
       * function only with array of pointers that you want to be deleted.
       */
      void free()
      {
        aFree( data, count );
      }

      /**
       * Allocates capacity for initSize elements. It analoguous to DArray( initSize ) constructor
       * if one want to reserving size on construction cannot be done.
       * DArray must be empty for this function to work.
       * @param initSize
       */
      void alloc( int initSize )
      {
        hard_assert( count == 0 && initSize > 0 );

        data = new Type[initSize];
        count = initSize;
      }

      /**
       * Deallocate resources.
       */
      void dealloc()
      {
        delete[] data;

        data = null;
        count = 0;
      }

  };

}
