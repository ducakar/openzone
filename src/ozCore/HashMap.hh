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
 * @file ozCore/HashMap.hh
 *
 * `HashMap` class template.
 */

#pragma once

#include "Map.hh"
#include "HashSet.hh"

namespace oz
{

/**
 * Chaining hashtable implementation.
 *
 * Memory is allocated when the first element is added.
 *
 * @sa `oz::HashSet`, `oz::Map`
 */
template <typename Key, typename Value>
class HashMap : private HashSet< detail::MapPair<Key, Value> >
{
private:

  /**
   * Shortcut for key-value pair type.
   */
  typedef detail::MapPair<Key, Value> Pair;

  using typename HashSet<Pair>::Bucket;

public:

  /**
   * %Iterator with constant access to elements.
   */
  typedef typename HashSet<Pair>::CIterator CIterator;

  /**
   * %Iterator with non-constant access to elements.
   */
  typedef typename HashSet<Pair>::Iterator Iterator;

private:

  using HashSet<Pair>::pool;
  using HashSet<Pair>::data;
  using HashSet<Pair>::size;
  using HashSet<Pair>::ensureCapacity;

  /**
   * Delete all elements and referenced objects in a given chain.
   */
  void freeChain( Bucket* chain )
  {
    while( chain != nullptr ) {
      Bucket* next = chain->next;

      delete chain->elem.value;
      chain->~Bucket();
      pool.deallocate( chain );

      chain = next;
    }
  }

public:

  using HashSet<Pair>::citer;
  using HashSet<Pair>::iter;
  using HashSet<Pair>::begin;
  using HashSet<Pair>::end;
  using HashSet<Pair>::length;
  using HashSet<Pair>::isEmpty;
  using HashSet<Pair>::capacity;
  using HashSet<Pair>::loadFactor;
  using HashSet<Pair>::trim;
  using HashSet<Pair>::clear;
  using HashSet<Pair>::deallocate;

  /**
   * Create an empty hashtable.
   */
  explicit HashMap() :
    HashSet<Pair>()
  {}

  /**
   * Initialise from an initialiser list.
   */
  HashMap( InitialiserList<Pair> l ) :
    HashSet<Pair>( l )
  {}

  /**
   * Copy constructor, copies elements and storage.
   */
  HashMap( const HashMap& ht ) :
    HashSet<Pair>( ht )
  {}

  /**
   * Move constructor, moves storage.
   */
  HashMap( HashMap&& ht ) :
    HashSet<Pair>( static_cast<HashMap&&>( ht ) )
  {}

  /**
   * Copy operator, copies elements and storage.
   */
  HashMap& operator = ( const HashMap& ht )
  {
    return static_cast<HashMap&>( HashSet<Pair>::operator = ( ht ) );
  }

  /**
   * Move operator, moves storage.
   */
  HashMap& operator = ( HashMap&& ht )
  {
    return static_cast<HashMap&>( HashSet<Pair>::operator = ( static_cast<HashMap&&>( ht ) ) );
  }

  /**
   * True iff contained elements are equal.
   */
  bool operator == ( const HashMap& ht ) const
  {
    return HashSet<Pair>::operator == ( ht );
  }

  /**
   * False iff contained elements are equal.
   */
  bool operator != ( const HashMap& ht ) const
  {
    return HashSet<Pair>::operator != ( ht );
  }

  /**
   * True iff a given key is found in the hashtable.
   */
  template <typename Key_ = Key>
  bool contains( const Key_& key ) const
  {
    if( size == 0 ) {
      return false;
    }

    int     h = hash( key );
    uint    i = uint( h ) % uint( size );
    Bucket* b = data[i];

    while( b != nullptr ) {
      if( b->elem.key == key ) {
        return true;
      }

      b = b->next;
    }
    return false;
  }

  /**
   * Constant pointer to the value for a given key or `nullptr` if not found.
   */
  template <typename Key_ = Key>
  const Value* find( const Key_& key ) const
  {
    if( size == 0 ) {
      return nullptr;
    }

    int     h = hash( key );
    uint    i = uint( h ) % uint( size );
    Bucket* b = data[i];

    while( b != nullptr ) {
      if( b->elem.key == key ) {
        return &b->elem.value;
      }
      b = b->next;
    }
    return nullptr;
  }

  /**
   * Pointer to the value for a given key or `nullptr` if not found.
   */
  template <typename Key_ = Key>
  Value* find( const Key_& key )
  {
    if( size == 0 ) {
      return nullptr;
    }

    int     h = hash( key );
    uint    i = uint( h ) % uint( size );
    Bucket* b = data[i];

    while( b != nullptr ) {
      if( b->elem.key == key ) {
        return &b->elem.value;
      }
      b = b->next;
    }
    return nullptr;
  }

  /**
   * Add a new element, if the key already exists in the hashtable overwrite existing element.
   *
   * @return Reference to the value of the inserted element.
   */
  template <typename Key_ = Key, typename Value_ = Value>
  Value& add( Key_&& key, Value_&& value )
  {
    ensureCapacity( pool.length() + 1 );

    int     h = hash( key );
    uint    i = uint( h ) % uint( size );
    Bucket* b = data[i];

    while( b != nullptr ) {
      if( b->elem.key == key ) {
        b->elem.value = static_cast<Value_&&>( value );
        return b->elem.value;
      }
      b = b->next;
    }

    data[i] = new( pool ) Bucket {
      data[i], h, { static_cast<Key_&&>( key ), static_cast<Value_&&>( value ) }
    };
    return data[i]->elem.value;
  }

  /**
   * Add a new element if the key does not exist in the hashtable.
   *
   * @return Reference to the value of the inserted or the existing element with the same key.
   */
  template <typename Key_ = Key, typename Value_ = Value>
  Value& include( Key_&& key, Value_&& value )
  {
    ensureCapacity( pool.length() + 1 );

    int     h = hash( key );
    uint    i = uint( h ) % uint( size );
    Bucket* b = data[i];

    while( b != nullptr ) {
      if( b->elem.key == key ) {
        return b->elem.value;
      }
      b = b->next;
    }

    data[i] = new( pool ) Bucket {
      data[i], h, { static_cast<Key_&&>( key ), static_cast<Value_&&>( value ) }
    };
    return data[i]->elem.value;
  }

  /**
   * Remove element with a given key.
   *
   * @return True iff the key was found (and removed).
   */
  bool exclude( const Key& key )
  {
    if( size == 0 ) {
      return nullptr;
    }

    uint     i     = uint( hash( key ) ) % uint( size );
    Bucket*  b     = data[i];
    Bucket** bPrev = &data[i];

    while( b != nullptr ) {
      if( b->elem.key == key ) {
        *bPrev = b->next;

        b->~Bucket();
        pool.deallocate( b );

        return true;
      }

      bPrev = &b->next;
      b = b->next;
    }
    return false;
  }

  /**
   * Delete all objects referenced by element values and clear the hashtable.
   */
  void free()
  {
    for( int i = 0; i < size; ++i ) {
      freeChain( data[i] );
      data[i] = nullptr;
    }
  }

};

}
