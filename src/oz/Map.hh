/*
 * liboz - OpenZone Core Library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file oz/Map.hh
 *
 * Map template class.
 */

#pragma once

#include "arrays.hh"

namespace oz
{

/**
 * Sorted array list of key-value pairs.
 *
 * %Map is implemented as a sorted array list of key-value pairs.
 * Better worst case performance than a hashtable; however, for large maps `HashMap` is preferred as
 * it is asymptotically faster in average case.
 *
 * Like in `List` all allocated elements are constructed all the time and a removed element's
 * destruction is guaranteed.
 *
 * Memory is allocated when the first element is added.
 */
template <typename Key, typename Value>
class Map
{
  private:

    /// Granularity for automatic storage allocations.
    static const int GRANULARITY = 8;

  public:

    /**
     * Key-value pair.
     */
    class Elem
    {
      public:

        Key   key;   ///< Key.
        Value value; ///< Value.

      public:

        /**
         * Less-than operator required for `aBisection`.
         */
        OZ_ALWAYS_INLINE
        friend bool operator < ( const Key& key, const Elem& e )
        {
          return key < e.key;
        }

    };

    /**
     * %Iterator with constant access to container elements.
     */
    typedef ArrayIterator<const Elem> CIterator;

    /**
     * %Iterator with non-constant access to container elements.
     */
    typedef ArrayIterator<Elem> Iterator;

  private:

    Elem* data;  ///< Element storage.
    int   count; ///< Number of elements.
    int   size;  ///< Capacity, number of elements in storage.

    /**
     * Double capacity if there is not enough space to add another element.
     */
    void ensureCapacity()
    {
      if( size == count ) {
        size = size == 0 ? GRANULARITY : 2 * size;
        data = aReallocate<Elem>( data, count, size );
      }
    }

    /**
     * Enlarge capacity to the smallest multiple of `GRANULARITY` able to hold the requested number
     * of elements.
     */
    void ensureCapacity( int desiredSize )
    {
      if( size < desiredSize ) {
        size = ( ( desiredSize - 1 ) / GRANULARITY + 1 ) * GRANULARITY;
        data = aReallocate<Elem>( data, count, size );
      }
    }

  public:

    /**
     * Create an empty map with the given initial capacity.
     */
    explicit Map( int size_ = 0 ) :
      data( size_ == 0 ? nullptr : new Elem[size_] ), count( 0 ), size( size_ )
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
      data( m.size == 0 ? nullptr : new Elem[m.size] ), count( m.count ), size( m.size )
    {
      aCopy<Elem>( data, m.data, m.count );
    }

    /**
     * Move constructor, moves element storage.
     */
    Map( Map&& m ) :
      data( m.data ), count( m.count ), size( m.size )
    {
      m.data  = nullptr;
      m.count = 0;
      m.size  = 0;
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

      aCopy<Elem>( data, m.data, m.count );
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
      count = m.count;
      size  = m.size;

      m.data  = nullptr;
      m.count = 0;
      m.size  = 0;

      return *this;
    }

    /**
     * True iff respective elements are equal.
     */
    bool operator == ( const Map& m ) const
    {
      return count == m.count && aEquals<Elem>( data, m.data, count );
    }

    /**
     * False iff respective elements are equal.
     */
    bool operator != ( const Map& m ) const
    {
      return count != m.count || !aEquals<Elem>( data, m.data, count );
    }

    /**
     * %Iterator with constant access, initially points to the first element.
     */
    OZ_ALWAYS_INLINE
    CIterator citer() const
    {
      return CIterator( data, data + count );
    }

    /**
     * %Iterator with non-constant access, initially points to the first element.
     */
    OZ_ALWAYS_INLINE
    Iterator iter() const
    {
      return Iterator( data, data + count );
    }

    /**
     * STL-compatible constant begin iterator.
     */
    OZ_ALWAYS_INLINE
    const Elem* begin() const
    {
      return data;
    }

    /**
     * STL-compatible begin iterator.
     */
    OZ_ALWAYS_INLINE
    Elem* begin()
    {
      return data;
    }

    /**
     * STL-compatible constant end iterator.
     */
    OZ_ALWAYS_INLINE
    const Elem* end() const
    {
      return data + count;
    }

    /**
     * STL-compatible end iterator.
     */
    OZ_ALWAYS_INLINE
    Elem* end()
    {
      return data + count;
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
     * Constant reference to the `i`-th element.
     */
    OZ_ALWAYS_INLINE
    const Elem& operator [] ( int i ) const
    {
      hard_assert( uint( i ) < uint( count ) );

      return data[i];
    }

    /**
     * Reference to the `i`-th element.
     */
    OZ_ALWAYS_INLINE
    Elem& operator [] ( int i )
    {
      hard_assert( uint( i ) < uint( count ) );

      return data[i];
    }

    /**
     * Constant reference to the first element.
     */
    OZ_ALWAYS_INLINE
    const Elem& first() const
    {
      hard_assert( count != 0 );

      return data[0];
    }

    /**
     * Reference to the first element.
     */
    OZ_ALWAYS_INLINE
    Elem& first()
    {
      hard_assert( count != 0 );

      return data[0];
    }

    /**
     * Constant reference to the last element.
     */
    OZ_ALWAYS_INLINE
    const Elem& last() const
    {
      hard_assert( count != 0 );

      return data[count - 1];
    }

    /**
     * Reference to the last element.
     */
    OZ_ALWAYS_INLINE
    Elem& last()
    {
      hard_assert( count != 0 );

      return data[count - 1];
    }

    /**
     * True iff the given key is found in the map.
     */
    bool contains( const Key& key ) const
    {
      int i = aBisection<Elem, Key>( data, key, count );
      return i >= 0 && data[i].key == key;
    }

    /**
     * Index of the element with the given value or -1 if not found.
     */
    int index( const Key& key ) const
    {
      int i = aBisection<Elem, Key>( data, key, count );
      return i >= 0 && data[i].key == key ? i : -1;
    }

    /**
     * Constant pointer to the given key's value or `nullptr` if not found.
     */
    const Value* find( const Key& key ) const
    {
      int i = aBisection<Elem, Key>( data, key, count );
      return i >= 0 && data[i].key == key ? &data[i].value : nullptr;
    }

    /**
     * Pointer to the given key's value or `nullptr` if not found.
     */
    Value* find( const Key& key )
    {
      int i = aBisection<Elem, Key>( data, key, count );
      return i >= 0 && data[i].key == key ? &data[i].value : nullptr;
    }

    /**
     * Add an element or override value if an element with the same key exists.
     *
     * @return Position of the inserted or the existing element.
     */
    template <typename Key_ = Key, typename Value_ = Value>
    int add( Key_&& key, Value_&& value )
    {
      int i = aBisection<Elem, Key>( data, key, count );

      if( i >= 0 && data[i].key == key ) {
        data[i].key   = static_cast<Key_&&>( key );
        data[i].value = static_cast<Value_&&>( value );
        return i;
      }
      else {
        insert<Key_, Value_>( i + 1, static_cast<Key_&&>( key ), static_cast<Value_&&>( value ) );
        return i + 1;
      }
    }

    /**
     * Add an element if the key does not exist in the map.
     *
     * @return Position of the inserted or the existing element.
     */
    template <typename Key_ = Key, typename Value_ = Value>
    int include( Key_&& key, Value_&& value )
    {
      int i = aBisection<Elem, Key>( data, key, count );

      if( i >= 0 && data[i].key == key ) {
        return i;
      }
      else {
        insert<Key_, Value_>( i + 1, static_cast<Key_&&>( key ), static_cast<Value_&&>( value ) );
        return i + 1;
      }
    }

    /**
     * Insert an element at the given position.
     *
     * All later elements are shifted to make a gap.
     * Use only when you are sure you are inserting at the right position to preserve order of the
     * element.
     */
    template <typename Key_ = Key, typename Value_ = Value>
    void insert( int i, Key_&& key, Value_&& value = Value() )
    {
      hard_assert( uint( i ) <= uint( count ) );

      ensureCapacity();

      aMoveBackward<Elem>( data + i + 1, data + i, count - i );
      data[i].key   = static_cast<Key_&&>( key );
      data[i].value = static_cast<Value_&&>( value );

      ++count;
    }

    /**
     * Remove the element at the given position.
     *
     * All later elements are shifted to fill the gap.
     */
    void erase( int i )
    {
      hard_assert( uint( i ) < uint( count ) );

      --count;

      if( i == count ) {
        // When removing the last element, no shift is performed, so its resources are not
        // implicitly destroyed by a move operation.
        data[count] = Elem();
      }
      else {
        aMove<Elem>( data + i, data + i + 1, count - i );
      }
    }

    /**
     * Find and remove the element with the given key.
     *
     * @return Index of the removed element or -1 if not found.
     */
    int exclude( const Key& key )
    {
      int i = aBisection<Elem, Key>( data, key, count );

      if( i >= 0 && data[i].key == key ) {
        erase( i );
        return i;
      }
      return -1;
    }

    /**
     * Resize the map.
     */
    void resize( int newCount )
    {
      if( newCount > count ) {
        ensureCapacity( newCount );
      }
      else {
        // Ensure destruction of removed elements.
        for( int i = newCount; i < count; ++i ) {
          data[i] = Elem();
        }
      }
      count = newCount;
    }

    /**
     * Trim capacity to the current number of elements.
     */
    void trim()
    {
      if( count < size ) {
        size = count;
        data = aReallocate<Elem>( data, count, size );
      }
    }

    /**
     * Clear the map.
     */
    void clear()
    {
      // Ensure destruction of all elements.
      for( int i = 0; i < count; ++i ) {
        data[i] = Elem();
      }
      count = 0;
    }

    /**
     * Delete all objects referenced by elements and clear the map.
     */
    void free()
    {
      aFree<Elem>( data, count );
      clear();
    }

    /**
     * For an empty map with no allocated storage, allocate capacity for `size_` elements.
     */
    void allocate( int size_ )
    {
      hard_assert( size == 0 && size_ > 0 );

      data = new Elem[size_];
      size = size_;
    }

    /**
     * Deallocate storage of an empty map.
     */
    void deallocate()
    {
      hard_assert( count == 0 );

      delete[] data;

      data = nullptr;
      size = 0;
    }

};

}
