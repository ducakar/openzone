/*
 *  Map.hpp
 *
 *  Map, implemented as a sorted vector that supports binding values to its elements (keys).
 *  Better worst case performance than hashtable and it can use an arbitrary type as a key. For
 *  large maps HashIndex/HashString is preferred as it is much faster on average.
 *  It can also be used as a set if we omit values.
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

namespace oz
{

  template <typename Key, typename Value = nil, int GRANULARITY = 8>
  class Map
  {
    static_assert( GRANULARITY > 0, "GRANULARITY must be at least 1" );

    private:

      struct Elem
      {
        Key   key;
        Value value;

        explicit Elem()
        {}

        explicit Elem( const Key& key_, const Value& value_ ) :
            key( key_ ), value( value_ )
        {}

        // operators overloads are required for bisection algorithms to work properly
        friend bool operator == ( const Key& key, const Elem& e )
        {
          return key == e.key;
        }

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
          explicit CIterator() : B( null, null )
          {}

          /**
           * Make iterator for given vector. After creation it points to first element.
           * @param m
           */
          explicit CIterator( const Map& m ) : B( m.data, m.data + m.count )
          {}

          /**
           * @return constant pointer to current element's value
           */
          operator const Value* () const
          {
            return &B::elem->value;
          }

          /**
           * @return constant reference to current element's value
           */
          const Value& operator * () const
          {
            return B::elem->value;
          }

          /**
           * @return constant access to value's member
           */
          const Value* operator -> () const
          {
            return &B::elem->value;
          }

          /**
           * @return constant reference to current element's key
           */
          const Key& key() const
          {
            return B::elem->key;
          }

          /**
           * @return constant reference to current element's value
           */
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
          explicit Iterator() : B( null, null )
          {}

          /**
           * Make iterator for given vector. After creation it points to first element.
           * @param m
           */
          explicit Iterator( const Map& m ) : B( m.data, m.data + m.count )
          {}

          /**
           * @return constant pointer to current element's value
           */
          operator const Value* () const
          {
            return &B::elem->value;
          }

          /**
           * @return pointer to current element's value
           */
          operator Value* ()
          {
            return &B::elem->value;
          }

          /**
           * @return constant reference to current element's value
           */
          const Value& operator * () const
          {
            return B::elem->value;
          }

          /**
           * @return reference to current element's value
           */
          Value& operator * ()
          {
            return B::elem->value;
          }

          /**
           * @return constant access to value's member
           */
          const Value* operator -> () const
          {
            return &B::elem->value;
          }

          /**
           * @return non-constant access to value's member
           */
          Value* operator -> ()
          {
            return &B::elem->value;
          }

          /**
           * @return constant reference to current element's key
           */
          const Key& key() const
          {
            return B::elem->key;
          }

          /**
           * @return constant reference to current element's value
           */
          const Value& value() const
          {
            return B::elem->value;
          }

          /**
           * @return reference to current element's value
           */
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
          if( size == 0 ) {
            size = GRANULARITY;
            data = Alloc::alloc<Elem>( size );
          }
          else {
            size *= 2;
            data = Alloc::realloc( data, count, size );
          }
        }
      }

    public:

      /**
       * Create empty map with initial capacity 8.
       */
      explicit Map() : data( null ), size( 0 ), count( 0 )
      {}

      /**
       * Create empty map with given initial capacity.
       * @param initSize
       */
      explicit Map( int initSize ) : data( Alloc::alloc<Elem>( initSize ) ), size( initSize ),
          count( 0 )
      {}

      /**
       * Copy constructor.
       * @param m
       */
      Map( const Map& m ) : data( Alloc::alloc<Elem>( m.size ) ), size( m.size ), count( m.count )
      {
        assert( m.size > 0 );

        aConstruct( data, m.data, m.count );
      }

      /**
       * Destructor.
       */
      ~Map()
      {
        aDestruct( data, count );
        Alloc::dealloc( data );
      }

      /**
       * Copy operator.
       * @param m
       * @return
       */
      Map& operator = ( const Map& m )
      {
        assert( &m != this );
        assert( m.size > 0 );

        aDestruct( data, count );

        if( size < m.count ) {
          Alloc::dealloc( data );
          data = Alloc::alloc<Elem>( m.size );
          size = m.size;
        }

        aConstruct( data, m.data, m.count );
        count = m.count;
        return *this;
      }

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
      CIterator citer() const
      {
        return CIterator( *this );
      }

      /**
       * @return iterator for this map
       */
      Iterator iter() const
      {
        return Iterator( *this );
      }

      /**
       * @return number of elements in the map
       */
      int length() const
      {
        return count;
      }

      /**
       * @return capacity of the map
       */
      int capacity() const
      {
        return size;
      }

      /**
       * @return true if map has no elements
       */
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
      const Key& operator [] ( int i ) const
      {
        assert( 0 <= i && i < count );

        return data[i].key;
      }

      /**
       * @param i
       * @return reference i-th element's key
       */
      Key& operator [] ( int i )
      {
        assert( 0 <= i && i < count );

        return data[i].key;
      }

      /**
       * @param i
       * @return constant reference i-th element's value
       */
      const Value& value( int i ) const
      {
        assert( 0 <= i && i < count );

        return data[i].value;
      }

      /**
       * @param i
       * @return reference i-th element's value
       */
      Value& value( int i )
      {
        assert( 0 <= i && i < count );

        return data[i].value;
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
       * @return constant reference to first element's key
       */
      const Key& first() const
      {
        assert( count != 0 );

        return data[0].key;
      }

      /**
       * @return constant reference to last element's key
       */
      const Key& last() const
      {
        assert( count != 0 );

        return data[count - 1].key;
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

        assert( i != -1 );

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

        assert( i != -1 );

        return data[i].value;
      }

      /**
       * Add an element. The key must not yet exist in this map.
       * @param e
       * @return true if element has been added
       */
      int add( const Key& key, const Value& value = Value() )
      {
        assert( !contains( key ) );

        int i = aBisectPosition( data, key, count );
        insert( i, key, value );
        return i;
      }

      /**
       * Add an element, but only if there's no any equal element in the map.
       * @param e
       * @return true if element has been added
       */
      int include( const Key& key, const Value& value = Value() )
      {
        int i = aBisectPosition( data, key, count );

        if( i == 0 || !( data[i - 1].key == key ) ) {
          insert( i, key, value );
          return i;
        }
        return -1;
      }

      /**
       * Insert an element at given position. All later elements are shifted to make a gap
       * for the new element.
       * @param e
       * @param index
       */
      void insert( int index, const Key& k, const Value& v = Value() )
      {
        assert( 0 <= index && index <= count );

        ensureCapacity();

        if( index == count ) {
          new( data + count ) Elem( k, v );
        }
        else {
          new( data + count ) Elem( data[count - 1] );
          aReverseCopy( data + index + 1, data + index, count - index - 1 );
          data[index] = Elem( k, v );
        }
        ++count;
      }

      /**
       * Remove the element at given position. All later element are shifted to fill the gap.
       * @param index
       */
      void remove( int index )
      {
        assert( 0 <= index && index < count );

        --count;
        aCopy( data + index, data + index + 1, count - index );
        data[count].~Type();
      }

      /**
       * Find and remove the given element.
       * @param e
       * @return
       */
      bool exclude( const Key& key )
      {
        int i = aBisectFind( data, key, count );

        if( i != -1 ) {
          remove( i );
          return true;
        }
        else {
          return false;
        }
      }

      /**
       * Empty the map but don't delete the elements.
       */
      void clear()
      {
        aDestruct( data, count );
        Alloc::dealloc( data );

        data = null;
        size = 0;
        count = 0;
      }

      /**
       * Empty the vector and delete all elements - take care of memory management. Use this
       * function only with vector of pointers that you want to be deleted.
       */
      void free()
      {
        aFree( data, count );
        clear();
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
          data = Alloc::realloc( data, count, size );
        }
      }

  };

}
