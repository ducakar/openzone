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

#include "Set.hh"

namespace oz
{

namespace detail
{

/**
 * Key-value pair.
 */
template <typename Key, typename Value, class LessFunc>
struct MapPair
{
  Key   key;   ///< Key.
  Value value; ///< Value.

  /**
   * Equality operator.
   */
  OZ_ALWAYS_INLINE
  constexpr bool operator == (const MapPair& p) const
  {
    return key == p.key;
  }

  /**
   * Less-than operator.
   */
  OZ_ALWAYS_INLINE
  constexpr bool operator < (const MapPair& p) const
  {
    return LessFunc()(key, p.key);
  }

  /**
   * Equality operator for key-pair comparison, required by `Set` class.
   */
  template <typename Key_>
  OZ_ALWAYS_INLINE
  friend constexpr bool operator == (const Key_& k, const MapPair& p)
  {
    return k == p.key;
  }

  /**
   * Less-than operator for key-pair comparison, required for `aBisection()`.
   */
  template <typename Key_>
  OZ_ALWAYS_INLINE
  friend constexpr bool operator < (const Key_& k, const MapPair& p)
  {
    return LessFunc()(k, p.key);
  }
};

}

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
template <typename Key, typename Value, class LessFunc = Less<void>>
class Map : private Set<detail::MapPair<Key, Value, LessFunc>>
{
public:

  /**
   * Shortcut for key-value pair type.
   */
  typedef detail::MapPair<Key, Value, LessFunc> Pair;

  /**
   * %Iterator with constant access to elements.
   */
  typedef typename Set<Pair>::CIterator CIterator;

  /**
   * %Iterator with non-constant access to elements.
   */
  typedef typename Set<Pair>::Iterator Iterator;

private:

  using Set<Pair>::data;
  using Set<Pair>::count;
  using Set<Pair>::size;
  using Set<Pair>::ensureCapacity;

  /**
   * Insert an element, optionally overwriting an existing one.
   *
   * This is a helper function to reduce code duplication between `add()` and `include()`.
   *
   * @return Position of the inserted element.
   */
  template <typename Key_, typename Value_>
  int insert(Key_&& key, Value_&& value, bool overwrite)
  {
    int i = Arrays::bisection<Pair, Key>(data, count, key);

    if (i >= 0 && data[i].key == key) {
      if (overwrite) {
        data[i].key   = static_cast<Key_&&>(key);
        data[i].value = static_cast<Value_&&>(value);
      }
      return i;
    }
    else {
      ensureCapacity(count + 1);
      ++i;

      Arrays::moveBackward<Pair>(data + i, count - i, data + i + 1);
      data[i].key   = static_cast<Key_&&>(key);
      data[i].value = static_cast<Value_&&>(value);
      ++count;

      return i;
    }
  }

public:

  using Set<Pair>::citerator;
  using Set<Pair>::iterator;
  using Set<Pair>::begin;
  using Set<Pair>::end;
  using Set<Pair>::length;
  using Set<Pair>::isEmpty;
  using Set<Pair>::capacity;
  using Set<Pair>::operator [];
  using Set<Pair>::first;
  using Set<Pair>::last;
  using Set<Pair>::contains;
  using Set<Pair>::index;
  using Set<Pair>::erase;
  using Set<Pair>::exclude;
  using Set<Pair>::reserve;
  using Set<Pair>::trim;
  using Set<Pair>::clear;

  /**
   * Create an empty map.
   */
  Map() = default;

  /**
   * Initialise from an initialiser list.
   */
  Map(InitialiserList<Pair> l) :
    Set<Pair>(l)
  {}

  /**
   * Copy constructor, copies elements.
   */
  Map(const Map& m) = default;

  /**
   * Move constructor, moves element storage.
   */
  Map(Map&& m) = default;

  /**
   * Copy operator, copies elements.
   *
   * Existing storage is reused if it suffices.
   */
  Map& operator = (const Map& m) = default;

  /**
   * Move operator, moves element storage.
   */
  Map& operator = (Map&& m) = default;

  /**
   * Assign from an initialiser list.
   *
   * Existing storage is reused if it suffices.
   */
  Map& operator = (InitialiserList<Pair> l)
  {
    return Set<Pair>::operator = (l);
  }

  /**
   * True iff respective elements are equal.
   */
  bool operator == (const Map& m) const
  {
    return Set<Pair>::operator == (m);
  }

  /**
   * False iff respective elements are equal.
   */
  bool operator != (const Map& m) const
  {
    return Set<Pair>::operator != (m);
  }

  /**
   * Constant pointer to the value for a given key or `nullptr` if not found.
   */
  template <typename Key_>
  const Value* find(const Key_& key) const
  {
    int i = Set<Pair>::template index<Key_>(key);
    return i < 0 ? nullptr : &data[i].value;
  }

  /**
   * Pointer to the value for a given key or `nullptr` if not found.
   */
  template <typename Key_>
  Value* find(const Key_& key)
  {
    return const_cast<Value*>(static_cast<const Map*>(this)->find<Key_>(key));
  }

  /**
   * Add an element or override value if an element with the same key exists.
   *
   * @return Position of the inserted element.
   */
  template <typename Key_, typename Value_>
  int add(Key_&& key, Value_&& value)
  {
    return insert<Key_, Value_>(static_cast<Key_&&>(key), static_cast<Value_&&>(value), true);
  }

  /**
   * Add an element if the key does not exist in the map.
   *
   * @return Position of the inserted or the existing element.
   */
  template <typename Key_, typename Value_>
  int include(Key_&& key, Value_&& value)
  {
    return insert<Key_, Value_>(static_cast<Key_&&>(key), static_cast<Value_&&>(value), false);
  }

  /**
   * Delete all objects referenced by elements (must be pointers) and clear the list.
   */
  void free()
  {
    for (int i = 0; i < count; ++i) {
      delete data[i].value;
    }
    count = 0;
  }

};

}
