/*
 *  SVector.h
 *
 *  Static size vector
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  template <class Type, int SIZE>
  class SVector
  {
    public:

      /**
       * Vector iterator.
       */
      class Iterator : public oz::Iterator<Type>
      {
        private:

          // base class
          typedef oz::Iterator<Type> B;

        public:

          /**
           * Make iterator for given vector. After creation it points to first element.
           * @param v
           */
          explicit Iterator( SVector &v ) : B( v.data, v.data + v.count )
          {}

      };

    private:

      // Pointer to data array
      Type data[SIZE];
      // Number of elements in vector
      int count;

    public:

      /**
       * Create empty vector with capacity SIZE.
       */
      SVector() : count( 0 )
      {}

      /**
       * Copy constructor.
       * @param v
       */
      SVector( const SVector &v ) : count( v.count )
      {
        aCopy( data, v.data, count );
      }

      /**
       * Copy operator.
       * @param v
       * @return
       */
      SVector &operator = ( const SVector &v )
      {
        count = v.count;
        aCopy( data, v.data, count );
        return *this;
      }

      /**
       * Equality operator.
       * @param v
       * @return true if all elements in both vectors are equal
       */
      bool operator == ( const SVector &v ) const
      {
        return count == v.count && aEqual( data, v.data, count );
      }

      /**
       * Inequality operator.
       * @param v
       * @return false if all elements in both vectors are equal
       */
      bool operator != ( const SVector &v ) const
      {
        return count != v.count || !aEqual( data, v.data, count );
      }

      /**
       * @return iterator for this vector
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
      Type *dataPtr()
      {
        return data;
      }

      /**
       * Get pointer to <code>data</code> array. Use with caution, since you can easily make buffer
       * overflows if you don't check the size of <code>data</code> array.
       * @return constant pointer to data array
       */
      const Type *dataPtr() const
      {
        return data;
      }

      /**
       * @return number of elements in the vector
       */
      int length() const
      {
        return count;
      }

      /**
       * @return capacity of the vector
       */
      int capacity() const
      {
        return SIZE;
      }

      /**
       * @return true if vector has no elements
       */
      bool isEmpty() const
      {
        return count == 0;
      }

      /**
       * @param e
       * @return true if the element is found in the vector
       */
      bool contains( const Type &e )
      {
        for( int i = 0; i < count; i++ ) {
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
      Type &operator [] ( int i )
      {
        assert( 0 <= i && i < count );

        return data[i];
      }

      /**
       * @param i
       * @return constant reference i-th element
       */
      const Type &operator [] ( int i ) const
      {
        assert( 0 <= i && i < count );

        return data[i];
      }

      /**
       * Find the first occurence of an element.
       * @param e
       * @return index of first occurence, -1 if not found
       */
      int index( const Type &e ) const
      {
        return aIndex( data, count, e );
      }

      /**
       * Find the last occurence of an element.
       * @param e
       * @return index of last occurence, -1 if not found
       */
      int lastIndex( const Type &e ) const
      {
        return aLastIndex( data, count, e );
      }

      /**
       * @return reference to first element
       */
      Type &first()
      {
        assert( count != 0 );

        return data[0];
      }

      /**
       * @return constant reference to first element
       */
      const Type &first() const
      {
        assert( count != 0 );

        return data[0];
      }

      /**
       * @return reference to last element
       */
      Type &last()
      {
        assert( count != 0 );

        return data[count - 1];
      }

      /**
       * @return constant reference to last element
       */
      const Type &last() const
      {
        assert( count != 0 );

        return data[count - 1];
      }

      /**
       * Create slot for a new element at the end.
       */
      void add()
      {
        assert( count < SIZE );

        count++;
      }

      /**
       * Add an element to the end.
       * @param e
       */
      void operator << ( const Type &e )
      {
        pushLast( e );
      }

      /**
       * Add an element to the end.
       * @param e element to be added
       */
      void add( const Type &e )
      {
        pushLast( e );
      }

      /**
       * Add an element to the beginning.
       * @param e
       */
      void pushFirst( const Type &e )
      {
        return insert( e, 0 );
      }

      /**
       * Add an element to the end.
       * @param e
       */
      void pushLast( const Type &e )
      {
        assert( count < SIZE );

        data[count] = e;
        count++;
      }

      /**
       * Add all elements from a vector to the end.
       * @param v
       */
      void addAll( const SVector &v )
      {
        addAll( v.data, v.count );
      }

      /**
       * Add all elements from an array to the end.
       * @param array
       * @param arrayCount
       */
      void addAll( const Type *array, int arrayCount )
      {
        int newCount = count + arrayCount;

        assert( SIZE >= newCount );

        for( int i = 0; i < arrayCount; i++ ) {
          data[count + i] = array[i];
        }
        count = newCount;
      }

      /**
       * Add an element to the end, but only if there's no any equal element in the vector.
       * This function is useful if you plan to use vector as a set.
       * @param e
       * @return true if element has been added
       */
      bool include( const Type &e )
      {
        if( !contains( e ) ) {
          add( e );
          return true;
        }
        else {
          return false;
        }
      }

      /**
       * Add all elements from given vector which are not yet included in this vector.
       * @param v
       * @return number of elements that have been added
       */
      int includeAll( const SVector &v )
      {
        return includeAll( v.data, v.count );
      }

      /**
       * Add all elements from given array which are not yet included in this vector.
       * @param array
       * @param count
       * @return number of elements that have been added
       */
      int includeAll( const Type *array, int count )
      {
        int n = 0;
        for( int i = 0; i < count; i++ ) {
          n += (int) include( array[i] );
        }
        return n;
      }

      /**
       * Insert an element at given position. All later elements are shifted to make a gap
       * for the new element.
       * @param e
       * @param index
       */
      void insert( int index, const Type &e )
      {
        assert( 0 <= index && index <= count );
        assert( count < SIZE );

        aRCopy( data + index + 1, data + index, count - index );
        data[index] = e;
        count++;
      }

      /**
       * Remove last element.
       * @param
       * @return
       */
      SVector &operator -- ( int )
      {
        assert( count != 0 );

        count--;
        return *this;
      }

      /**
       * Remove the element at given position. All later element are shifted to fill the gap.
       * @param index
       */
      void remove( int index )
      {
        assert( 0 <= index && index < count );

        count--;
        aCopy( data + index, data + index + 1, count - index );
      }

      /**
       * Find and remove the given element.
       * @param e
       * @return
       */
      bool exclude( const Type &e )
      {
        int i = aIndex( data, count, e );

        if( i != -1 ) {
          count--;
          aCopy( data + i, data + i + 1, count - i );

          return true;
        }
        else {
          return false;
        }
      }

      /**
       * Remove intersection of vectors from this vector.
       * @param v
       * @return
       */
      int excludeAll( const SVector &v )
      {
        return excludeAll( v.data, v.count );
      }

      /**
       * Remove intersection of this vector and given array from this vector.
       * @param v
       * @return
       */
      int excludeAll( const Type *array, int count )
      {
        int n = 0;
        for( int i = 0; i < count; i++ ) {
          n += (int) exclude( array[i] );
        }
        return n;
      }

      /**
       * Remove first element
       * @return value of removed element
       */
      Type popFirst()
      {
        Type e = data[0];

        count--;
        aCopy( data, data + 1, count );

        return e;
      }

      /**
       * Remove last element
       * @param e reference to variable to be overwritten with value of removed element
       */
      void operator >> ( Type &e )
      {
        e = popLast();
      }

      /**
       * Remove last element
       * @return value of removed element
       */
      Type popLast()
      {
        assert( count != 0 );

        count--;

        return data[count];
      }

      /**
       * Sort elements with quicksort algorithm (last element as pivot).
       */
      void sort()
      {
        aSort( data, count );
      }

      // remove all elements
      void clear()
      {
        count = 0;
      }

      /**
       * Empty the list and delete all elements - take care of memory managment. Use this function
       * only with vector of pointer that you want to be deleted.
       */
      void free()
      {
        for( int i = 0; i < count; i++ ) {
          delete data[i];
        }
        clear();
      }

  };

}
