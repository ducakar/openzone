/*
 *  Map.hpp
 *
 *  Map, implemented as a sorted vector that supports binding values to its elements (keys).
 *  Better worst case performance than hashtable and it can use an arbitrary type as a key. For
 *  large maps HashIndex/HashString is preferred as it is much faster on average.
 *  It can also be used as a set if we omit values.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "arrays.hpp"
#include "Alloc.hpp"

namespace oz
{

  template <typename Key, typename Value = nullptr_t>
  class Map
  {
    private:

      static const int GRANULARITY = 8;

      struct Elem
      {
        Key   key;
        Value value;

        OZ_ALWAYS_INLINE
        Elem()
        {}

        OZ_ALWAYS_INLINE
        explicit Elem( const Key& key_, const Value& value_ ) : key( key_ ), value( value_ )
        {}

        // required for bisection algorithms to work properly
        OZ_ALWAYS_INLINE
        friend bool operator == ( const Key& key, const Elem& e )
        {
          return key == e.key;
        }

        OZ_ALWAYS_INLINE
        friend bool operator < ( const Key& key, const Elem& e )
        {
          return key < e.key;
        }
      };

    public:

      /**
       * Constant Map iterator.
       */
      class CIterator : public oz::CIterator<Elem>
      {
        private:

          typedef oz::CIterator<Elem> B;

        public:

          /**
           * Default constructor returns an invalid iterator
           */
          OZ_ALWAYS_INLINE
          CIterator() : B( null, null )
          {}

          /**
           * Make iterator for given vector. After creation it points to first element.
           * @param m
           */
          OZ_ALWAYS_INLINE
          explicit CIterator( const Map& m ) : B( m.data, m.data + m.count )
          {}

          /**
           * @return constant pointer to current element's value
           */
          OZ_ALWAYS_INLINE
          operator const Value* () const
          {
            return &B::elem->value;
          }

          /**
           * @return constant reference to current element's value
           */
          OZ_ALWAYS_INLINE
          const Value& operator * () const
          {
            return B::elem->value;
          }

          /**
           * @return constant access to value's member
           */
          OZ_ALWAYS_INLINE
          const Value* operator -> () const
          {
            return &B::elem->value;
          }

          /**
           * @return constant reference to current element's key
           */
          OZ_ALWAYS_INLINE
          const Key& key() const
          {
            return B::elem->key;
          }

          /**
           * @return constant reference to current element's value
           */
          OZ_ALWAYS_INLINE
          const Value& value() const
          {
            return B::elem->value;
          }

      };

      /**
       * Map iterator.
       */
      class Iterator : public oz::Iterator<Elem>
      {
        private:

          typedef oz::Iterator<Elem> B;

        public:

          /**
           * Default constructor returns an invalid iterator
           */
          OZ_ALWAYS_INLINE
          Iterator() : B( null, null )
          {}

          /**
           * Make iterator for given vector. After creation it points to first element.
           * @param m
           */
          OZ_ALWAYS_INLINE
          explicit Iterator( const Map& m ) : B( m.data, m.data + m.count )
          {}

          /**
           * @return constant pointer to current element's value
           */
          OZ_ALWAYS_INLINE
          operator const Value* () const
          {
            return &B::elem->value;
          }

          /**
           * @return pointer to current element's value
           */
          OZ_ALWAYS_INLINE
          operator Value* ()
          {
            return &B::elem->value;
          }

          /**
           * @return constant reference to current element's value
           */
          OZ_ALWAYS_INLINE
          const Value& operator * () const
          {
            return B::elem->value;
          }

          /**
           * @return reference to current element's value
           */
          OZ_ALWAYS_INLINE
          Value& operator * ()
          {
            return B::elem->value;
          }

          /**
           * @return constant access to value's member
           */
          OZ_ALWAYS_INLINE
          const Value* operator -> () const
          {
            return &B::elem->value;
          }

          /**
           * @return non-constant access to value's member
           */
          OZ_ALWAYS_INLINE
          Value* operator -> ()
          {
            return &B::elem->value;
          }

          /**
           * @return constant reference to current element's key
           */
          OZ_ALWAYS_INLINE
          const Key& key() const
          {
            return B::elem->key;
          }

          /**
           * @return constant reference to current element's value
           */
          OZ_ALWAYS_INLINE
          const Value& value() const
          {
            return B::elem->value;
          }

          /**
           * @return reference to current element's value
           */
          OZ_ALWAYS_INLINE
          Value& value()
          {
            return B::elem->value;
          }

      };

    private:

      // Pointer to data array
      Elem* data;
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
          size = size == 0 ? GRANULARITY : 2 * size;
          data = aRealloc( data, count, size );
        }
      }

    public:

      /**
       * Create empty map.
       */
      Map() : data( null ), size( 0 ), count( 0 )
      {}

      /**
       * Destructor.
       */
      ~Map()
      {
        delete[] data;
      }

      /**
       * Copy constructor.
       * @param m
       */
      Map( const Map& m ) : data( m.size == 0 ? null : new Elem[m.size] ),
          size( m.size ), count( m.count )
      {}

      /**
       * Copy operator.
       * @param m
       * @return
       */
      Map& operator = ( const Map& m )
      {
        if( &m == this ) {
          soft_assert( &m != this );
          return *this;
        }

        if( size < m.count ) {
          delete[] data;

          data = new Elem[m.size];
          size = m.size;
        }

        aCopy( data, m.data, m.count );
        count = m.count;
        return *this;
      }

      /**
       * Create empty map with given initial capacity.
       * @param initSize
       */
      explicit Map( int initSize ) : data( initSize == 0 ? null : new Elem[initSize] ),
          size( initSize ), count( 0 )
      {}

      /**
       * Equality operator. Capacity of map doesn't matter.
       * @param m
       * @return true if all elements in both vectors are equal
       */
      bool operator == ( const Map& m ) const
      {
        return count == m.count && aEquals( data, m.data, count );
      }

      /**
       * Inequality operator. Capacity of map doesn't matter.
       * @param m
       * @return false if all elements in both vectors are equal
       */
      bool operator != ( const Map& m ) const
      {
        return count != m.count || !aEquals( data, m.data, count );
      }

      /**
       * @return constant iterator for this map
       */
      OZ_ALWAYS_INLINE
      CIterator citer() const
      {
        return CIterator( *this );
      }

      /**
       * @return iterator for this map
       */
      OZ_ALWAYS_INLINE
      Iterator iter() const
      {
        return Iterator( *this );
      }

      /**
       * @return number of elements in the map
       */
      OZ_ALWAYS_INLINE
      int length() const
      {
        return count;
      }

      /**
       * @return capacity of the map
       */
      OZ_ALWAYS_INLINE
      int capacity() const
      {
        return size;
      }

      /**
       * @return true if map has no elements
       */
      OZ_ALWAYS_INLINE
      bool isEmpty() const
      {
        return count == 0;
      }

      /**
       * @param e
       * @return true if the key is found in the vector
       */
      bool contains( const Key& key ) const
      {
        return aBisectFind( data, key, count ) != -1;
      }

      /**
       * @param i
       * @return constant reference i-th element's key
       */
      OZ_ALWAYS_INLINE
      const Key& operator [] ( int i ) const
      {
        hard_assert( uint( i ) < uint( count ) );

        return data[i].key;
      }

      /**
       * @param i
       * @return reference i-th element's key
       */
      OZ_ALWAYS_INLINE
      Key& operator [] ( int i )
      {
        hard_assert( uint( i ) < uint( count ) );

        return data[i].key;
      }

      /**
       * @param i
       * @return constant reference i-th element's value
       */
      OZ_ALWAYS_INLINE
      const Value& value( int i ) const
      {
        hard_assert( uint( i ) < uint( count ) );

        return data[i].value;
      }

      /**
       * @param i
       * @return reference i-th element's value
       */
      OZ_ALWAYS_INLINE
      Value& value( int i )
      {
        hard_assert( uint( i ) < uint( count ) );

        return data[i].value;
      }

      /**
       * @return constant reference to first element's key
       */
      OZ_ALWAYS_INLINE
      const Key& first() const
      {
        hard_assert( count != 0 );

        return data[0].key;
      }

      /**
       * @return constant reference to last element's key
       */
      OZ_ALWAYS_INLINE
      const Key& last() const
      {
        hard_assert( count != 0 );

        return data[count - 1].key;
      }

      /**
       * Return index of the specified element
       * @param e
       * @return index of element, -1 if not found
       */
      int index( const Key& key ) const
      {
        return aBisectFind( data, key, count );
      }

      /**
       * If given key exists, return constant pointer to its value, otherwise return null.
       * @param key
       * @return
       */
      const Value* find( const Key& key ) const
      {
        int i = aBisectFind( data, key, count );
        return i == -1 ? null : &data[i].value;
      }

      /**
       * If given key exists, return pointer to its value, otherwise return null.
       * @param key
       * @return
       */
      Value* find( const Key& key )
      {
        int i = aBisectFind( data, key, count );
        return i == -1 ? null : &data[i].value;
      }

      /**
       * If given key exists, return constant reference to its value.
       * Only use this function if you are certain that the key exists.
       * @param key
       * @return constant reference to value associated to the given key
       */
      const Value& get( const Key& key ) const
      {
        int i = aBisectFind( data, key, count );

        hard_assert( i != -1 );

        return data[i].value;
      }

      /**
       * If given key exists, return reference to its value.
       * Only use this function if you are certain that the key exists.
       * @param key
       * @return reference to value associated to the given key
       */
      Value& get( const Key& key )
      {
        int i = aBisectFind( data, key, count );

        hard_assert( i != -1 );

        return data[i].value;
      }

      /**
       * Add an element. The key must not yet exist in this map.
       * @param e
       * @return true if element has been added
       */
      int add( const Key& key, const Value& value = Value() )
      {
        hard_assert( !contains( key ) );

        int i = aBisectPosition( data, key, count );
        insert( i, key, value );
        return i;
      }

      /**
       * Add an element, but only if there's no any equal element in the map.
       * @param e
       * @return position of the inserted element or an existing one if it was not inserted
       */
      int include( const Key& key, const Value& value = Value() )
      {
        int i = aBisectPosition( data, key, count );

        if( i == 0 || !( data[i - 1].key == key ) ) {
          insert( i, key, value );
        }
        return i;
      }

      /**
       * Insert an element at given position. All later elements are shifted to make a gap
       * for the new element.
       * @param e
       * @param i
       */
      void insert( int i, const Key& k, const Value& v = Value() )
      {
        hard_assert( uint( i ) <= uint( count ) );

        ensureCapacity();

        aReverseCopy( data + i + 1, data + i, count - i );
        data[i].key   = k;
        data[i].value = v;

        ++count;
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
       * Find and remove the given element.
       * @param e
       * @return
       */
      int exclude( const Key& key )
      {
        int i = aBisectFind( data, key, count );

        if( i != -1 ) {
          remove( i );
        }
        return i;
      }

      /**
       * Empty the map but don't delete the elements.
       */
      void clear()
      {
        count = 0;
      }

      /**
       * Empty the vector and delete all elements - take care of memory management. Use this
       * function only with vector of pointers that you want to be deleted.
       */
      void free()
      {
        for( int i = 0; i < count; ++i ) {
          delete data[i].value;
          data[i].value = null;
        }
        clear();
      }

      /**
       * Allocates capacity for initSize elements. It analoguous to Map( initSize ) constructor
       * if one want to reserving size on construction cannot be done.
       * Map must be empty for this function to work.
       * @param initSize
       */
      void alloc( int initSize )
      {
        hard_assert( size == 0 && initSize > 0 );

        data = new Elem[initSize];
        size = initSize;
      }

      /**
       * Deallocate resources.
       * Container must be empty for this function to work.
       */
      void dealloc()
      {
        hard_assert( count == 0 );

        delete[] data;

        data = null;
        size = 0;
      }

      /**
       * Trim map, leave at most <code>left</code> elements/capacity.
       * @param left
       */
      void trim()
      {
        int newSize = ( ( count - 1 ) / GRANULARITY + 1 ) * GRANULARITY;

        if( newSize < size ) {
          size = newSize;
          data = aRealloc( data, count, size );
        }
      }

  };

}
