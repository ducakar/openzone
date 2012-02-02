/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file oz/Map.hh
 */

#pragma once

#include "arrays.hh"

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
 *
 * @ingroup oz
 */
template <typename Key, typename Value = nullptr_t>
class Map
{
  private:

    /// Granularity for automatic capacity allocations.
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
      Elem() = default;

      /**
       * Initialise a new element.
       */
      template <typename Key_, typename Value_>
      OZ_ALWAYS_INLINE
      explicit Elem( Key_&& key_, Value_&& value_ ) :
        key( static_cast<Key_&&>( key_ ) ), value( static_cast<Value_&&>( value_ ) )
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
     */
    class CIterator : public oz::CIterator<Elem>
    {
      friend class Map;

      OZ_RANGE_ITERATOR( CIterator )

      private:

        /// Base class type, convenience definition to make code cleaner.
        typedef oz::CIterator<Elem> B;

        /**
         * %Iterator for the given container, points to its first element.
         */
        OZ_ALWAYS_INLINE
        explicit CIterator( const Map& m ) :
          B( m.data, m.data + m.count )
        {}

      public:

        /**
         * Default constructor, creates an invalid iterator.
         */
        OZ_ALWAYS_INLINE
        CIterator() :
          B( null, null )
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
     */
    class Iterator : public oz::Iterator<Elem>
    {
      friend class Map;

      OZ_RANGE_ITERATOR( Iterator )

      private:

        /// Base class type, convenience definition to make code cleaner.
        typedef oz::Iterator<Elem> B;

        /**
         * %Iterator for the given container, points to its first element.
         */
        OZ_ALWAYS_INLINE
        explicit Iterator( const Map& m ) :
          B( m.data, m.data + m.count )
        {}

      public:

        /**
         * Default constructor, creates an invalid iterator.
         */
        OZ_ALWAYS_INLINE
        Iterator() :
          B( null, null )
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
    Map() :
      data( null ), size( 0 ), count( 0 )
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
    Map( const Map& m ) :
      data( m.size == 0 ? null : new Elem[m.size] ), size( m.size ), count( m.count )
    {
      aCopy( data, m.data, m.count );
    }

    /**
     * Move constructor, moves element storage.
     */
    Map( Map&& m ) :
      data( m.data ), size( m.size ), count( m.count )
    {
      m.data  = null;
      m.size  = 0;
      m.count = 0;
    }

    /**
     * Copy operator, copies elements.
     *
     * Reuse existing storage if it suffices.
     */
    Map& operator = ( const Map& m )
    {
      if( &m == this ) {
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
     * Move operator, moves element storage.
     */
    Map& operator = ( Map&& m )
    {
      if( &m == this ) {
        return *this;
      }

      delete[] data;

      data  = m.data;
      size  = m.size;
      count = m.count;

      m.data  = null;
      m.size  = 0;
      m.count = 0;

      return *this;
    }

    /**
     * Create an empty map with the given initial capacity.
     */
    explicit Map( int size_ ) :
      data( new Elem[size_] ), size( size_ ), count( 0 )
    {}

    /**
     * True iff respective elements are equal.
     */
    bool operator == ( const Map& m ) const
    {
      return count == m.count && aEquals( data, m.data, count );
    }

    /**
     * False iff respective elements are equal.
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
     * Constant pointer to the given key's value or <tt>null</tt> if not found.
     */
    const Value* find( const Key& key ) const
    {
      int i = aBisectFind( data, key, count );
      return i == -1 ? null : &data[i].value;
    }

    /**
     * Pointer to the given key's value or <tt>null</tt> if not found.
     */
    Value* find( const Key& key )
    {
      int i = aBisectFind( data, key, count );
      return i == -1 ? null : &data[i].value;
    }

    /**
     * Add an element or override value if an element with the same key exists.
     *
     * @return Position of the inserted or the existing element.
     */
    template <typename Key_, typename Value_ = Value>
    int add( Key_&& key, Value_&& value = Value() )
    {
      int i = aBisectPosition( data, key, count );

      if( i != 0 && data[i - 1].key == key ) {
        data[i - 1].value = static_cast<Value_&&>( value );
      }
      else {
        insert( i, static_cast<Key_&&>( key ), static_cast<Value_&&>( value ) );
      }
      return i;
    }

    /**
     * Add an element if the key does not exist in the map.
     *
     * @return Position of the inserted or the existing element with the same key.
     */
    template <typename Key_, typename Value_ = Value>
    int include( Key_&& key, Value_&& value = Value() )
    {
      int i = aBisectPosition( data, key, count );

      if( i == 0 || !( data[i - 1].key == key ) ) {
        insert( i, static_cast<Key_&&>( key ), static_cast<Value_&&>( value ) );
      }
      return i;
    }

    /**
     * Insert an element at the given position.
     *
     * All later elements are shifted to make a gap.
     * Use only when you are sure you are inserting at the right position to preserve order of the
     * element.
     */
    template <typename Key_, typename Value_ = Value>
    void insert( int i, Key_&& key, Value_&& value = Value() )
    {
      hard_assert( uint( i ) <= uint( count ) );

      ensureCapacity();

      aReverseMove( data + i + 1, data + i, count - i );
      data[i].key   = static_cast<Key_&&>( key );
      data[i].value = static_cast<Value_&&>( value );

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
      aMove( data + i, data + i + 1, count - i );
    }

    /**
     * Find and remove the element with the given key.
     *
     * @return Index of the removed element or -1 if not found.
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
     * For an empty map with no allocated storage, allocate capacity for <tt>size_</tt> elements.
     */
    void alloc( int size_ )
    {
      hard_assert( size == 0 && size_ > 0 );

      data = new Elem[size_];
      size = size_;
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
