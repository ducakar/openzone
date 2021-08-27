/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2019 Davorin Učakar
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

/**
 * Key-value pair.
 */
template <typename Key, typename Value, class LessFunc = Less<Key>>
struct MapPair
{
  Key   key;   ///< Key.
  Value value; ///< Value.

  /**
   * Equality operator.
   */
  OZ_ALWAYS_INLINE
  constexpr bool operator==(const MapPair& other) const
  {
    return key == other.key;
  }

  /**
   * Less-than operator.
   */
  OZ_ALWAYS_INLINE
  constexpr bool operator<(const MapPair& other) const
  {
    return LessFunc()(key, other.key);
  }

  /**
   * Equality operator for pair-key comparison, required by `Set` class.
   */
  template <typename Key_>
  OZ_ALWAYS_INLINE
  constexpr bool operator==(const Key_& k) const
  {
    return key == k;
  }

  /**
   * Less-than operator for pair-key comparison, required for `aBisection()`.
   */
  template <typename Key_>
  OZ_ALWAYS_INLINE
  constexpr bool operator<(const Key_& k) const
  {
    return LessFunc()(key, k);
  }
};

/**
 * Sorted array list of unique key-value pairs.
 *
 * %Map is implemented as a sorted array list of key-value pairs.
 * Better worst case performance than a hashtable; however, for large maps `HashMap` is preferred as
 * it is asymptotically faster in average case.
 *
 * Like in `List` all allocated elements are constructed all the time so removed elements'
 * destruction is still guaranteed.
 *
 * Memory is allocated when the first element is added.
 *
 * @sa `oz::Set`, `oz::HashMap`
 */
template <typename Key, typename Value, class LessFunc = Less<Key>>
class Map : private Set<MapPair<Key, Value, LessFunc>>
{
public:

  /**
   * Shortcut for key-value pair type.
   */
  using Pair = MapPair<Key, Value, LessFunc>;

  using typename Set<Pair>::CRange;
  using typename Set<Pair>::Range;

private:

  using Set<Pair>::data_;
  using Set<Pair>::size_;
  using Set<Pair>::ensureCapacity;

  /**
   * Insert an element, optionally overwriting an existing one.
   *
   * This is a helper function to reduce code duplication between `add()` and `include()`.
   *
   * @return Position of the inserted element.
   */
  template <typename Key_, typename Value_>
  Pair& insert(Key_&& key, Value_&& value, bool overwrite)
  {
    int i = Arrays::bisection<Pair, Key>(data_, size_, key);

    if (i != size_ && data_[i].key == key) {
      if (overwrite) {
        data_[i].key   = static_cast<Key_&&>(key);
        data_[i].value = static_cast<Value_&&>(value);
      }
    }
    else {
      ensureCapacity(size_ + 1);

      Arrays::moveBackward<Pair>(data_ + i, size_ - i, data_ + i + 1);
      data_[i].key   = static_cast<Key_&&>(key);
      data_[i].value = static_cast<Value_&&>(value);
      ++size_;
    }
    return data_[i];
  }

public:

  using Set<Pair>::cbegin;
  using Set<Pair>::begin;
  using Set<Pair>::cend;
  using Set<Pair>::end;
  using Set<Pair>::size;
  using Set<Pair>::isEmpty;
  using Set<Pair>::capacity;
  using Set<Pair>::operator[];
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
  Map(InitialiserList<Pair> il)
    : Set<Pair>(il)
  {}

  /**
   * Assign from an initialiser list.
   *
   * Existing storage is reused if it suffices.
   */
  Map& operator=(InitialiserList<Pair> il)
  {
    Set<Pair>::operator=(il);
    return *this;
  }

  /**
   * True iff respective elements are equal.
   */
  bool operator==(const Map& other) const
  {
    return Set<Pair>::operator==(other);
  }

  /**
   * Constant pointer to the value for a given key or `nullptr` if not found.
   */
  template <typename Key_>
  const Value* find(const Key_& key) const
  {
    int i = Set<Pair>::template index<Key_>(key);
    return i < 0 ? nullptr : &data_[i].value;
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
  Pair& add(Key_&& key, Value_&& value)
  {
    return insert<Key_, Value_>(static_cast<Key_&&>(key), static_cast<Value_&&>(value), true);
  }

  /**
   * Add an element if the key does not exist in the map.
   *
   * @return Position of the inserted or the existing element.
   */
  template <typename Key_, typename Value_>
  Pair& include(Key_&& key, Value_&& value)
  {
    return insert<Key_, Value_>(static_cast<Key_&&>(key), static_cast<Value_&&>(value), false);
  }

  /**
   * Delete all objects referenced by elements (must be pointers) and clear the list.
   */
  void free()
  {
    for (int i = 0; i < size_; ++i) {
      data_[i].key = Key();
      delete data_[i].value;
    }
    size_ = 0;
  }

};

}
