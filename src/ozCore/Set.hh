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
 * @file ozCore/Set.hh
 *
 * `Set` class template.
 */

#pragma once

#include "List.hh"

namespace oz
{

/**
 * Sorted array list.
 *
 * %Set is implemented as a sorted array list.
 * Better worst case performance than a hashtable; however, for large sets `HashSet` is preferred as
 * it is asymptotically faster in average case.
 *
 * Like in `List` all allocated elements are constructed all the time and a removed element's
 * destruction is still guaranteed.
 *
 * Memory is allocated when the first element is added.
 *
 * @sa `oz::Map`, `oz::HashSet`
 */
template <typename Elem, class LessFunc = Less<void>>
class Set : protected List<Elem>
{
public:

  /**
   * %Iterator with constant access to elements.
   */
  typedef typename List<Elem>::CIterator CIterator;

  /**
   * %Iterator with non-constant access to elements.
   */
  typedef typename List<Elem>::Iterator Iterator;

protected:

  using List<Elem>::data;
  using List<Elem>::count;
  using List<Elem>::size;
  using List<Elem>::ensureCapacity;

  /**
   * Insert an element, optionally overwriting an existing one.
   *
   * @return Position of the inserted element.
   */
  template <typename Elem_>
  int insert(Elem_&& elem, bool overwrite)
  {
    int i = Arrays::bisection<Elem, Elem_, LessFunc>(data, count, elem);

    if (i >= 0 && elem == data[i]) {
      if (overwrite) {
        data[i] = static_cast<Elem_&&>(elem);
      }
      return i;
    }
    else {
      ensureCapacity(count + 1);
      ++i;

      Arrays::moveBackward<Elem>(data + i, count - i, data + i + 1);
      data[i] = static_cast<Elem_&&>(elem);
      ++count;

      return i;
    }
  }

public:

  using List<Elem>::citerator;
  using List<Elem>::iterator;
  using List<Elem>::begin;
  using List<Elem>::end;
  using List<Elem>::length;
  using List<Elem>::isEmpty;
  using List<Elem>::capacity;
  using List<Elem>::operator [];
  using List<Elem>::first;
  using List<Elem>::last;
  using List<Elem>::erase;
  using List<Elem>::reserve;
  using List<Elem>::trim;
  using List<Elem>::clear;

  /**
   * Create an empty set.
   */
  Set() = default;

  /**
   * Initialise from an initialiser list.
   */
  Set(InitialiserList<Elem> l) :
    List<Elem>(l)
  {
    List<Elem>::template sort<LessFunc>();
  }

  /**
   * Copy constructor, copies elements.
   */
  Set(const Set& s) = default;

  /**
   * Move constructor, moves element storage.
   */
  Set(Set&& s) = default;

  /**
   * Copy operator, copies elements.
   *
   * Existing storage is reused if it suffices.
   */
  Set& operator = (const Set& s) = default;

  /**
   * Move operator, moves element storage.
   */
  Set& operator = (Set&& s) = default;

  /**
   * Assign from an initialiser list.
   *
   * Existing storage is reused if it suffices.
   */
  Set& operator = (InitialiserList<Elem> l)
  {
    List<Elem>::operator = (l);
    List<Elem>::template sort<LessFunc>();

    return *this;
  }

  /**
   * True iff respective elements are equal.
   */
  bool operator == (const Set& s) const
  {
    return List<Elem>::operator == (s);
  }

  /**
   * False iff respective elements are equal.
   */
  bool operator != (const Set& s) const
  {
    return List<Elem>::operator != (s);
  }

  /**
   * True iff an element that matches a given key is found in the set.
   */
  template <typename Key>
  bool contains(const Key& key) const
  {
    return index<Key>(key) >= 0;
  }

  /**
   * Index of the element that matches a given key or -1 if not found.
   */
  template <typename Key>
  int index(const Key& key) const
  {
    int i = Arrays::bisection<Elem, Key, LessFunc>(data, count, key);
    return i < 0 || !(key == data[i]) ? -1 : i;
  }

  /**
   * Add an element overriding any existing equal element.
   *
   * @return Position of the inserted element.
   */
  template <typename Elem_>
  int add(Elem_&& elem)
  {
    return insert<Elem_>(static_cast<Elem_&&>(elem), true);
  }

  /**
   * Add an element if if there is no equal element in the set.
   *
   * @return Position of the inserted or the existing element.
   */
  template <typename Elem_>
  int include(Elem_&& elem)
  {
    return insert<Elem_>(static_cast<Elem_&&>(elem), false);
  }

  /**
   * Find and remove the element that mathes a given key.
   *
   * @return Index of the removed element or -1 if not found.
   */
  template <typename Key>
  int exclude(const Key& key)
  {
    int i = Arrays::bisection<Elem, Key, LessFunc>(data, count, key);

    if (i >= 0 && key == data[i]) {
      erase(i);
      return i;
    }
    return -1;
  }

};

}
