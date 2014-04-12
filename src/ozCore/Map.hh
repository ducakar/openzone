/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file ozCore/Map.hh
 *
 * `Map` class template.
 */

#pragma once

#include "arrays.hh"
#include "System.hh"

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
 * destruction is still guaranteed.
 *
 * Memory is allocated when the first element is added.
 *
 * @sa `oz::Set`, `oz::HashMap`
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
  struct Pair
  {
    Key   key;   ///< Key.
    Value value; ///< Value.

    /**
     * Less-than operator required for `aSort()`.
     */
    OZ_ALWAYS_INLINE
    bool operator < ( const Pair& e ) const
    {
      return key < e.key;
    }

    /**
     * Less-than operator required for `aBisection()`.
     */
    OZ_ALWAYS_INLINE
    friend bool operator < ( const Key& key, const Pair& e )
    {
      return key < e.key;
    }
  };

  /**
   * %Iterator with constant access to elements.
   */
  typedef ArrayIterator<const Pair> CIterator;

  /**
   * %Iterator with non-constant access to elements.
   */
  typedef ArrayIterator<Pair> Iterator;

private:

  Pair* data;  ///< Element storage.
  int   count; ///< Number of elements.
  int   size;  ///< Capacity, number of elements in storage.

  /**
   * Ensure a given capacity.
   *
   * Capacity is doubled if neccessary. If that doesn't suffice it is set to the least multiple of
   * `GRANULARITY` able to hold the requested number of elements.
   */
  void ensureCapacity( int capacity )
  {
    if( capacity < 0 ) {
      OZ_ERROR( "oz::Map: Capacity overflow" );
    }
    else if( size < capacity ) {
      size *= 2;
      size  = size < capacity ? ( capacity + GRANULARITY - 1 ) & ~( GRANULARITY - 1 ) : size;

      if( size <= 0 ) {
        OZ_ERROR( "oz::Map: Capacity overflow" );
      }

      data = aReallocate<Pair>( data, count, size );
    }
  }

public:

  /**
   * Create an empty map with a given initial capacity.
   */
  explicit Map( int capacity = 0 ) :
    data( capacity == 0 ? nullptr : new Pair[capacity] ), count( 0 ), size( capacity )
  {}

  /**
   * Initialise from an initialiser list.
   */
  Map( InitialiserList<Pair> l ) :
    data( new Pair[ l.size() ] ), count( int( l.size() ) ), size( int( l.size() ) )
  {
    aCopy<Pair>( l.begin(), int( l.size() ), data );
    aSort<Pair>( data, count );
  }

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
    data( m.size == 0 ? nullptr : new Pair[m.size] ), count( m.count ), size( m.size )
  {
    aCopy<Pair>( m.data, m.count, data );
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
   * Existing storage is reused if it suffices.
   */
  Map& operator = ( const Map& m )
  {
    if( &m == this ) {
      return *this;
    }

    if( size < m.count ) {
      delete[] data;

      data = new Pair[m.size];
      size = m.size;
    }

    aCopy<Pair>( m.data, m.count, data );
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

    data    = m.data;
    count   = m.count;
    size    = m.size;

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
    return count == m.count && aEquals<Pair>( data, count, m.data );
  }

  /**
   * False iff respective elements are equal.
   */
  bool operator != ( const Map& m ) const
  {
    return !operator == ( m );
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
  Iterator iter()
  {
    return Iterator( data, data + count );
  }

  /**
   * STL-compatible constant begin iterator.
   */
  OZ_ALWAYS_INLINE
  const Pair* begin() const
  {
    return data;
  }

  /**
   * STL-compatible begin iterator.
   */
  OZ_ALWAYS_INLINE
  Pair* begin()
  {
    return data;
  }

  /**
   * STL-compatible constant end iterator.
   */
  OZ_ALWAYS_INLINE
  const Pair* end() const
  {
    return data + count;
  }

  /**
   * STL-compatible end iterator.
   */
  OZ_ALWAYS_INLINE
  Pair* end()
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
  const Pair& operator [] ( int i ) const
  {
    hard_assert( uint( i ) < uint( count ) );

    return data[i];
  }

  /**
   * Reference to the `i`-th element.
   */
  OZ_ALWAYS_INLINE
  Pair& operator [] ( int i )
  {
    hard_assert( uint( i ) < uint( count ) );

    return data[i];
  }

  /**
   * Constant reference to the first element.
   */
  OZ_ALWAYS_INLINE
  const Pair& first() const
  {
    hard_assert( count != 0 );

    return data[0];
  }

  /**
   * Reference to the first element.
   */
  OZ_ALWAYS_INLINE
  Pair& first()
  {
    hard_assert( count != 0 );

    return data[0];
  }

  /**
   * Constant reference to the last element.
   */
  OZ_ALWAYS_INLINE
  const Pair& last() const
  {
    hard_assert( count != 0 );

    return data[count - 1];
  }

  /**
   * Reference to the last element.
   */
  OZ_ALWAYS_INLINE
  Pair& last()
  {
    hard_assert( count != 0 );

    return data[count - 1];
  }

  /**
   * True iff a given key is found in the map.
   */
  template <typename Key_ = Key>
  bool contains( const Key_& key ) const
  {
    int i = aBisection<Pair, Key_>( data, count, key );
    return i >= 0 && data[i].key == key;
  }

  /**
   * Index of the element with a given value or -1 if not found.
   */
  template <typename Key_ = Key>
  int index( const Key_& key ) const
  {
    int i = aBisection<Pair, Key_>( data, count, key );
    return i >= 0 && data[i].key == key ? i : -1;
  }

  /**
   * Constant pointer to the value for a given key or `nullptr` if not found.
   */
  template <typename Key_ = Key>
  const Value* find( const Key_& key ) const
  {
    int i = aBisection<Pair, Key_>( data, count, key );
    return i >= 0 && data[i].key == key ? &data[i].value : nullptr;
  }

  /**
   * Pointer to the value for a given key or `nullptr` if not found.
   */
  template <typename Key_ = Key>
  Value* find( const Key_& key )
  {
    int i = aBisection<Pair, Key_>( data, count, key );
    return i >= 0 && data[i].key == key ? &data[i].value : nullptr;
  }

  /**
   * Add an element or override value if an element with the same key exists.
   *
   * @return Position of the inserted element.
   */
  template <typename Key_ = Key, typename Value_ = Value>
  int add( Key_&& key, Value_&& value )
  {
    int i = aBisection<Pair, Key>( data, count, key );

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
    int i = aBisection<Pair, Key>( data, count, key );

    if( i >= 0 && data[i].key == key ) {
      return i;
    }
    else {
      insert<Key_, Value_>( i + 1, static_cast<Key_&&>( key ), static_cast<Value_&&>( value ) );
      return i + 1;
    }
  }

  /**
   * Insert an element at a given position.
   *
   * All later elements are shifted to make a gap.
   * Use only when you are sure you are inserting at the right position to preserve order of the
   * element.
   */
  template <typename Key_ = Key, typename Value_ = Value>
  void insert( int i, Key_&& key, Value_&& value = Value() )
  {
    hard_assert( uint( i ) <= uint( count ) );

    ensureCapacity( count + 1 );

    aMoveBackward<Pair>( data + i, count - i, data + i + 1 );
    data[i].key   = static_cast<Key_&&>( key );
    data[i].value = static_cast<Value_&&>( value );

    ++count;
  }

  /**
   * Remove the element at a given position.
   *
   * All later elements are shifted to fill the gap.
   */
  void erase( int i )
  {
    hard_assert( uint( i ) < uint( count ) );

    --count;

    if( i == count ) {
      // When removing the last element, no shift is performed, so it is not implicitly destroyed by
      // the move operation.
      data[count] = Pair();
    }
    else {
      aMove<Pair>( data + i + 1, count - i, data + i );
    }
  }

  /**
   * Find and remove the element with a given key.
   *
   * @return Index of the removed element or -1 if not found.
   */
  int exclude( const Key& key )
  {
    int i = aBisection<Pair, Key>( data, count, key );

    if( i >= 0 && data[i].key == key ) {
      erase( i );
      return i;
    }
    return -1;
  }

  /**
   * Resize the map to the specified number of elements.
   */
  void resize( int newCount )
  {
    if( newCount > count ) {
      ensureCapacity( newCount );
    }
    else {
      // Ensure destruction of removed elements.
      for( int i = newCount; i < count; ++i ) {
        data[i] = Pair();
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
      data = aReallocate<Pair>( data, count, size );
    }
  }

  /**
   * Clear the map.
   */
  void clear()
  {
    // Ensure destruction of all elements.
    aFill<Pair, Pair>( data, count, Pair() );
    count = 0;
  }

  /**
   * Delete all objects referenced by element values (must be pointers) and clear the map.
   */
  void free()
  {
    for( int i = 0; i < count; ++i ) {
      delete data[i].value;

      // Ensure destruction.
      data[i] = Pair();
    }
    count = 0;
  }

  /**
   * For an empty map with no allocated storage, allocate capacity for `capacity` elements.
   */
  void allocate( int capacity )
  {
    hard_assert( size == 0 && capacity > 0 );

    data = new Pair[capacity];
    size = capacity;
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
