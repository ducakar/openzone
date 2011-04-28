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
      union
      {
        char cdata[ SIZE * sizeof( Type ) ];
        Type data[SIZE];
      };
      // Number of elements in vector
      int  count;

    public:

      /**
       * Create empty vector with capacity SIZE.
       */
      SVector() : count( 0 )
      {}

      /**
       * Destructor.
       */
      ~SVector()
      {
        aDestruct( data, count );
      }

      /**
       * Copy constructor.
       * @param v
       */
      SVector( const SVector& v ) : count( v.count )
      {
        aConstruct( data, v.data, v.count );
      }

      /**
       * Move constructor.
       * @param v
       */
      SVector( SVector&& v ) : count( v.count )
      {
        aReconstruct( data, v.data, v.count );
        aDestruct( v.data, v.count );
        v.count = 0;
      }

      /**
       * Copy operator.
       * @param v
       * @return
       */
      SVector& operator = ( const SVector& v )
      {
        if( &v == this ) {
          soft_assert( &v != this );
          return *this;
        }

        aCopy( data, v.data, v.count );
        count = v.count;
        return *this;
      }

      /**
       * Move operator.
       * @param v
       * @return
       */
      SVector& operator = ( SVector&& v )
      {
        if( &v == this ) {
          soft_assert( &v != this );
          return *this;
        }

        aMove( data, v.data, v.count );
        aDestruct( v.data, v.count );
        count   = v.count;
        v.count = 0;

        return *this;
      }

      /**
       * Initialise from an initialiser list.
       * @param l
       */
      SVector( initializer_list<Type> l ) : count( int( l.size() ) )
      {
        hard_assert( count <= SIZE );

        aConstruct( data, l.begin(), count );
      }

      /**
       * Copy from an initialiser list.
       * @param l
       * @return
       */
      SVector& operator = ( initializer_list<Type> l )
      {
        hard_assert( int( l.size() ) <= SIZE );

        aDestruct( data, count );
        count = int( l.size() );

        aConstruct( data, l.begin(), count );
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

        new( data + count ) Type;
        ++count;
      }

      /**
       * Add an element to the end.
       * @param e
       */
      template <typename Value>
      void add( Value&& e )
      {
        hard_assert( uint( count ) < uint( SIZE ) );

        new( data + count ) Type( static_cast<Value&&>( e ) );
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
      template <typename Value>
      int include( Value&& e )
      {
        int i = aIndex( data, e, count );

        if( i == -1 ) {
          hard_assert( uint( count ) < uint( SIZE ) );

          new( data + count ) Type( static_cast<Value&&>( e ) );
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
      template <typename Value>
      void insert( int i, Value&& e )
      {
        hard_assert( uint( i ) <= uint( count ) );
        hard_assert( uint( count ) < uint( SIZE ) );

        if( i == count ) {
          new( data + count ) Type( static_cast<Value&&>( e ) );
        }
        else {
          new( data + count ) Type( static_cast<Type&&>( data[count - 1] ) );
          aReverseMove( data + i + 1, data + i, count - i - 1 );
          data[i] = static_cast<Value&&>( e );
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
        data[count].~Type();
      }

      /**
       * Remove the element at given position. All later element are shifted to fill the gap.
       * @param i
       */
      void remove( int i )
      {
        hard_assert( uint( i ) < uint( count ) );

        --count;
        aMove( data + i, data + i + 1, count - i );
        data[count].~Type();
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
        if( i != count ) {
          data[i] = static_cast<Type&&>( data[count] );
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
          aMove( data + i, data + i + 1, count - i );
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
            data[i] = static_cast<Type&&>( data[count] );
          }
          data[count].~Type();
        }
        return i;
      }

      /**
       * Add an element to the beginning.
       * @param e
       */
      template <typename Value>
      void pushFirst( Value&& e )
      {
        hard_assert( uint( count ) < uint( SIZE ) );

        if( count == 0 ) {
          new( data + 0 ) Type( static_cast<Value&&>( e ) );
          ++count;
        }
        else {
          new( data + count ) Type( static_cast<Type&&>( data[count - 1] ) );
          aReverseMove( data + 1, data, count - 1 );
          data[0] = static_cast<Value&&>( e );
          ++count;
        }
      }

      /**
       * Add an element to the end.
       * @param e
       */
      template <typename Value>
      void pushLast( Value&& e )
      {
        hard_assert( uint( count ) < uint( SIZE ) );

        new( data + count ) Type( static_cast<Value&&>( e ) );
        ++count;
      }

      /**
       * Remove first element
       * @return value of removed element
       */
      Type popFirst()
      {
        Type e = static_cast<Type&&>( data[0] );

        --count;
        aMove( data, data + 1, count );
        data[count].~Type();

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
        Type e = static_cast<Type&&>( data[count] );
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
