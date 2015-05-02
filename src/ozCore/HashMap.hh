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
template <typename Key, typename Value, class HashFunc = Hash<Key>>
class HashMap : private HashSet<detail::MapPair<Key, Value>, HashFunc>
{
public:

  /**
   * Shortcut for key-value pair type.
   */
  typedef detail::MapPair<Key, Value> Pair;

  /**
   * %Iterator with constant access to elements.
   */
  typedef typename HashSet<Pair, HashFunc>::CIterator CIterator;

  /**
   * %Iterator with non-constant access to elements.
   */
  typedef typename HashSet<Pair, HashFunc>::Iterator Iterator;

private:

  using typename HashSet<Pair, HashFunc>::Entry;

  using HashSet<Pair, HashFunc>::pool;
  using HashSet<Pair, HashFunc>::data;
  using HashSet<Pair, HashFunc>::size;
  using HashSet<Pair, HashFunc>::ensureCapacity;

  /**
   * Delete all elements and referenced objects in a given chain.
   */
  void freeChain(Entry* entry)
  {
    while (entry != nullptr) {
      Entry* next = entry->next;

      delete entry->elem.value;
      entry->~Entry();
      pool.deallocate(entry);

      entry = next;
    }
  }

public:

  using HashSet<Pair, HashFunc>::citerator;
  using HashSet<Pair, HashFunc>::iterator;
  using HashSet<Pair, HashFunc>::begin;
  using HashSet<Pair, HashFunc>::end;
  using HashSet<Pair, HashFunc>::length;
  using HashSet<Pair, HashFunc>::isEmpty;
  using HashSet<Pair, HashFunc>::capacity;
  using HashSet<Pair, HashFunc>::contains;
  using HashSet<Pair, HashFunc>::exclude;
  using HashSet<Pair, HashFunc>::trim;
  using HashSet<Pair, HashFunc>::clear;

  /**
   * Create an empty hashtable.
   */
  HashMap() = default;

  /**
   * Initialise from an initialiser list.
   */
  HashMap(InitialiserList<Pair> l)
  {
    for (const Pair& p : l) {
      add(p.key, p.value);
    }
  }

  /**
   * Copy constructor, copies elements and storage.
   */
  HashMap(const HashMap& ht) = default;

  /**
   * Move constructor, moves storage.
   */
  HashMap(HashMap&& ht) = default;

  /**
   * Copy operator, copies elements and storage.
   */
  HashMap& operator = (const HashMap& ht) = default;

  /**
   * Move operator, moves storage.
   */
  HashMap& operator = (HashMap&& ht) = default;

  /**
   * Assign from an initialiser list.
   */
  HashMap& operator = (InitialiserList<Pair> l)
  {
    clear();

    for (const Pair& p : l) {
      add(p.key, p.value);
    }
    return *this;
  }

  /**
   * True iff contained elements are equal.
   */
  bool operator == (const HashMap& ht) const
  {
    return HashSet<Pair, HashFunc>::operator == (ht);
  }

  /**
   * False iff contained elements are equal.
   */
  bool operator != (const HashMap& ht) const
  {
    return HashSet<Pair, HashFunc>::operator != (ht);
  }

  /**
   * Constant pointer to the value for a given key or `nullptr` if not found.
   */
  template <typename Key_ = Key>
  const Value* find(const Key_& key) const
  {
    if (size == 0) {
      return nullptr;
    }

    int    h     = HashFunc()(key);
    uint   index = uint(h) % uint(size);
    Entry* entry = data[index];

    while (entry != nullptr) {
      if (key == entry->elem.key) {
        return &entry->elem.value;
      }
      entry = entry->next;
    }
    return nullptr;
  }

  /**
   * Pointer to the value for a given key or `nullptr` if not found.
   */
  template <typename Key_ = Key>
  Value* find(const Key_& key)
  {
    if (size == 0) {
      return nullptr;
    }

    int    h     = HashFunc()(key);
    uint   index = uint(h) % uint(size);
    Entry* entry = data[index];

    while (entry != nullptr) {
      if (key == entry->elem.key) {
        return &entry->elem.value;
      }
      entry = entry->next;
    }
    return nullptr;
  }

  /**
   * Add a new element, if the key already exists in the hashtable overwrite existing element.
   *
   * @return Reference to the value of the inserted element.
   */
  template <typename Key_ = Key, typename Value_ = Value>
  Value& add(Key_&& key, Value_&& value)
  {
    ensureCapacity(pool.length() + 1);

    int    h     = HashFunc()(key);
    uint   index = uint(h) % uint(size);
    Entry* entry = data[index];

    while (entry != nullptr) {
      if (key == entry->elem.key) {
        entry->elem.value = static_cast<Value_&&>(value);
        return entry->elem.value;
      }
      entry = entry->next;
    }

    data[index] = new(pool) Entry {
      data[index], h, { static_cast<Key_&&>(key), static_cast<Value_&&>(value) }
    };
    return data[index]->elem.value;
  }

  /**
   * Add a new element if the key does not exist in the hashtable.
   *
   * @return Reference to the value of the inserted or the existing element with the same key.
   */
  template <typename Key_ = Key, typename Value_ = Value>
  Value& include(Key_&& key, Value_&& value)
  {
    ensureCapacity(pool.length() + 1);

    int    h     = HashFunc()(key);
    uint   index = uint(h) % uint(size);
    Entry* entry = data[index];

    while (entry != nullptr) {
      if (key == entry->elem.key) {
        return entry->elem.value;
      }
      entry = entry->next;
    }

    data[index] = new(pool) Entry {
      data[index], h, { static_cast<Key_&&>(key), static_cast<Value_&&>(value) }
    };
    return data[index]->elem.value;
  }

  /**
   * Delete all objects referenced by element values and clear the hashtable.
   */
  void free()
  {
    for (int i = 0; i < size; ++i) {
      freeChain(data[i]);
      data[i] = nullptr;
    }
  }

};

}
