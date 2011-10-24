/*
 *  Map.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * @file Map.hpp
 */

#include "arrays.hpp"

namespace oz
{

/**
 * %Map.
 *
 * %Map is implemented as a sorted vector that supports binding values to its elements (keys).
 * Better worst case performance than hashtable and it can use an arbitrary type as a key. For
 * large maps HashIndex/HashString is preferred as it is much faster on average.
 * It can also be used as a set if one omits values.
 *
 * Memory is allocated when the first element is added.
 */
template <typename Key, typename Value = nullptr_t>
class Map
{
  private:

    /// Capacity is always a multiple of <tt>GRANULARITY</tt>.
    static const int GRANULARITY = 8;

    /**
     * Internal class for key/value elements.
     */
    struct Elem
    {
      Key   key;   ///< Key.
      Value value; ///< Value.

      /**
       * Create an uninitialised instance.
       */
      OZ_ALWAYS_INLINE
      Elem()
      {}

      /**
       * Initialise a new element.
       */
      OZ_ALWAYS_INLINE
      explicit Elem( const Key& key_, const Value& value_ ) : key( key_ ), value( value_ )
      {}

      /**
       * Equality operator for bisection algorithms.
       */
      OZ_ALWAYS_INLINE
      friend bool operator == ( const Key& key, const Elem& e )
      {
        return key == e.key;
      }

      /**
       * Less than operator for bisection algorithms.
       */
      OZ_ALWAYS_INLINE
      friend bool operator < ( const Key& key, const Elem& e )
      {
        return key < e.key;
      }
    };

  public:

    /**
     * %Iterator with constant access to container elements.
     *
     * Since <tt>Elem</tt> class is private, inherited cast and operator functions are invalid.
     */
    class CIterator : public oz::CIterator<Elem>
    {
      friend class Map;

      private:

        /// Base class type, convenience definition to make code cleaner.
        typedef oz::CIterator<Elem> B;

        /**
         * %Iterator for the given container, points to its first element.
         */
        OZ_ALWAYS_INLINE
        explicit CIterator( const Map& m ) : B( m.data, m.data + m.count )
        {}

      public:

        /**
         * Default constructor, creates an invalid iterator.
         */
        OZ_ALWAYS_INLINE
        CIterator() : B( null, null )
        {}

        /**
         * Constant pointer to the current element's key.
         */
        OZ_ALWAYS_INLINE
        operator const Key* () const
        {
          return &B::elem->key;
        }

        /**
         * Constant reference to the current element's key.
         */
        OZ_ALWAYS_INLINE
        const Key& operator * () const
        {
          return B::elem->key;
        }

        /**
         * Constant access to a member of the current element's key.
         */
        OZ_ALWAYS_INLINE
        const Key* operator -> () const
        {
          return &B::elem->key;
        }

        /**
         * Constant reference to the current element's key.
         */
        OZ_ALWAYS_INLINE
        const Key& key() const
        {
          return B::elem->key;
        }

        /**
         * Constant reference to the current element's value.
         */
        OZ_ALWAYS_INLINE
        const Value& value() const
        {
          return B::elem->value;
        }

    };

    /**
     * %Iterator with non-constant access to container elements.
     *
     * Since <tt>Elem</tt> class is private, inherited cast and operator functions are invalid.
     */
    class Iterator : public oz::Iterator<Elem>
    {
      friend class Map;

      private:

        /// Base class type, convenience definition to make code cleaner.
        typedef oz::Iterator<Elem> B;

        /**
         * %Iterator for the given container, points to its first element.
         */
        OZ_ALWAYS_INLINE
        explicit Iterator( const Map& m ) : B( m.data, m.data + m.count )
        {}

      public:

        /**
         * Default constructor, creates an invalid iterator.
         */
        OZ_ALWAYS_INLINE
        Iterator() : B( null, null )
        {}

        /**
         * Constant pointer to the current element's key.
         */
        OZ_ALWAYS_INLINE
        operator const Key* () const
        {
          return &B::elem->key;
        }

        /**
         * Constant reference to the current element's value.
         */
        OZ_ALWAYS_INLINE
        const Key& operator * () const
        {
          return B::elem->key;
        }

        /**
         * Constant access to a member of the current element's key.
         */
        OZ_ALWAYS_INLINE
        const Key* operator -> () const
        {
          return &B::elem->key;
        }

        /**
         * Constant reference to the current element's key.
         */
        OZ_ALWAYS_INLINE
        const Key& key() const
        {
          return B::elem->key;
        }

        /**
         * Constant reference to the current element's value.
         */
        OZ_ALWAYS_INLINE
        const Value& value() const
        {
          return B::elem->value;
        }

        /**
         * Reference to the current element's value.
         */
        OZ_ALWAYS_INLINE
        Value& value()
        {
          return B::elem->value;
        }

    };

  private:

    Elem* data;  ///< Element storage.
    int   size;  ///< Capacity, number of elements in storage.
    int   count; ///< Number of elements.

    /**
     * Double capacity if there is not enough space to add another element.
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
     * Create an empty map.
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
     * Copy constructor, copies elements.
     */
    Map( const Map& m ) : data( m.size == 0 ? null : new Elem[m.size] ),
        size( m.size ), count( m.count )
    {}

    /**
     * Copy operator, copies elements.
     *
     * Reuse existing storage if it suffices.
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
     * Create an empty map with the given initial capacity.
     */
    explicit Map( int initSize ) : data( initSize == 0 ? null : new Elem[initSize] ),
        size( initSize ), count( 0 )
    {}

    /**
     * True iff same size or respective elements are equal.
     */
    bool operator == ( const Map& m ) const
    {
      return count == m.count && aEquals( data, m.data, count );
    }

    /**
     * False iff same size or respective elements are equal.
     */
    bool operator != ( const Map& m ) const
    {
      return count != m.count || !aEquals( data, m.data, count );
    }

    /**
     * %Iterator with constant access, initially points to the first element.
     */
    OZ_ALWAYS_INLINE
    CIterator citer() const
    {
      return CIterator( *this );
    }

    /**
     * %Iterator with non-constant access, initially points to the first element.
     */
    OZ_ALWAYS_INLINE
    Iterator iter() const
    {
      return Iterator( *this );
    }

    /**
     * Number of elements.
     */
    OZ_ALWAYS_INLINE
    int length() const
    {
      return count;
    }

    /**
     * True iff empty.
     */
    OZ_ALWAYS_INLINE
    bool isEmpty() const
    {
      return count == 0;
    }

    /**
     * Number of allocated elements.
     */
    OZ_ALWAYS_INLINE
    int capacity() const
    {
      return size;
    }

    /**
     * Constant reference to the i-th element's key.
     */
    OZ_ALWAYS_INLINE
    const Key& operator [] ( int i ) const
    {
      hard_assert( uint( i ) < uint( count ) );

      return data[i].key;
    }

    /**
     * Reference to the i-th element's key.
     */
    OZ_ALWAYS_INLINE
    Key& operator [] ( int i )
    {
      hard_assert( uint( i ) < uint( count ) );

      return data[i].key;
    }

    /**
     * Constant reference to the i-th element's value.
     */
    OZ_ALWAYS_INLINE
    const Value& value( int i ) const
    {
      hard_assert( uint( i ) < uint( count ) );

      return data[i].value;
    }

    /**
     * Reference to the i-th element's value.
     */
    OZ_ALWAYS_INLINE
    Value& value( int i )
    {
      hard_assert( uint( i ) < uint( count ) );

      return data[i].value;
    }

    /**
     * Constant reference to the first element's key.
     */
    OZ_ALWAYS_INLINE
    const Key& first() const
    {
      hard_assert( count != 0 );

      return data[0].key;
    }

    /**
     * Constant reference to the last element's key.
     */
    OZ_ALWAYS_INLINE
    const Key& last() const
    {
      hard_assert( count != 0 );

      return data[count - 1].key;
    }

    /**
     * True iff the given key is found in the map.
     */
    bool contains( const Key& key ) const
    {
      return aBisectFind( data, key, count ) != -1;
    }

    /**
     * Index of the element with the given value or -1 if not found.
     */
    int index( const Key& key ) const
    {
      return aBisectFind( data, key, count );
    }

    /**
     * Constant pointer to the given key's value or null if not found.
     */
    const Value* find( const Key& key ) const
    {
      int i = aBisectFind( data, key, count );
      return i == -1 ? null : &data[i].value;
    }

    /**
     * Pointer to the given key's value or null if not found.
     */
    Value* find( const Key& key )
    {
      int i = aBisectFind( data, key, count );
      return i == -1 ? null : &data[i].value;
    }

    /**
     * Add an element or override value if an element with the same key exists.
     *
     * @return position of the inserted or the existing element.
     */
    int add( const Key& key, const Value& value = Value() )
    {
      int i = aBisectPosition( data, key, count );

      if( i != 0 && data[i - 1].key == key ) {
        data[i - 1].value = value;
      }
      else {
        insert( i, key, value );
      }
      return i;
    }

    /**
     * Add an element if the key does not exist in the map.
     *
     * @return position of the inserted or the existing element with the same key.
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
     * Insert an element at the given position.
     *
     * All later elements are shifted to make a gap.
     * Use only when you are sure you are inserting at the right position to keep the element order.
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
     * Remove the element at the given position.
     *
     * All later elements are shifted to fill the gap.
     */
    void remove( int i )
    {
      hard_assert( uint( i ) < uint( count ) );

      --count;
      aCopy( data + i, data + i + 1, count - i );
    }

    /**
     * Find and remove the element with the given key.
     *
     * @return index of the removed element or -1 if not found.
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
     * Empty the map.
     */
    void clear()
    {
      count = 0;
    }

    /**
     * Delete all objects referenced by elements and empty the map.
     */
    void free()
    {
      aFree( data, count );
      clear();
    }

    /**
     * For an empty map with no allocated storage, allocate capacity for <tt>initSize</tt>
     * elements.
     */
    void alloc( int initSize )
    {
      hard_assert( size == 0 && initSize > 0 );

      data = new Elem[initSize];
      size = initSize;
    }

    /**
     * Deallocate storage of an empty map.
     */
    void dealloc()
    {
      hard_assert( count == 0 );

      delete[] data;

      data = null;
      size = 0;
    }

    /**
     * Trim map capacity to the least multiple of <tt>GRANULARITY</tt> that can hold the
     * elements.
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
