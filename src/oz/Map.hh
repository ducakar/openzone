/*
 *  Map.hh
 *
 *  Map, implemented as sorted vector. Better worst case performance than hashtable and it can use
 *  arbitrary type as a key. For large maps HashIndex/HashString is preferred as it is much faster
 *  on average.
 *  It can also be used as a set if we omit values.
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

namespace oz
{

  template <typename Key, typename Value = nil, int GRANULARITY = 8>
  class Map
  {
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
           * Default constructor returns a dummy passed iterator
           * @return
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
           * @return current element's key
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
           * @return constant pointer to current element
           */
          operator const Value* () const
          {
            return &B::elem->value;
          }

          /**
           * @return constant reference to current element
           */
          const Value& operator * () const
          {
            return B::elem->value;
          }

          /**
           * @return constant access to member
           */
          const Value* operator -> () const
          {
            return &B::elem->value;
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
           * Default constructor returns a dummy passed iterator
           * @return
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
           * @return current element's key
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

          /**
           * @return constant pointer to current element
           */
          operator const Value* () const
          {
            return &B::elem->value;
          }

          /**
           * @return pointer to current element
           */
          operator Value* ()
          {
            return &B::elem->value;
          }

          /**
           * @return constant reference to current element
           */
          const Value& operator * () const
          {
            return B::elem->value;
          }

          /**
           * @return reference to current element
           */
          Value& operator * ()
          {
            return B::elem->value;
          }

          /**
           * @return constant access to member
           */
          const Value* operator -> () const
          {
            return &B::elem->value;
          }

          /**
           * @return non-constant access to member
           */
          Value* operator -> ()
          {
            return &B::elem->value;
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
          size *= 2;
          data = Alloc::realloc( data, count, size );
        }
      }

      int bisectFind( const Key& key ) const
      {
        if( count == 0 ) {
          return -1;
        }

        int a = 0;
        int b = count;

        // Note that the algorithm ensures data[a] <= key and key < data[b] all the time, so the
        // key may only lie on position a.
        do {
          int c = ( a + b ) / 2;

          if( data[c].key == key ) {
            return c;
          }
          else if( key < data[c].key ) {
            b = c;
          }
          else {
            a = c;
          }
        }
        while( b - a > 1 );

        return data[a].key == key ? a : -1;
      }

      int bisectPosition( const Key& key ) const
      {
        if( count == 0 ) {
          return 0;
        }
        // the algorithm cannot put elements before data[1]
        if( key < data[0].key ) {
          return 0;
        }

        int a = 0;
        int b = count;

        // Note that the algorithm ensures data[a] <= key and key < data[b] all the time, so the
        // key may only lie on position a.
        do {
          int c = ( a + b ) / 2;

          if( data[c].key == key ) {
            return -1;
          }
          else if( key < data[c].key ) {
            b = c;
          }
          else {
            a = c;
          }
        }
        while( b - a > 1 );

        return b;
      }

    public:

      /**
       * Create empty map with initial capacity 8.
       */
      explicit Map() : data( Alloc::alloc<Elem>( 8 ) ), size( 8 ), count( 0 )
      {}

      /**
       * Create empty map with given initial capacity.
       * @param initSize
       */
      explicit Map( int initSize ) : data( Alloc::alloc<Elem>( initSize ) ),
          size( initSize ), count( 0 )
      {}

      /**
       * Copy constructor.
       * @param m
       */
      Map( const Map& m ) : data( Alloc::alloc<Elem>( m.size ) ),
          size( m.size ), count( m.count )
      {
        aConstruct( data, m.data, count );
      }

      /**
       * Move constructor.
       * @param m
       */
      Map( Map&& m ) : data( m.data ), size( m.size ), count( m.count )
      {
        m.data  = null;
        m.size  = 0;
        m.count = 0;
      }

      /**
       * Destructor.
       */
      ~Map()
      {
        if( size != 0 ) {
          aDestruct( data, count );
          Alloc::dealloc( data );
        }
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
        // create new data array of the new data doesn't fit, keep the old one otherwise
        if( size < m.count || size == 0 ) {
          if( size != 0 ) {
            Alloc::dealloc( data );
          }
          data = Alloc::alloc<Elem>( m.size );
          size = m.size;
        }
        aConstruct( data, m.data, m.count );
        count = m.count;
        return *this;
      }

      /**
       * Move operator.
       * @param m
       * @return
       */
      Map& operator = ( Map&& m )
      {
        assert( &m != this );
        assert( m.size > 0 );

        if( size != 0 ) {
          aDestruct( data, count );
          Alloc::dealloc( data );
        }
        data    = m.data;
        size    = m.size;
        count   = m.count;

        m.data  = null;
        m.size  = 0;
        m.count = 0;
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
       * @return iterator for this map
       */
      Iterator iter() const
      {
        return Iterator( *this );
      }

      /**
       * @return constant iterator for this map
       */
      CIterator citer() const
      {
        return CIterator( *this );
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

      /**
       * @param e
       * @return true if the key is found in the vector
       */
      bool contains( const Key& key ) const
      {
        return bisectFind( key ) != -1;
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
       * @return constant reference i-th element's key
       */
      const Key& operator [] ( int i ) const
      {
        assert( 0 <= i && i < count );

        return data[i].key;
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
       * @param i
       * @return constant reference i-th element's value
       */
      const Value& value( int i ) const
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
        return bisectFind( key );
      }

      /**
       * @return constant reference to first element
       */
      const Value& first() const
      {
        assert( count != 0 );

        return data[0];
      }

      /**
       * @return reference to first element
       */
      Value& first()
      {
        assert( count != 0 );

        return data[0];
      }

      /**
       * @return constant reference to last element
       */
      const Value& last() const
      {
        assert( count != 0 );

        return data[count - 1];
      }

      /**
       * @return reference to last element
       */
      Value& last()
      {
        assert( count != 0 );

        return data[count - 1];
      }

      /**
       * @param i
       * @return pointer i-th giver key's value
       */
      Value* get( const Key& key )
      {
        int i = index( key );
        return i == -1 ? null : data[i].value;
      }

      /**
       * @param i
       * @return constant pointer i-th giver key's value
       */
      const Value* get( const Key& key ) const
      {
        int i = index( key );
        return i == -1 ? null : data[i].value;
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
       * Add an element, but only if there's no any equal element in the map.
       * @param e
       * @return true if element has been added
       */
      void include( const Key& key, const Value& value = Value() )
      {
        int i = bisectPosition( key );
        if( i != -1 ) {
          insert( i, key, value );
        }
      }

      /**
       * Add all elements from given container which are not yet included in this map.
       * @param c
       */
      template <class Container>
      void includeAll( const Container& c )
      {
        foreach( e, c.citer() ) {
          include( *e );
        }
      }

      /**
       * Find and remove the given element.
       * @param e
       * @return
       */
      bool exclude( const Key& key )
      {
        int i = index( key );
        if( i != -1 ) {
          remove( i );
          return true;
        }
        else {
          return false;
        }
      }

      /**
       * Remove intersection from this map.
       * @param c
       */
      template <class Container>
      void excludeAll( const Container& c )
      {
        foreach( e, c.citer() ) {
          exclude( *e );
        }
      }

      /**
       * Empty the map but don't delete the elements.
       */
      void clear()
      {
        aDestruct( data, count );
        count = 0;
      }

      /**
       * Empty the vector and delete all elements - take care of memory management. Use this function
       * only with vector of pointers that you want to be deleted.
       */
      void free()
      {
        aFree( data, count );
        clear();
      }

  };

}
