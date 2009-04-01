/*
 *  Vector.h
 *
 *  Vector
 *  It can be also used as stack or small set.
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  template <class Type>
  class Vector
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
          explicit Iterator( Vector &v ) : B( v.data, v.data + v.count )
          {}

      };

    private:

      // Pointer to data array
      Type *data;
      // Size of data array
      int  size;
      // Number of elements in the vector
      int  count;

      /**
       * Enlarge capacity by two times if there's not enough space to add another element.
       */
      void ensureCapacity()
      {
        if( size == count ) {
          size *= 2;
          assert( size <= 1024*1024*10 );
          data = aRealloc( data, count, size );
        }
      }

    public:

      /**
       * Create empty vector with initial capacity 8.
       */
      explicit Vector() : data( new Type[8] ), size( 8 ), count( 0 )
      {}

      /**
       * Copy constructor.
       * @param v
       */
      Vector( const Vector &v ) : data( new Type[v.size] ), size( v.size ), count( v.count )
      {
        aCopy( data, v.data, count );
      }

      /**
       * Create empty vector with given initial capacity.
       * @param initSize
       */
      explicit Vector( int initSize ) : data( new Type[initSize] ), size( initSize ), count( 0 )
      {}

      /**
       * Destructor.
       */
      ~Vector()
      {
        delete[] data;
      }

      /**
       * Copy operator.
       * @param v
       * @return
       */
      Vector &operator = ( const Vector &v )
      {
        // create new data array of the new data doesn't fit, keep the old one otherwise
        if( size < v.count ) {
          delete[] data;
          data = new Type[v.size];
        }
        count = v.count;
        aCopy( data, v.data, count );
        return *this;
      }

      /**
       * Equality operator. Capacity of vectors doesn't matter.
       * @param v
       * @return true if all elements in both vectors are equal
       */
      bool operator == ( const Vector &v ) const
      {
        return count == v.count && aEqual( data, v.data, count );
      }

      /**
       * Inequality operator. Capacity of vectors doesn't matter.
       * @param v
       * @return false if all elements in both vectors are equal
       */
      bool operator != ( const Vector &v ) const
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
        return size;
      }

      /**
       * @return true if vector has no elements
       */
      bool isEmpty() const
      {
        return count == 0;
      }

      /**
       * Trim vector, leave at most <code>left</code> elements/capacity.
       * @param left
       */
      void trim( int left )
      {
        int newCapacity = count + left;

        if( newCapacity < size ) {
          size = newCapacity;
          data = aRealloc( data, count, size );
        }
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
       * Add an element to the end.
       * @param e
       */
      void operator << ( const Type &e )
      {
        pushLast( e );
      }

      /**
       * Create slot for a new element at the end.
       */
      void add()
      {
        ensureCapacity();
        count++;
      }

      /**
       * Add an element to the end.
       * @param e
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
        ensureCapacity();
        data[count] = e;
        count++;
      }

      /**
       * Add all elements from a vector to the end.
       * @param v
       */
      void addAll( const Vector &v )
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

        if( size < newCount ) {
          size = max( size * 2, newCount );
          data = aRealloc( data, count, size );
        }
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
          pushLast( e );
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
      int includeAll( const Vector &v )
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
      void insert( const Type &e, int index )
      {
        assert( 0 <= index && index < count );

        ensureCapacity();
        aRCopy( data + index + 1, data + index, count - index );
        data[index] = e;
        count++;
      }

      /**
       * Remove last element.
       * @param
       * @return
       */
      Vector &operator -- ()
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
        int index = aIndex( data, count, e );

        if( index != -1 ) {
          remove( index );

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
      int excludeAll( const Vector &v )
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

      /**
       * Empty the vector but don't delete the elements.
       */
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
