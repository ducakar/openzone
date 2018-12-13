/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2016 Davorin Učakar
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
 * Sorted array list of unique elements.
 *
 * %Set is implemented as a sorted array list.
 * Better worst case performance than a hashtable; however, for large sets `HashSet` is preferred as
 * it is asymptotically faster in average case.
 *
 * Like in `List` all allocated elements are constructed all the time so removed elements'
 * destruction is still guaranteed.
 *
 * Memory is allocated when the first element is added.
 *
 * @sa `oz::Map`, `oz::HashSet`, `oz::Heap`, `oz::List`
 */
template <typename Elem, class LessFunc = Less<Elem>>
class Set : protected List<Elem>
{
public:

  using typename List<Elem>::CRangeType;
  using typename List<Elem>::RangeType;

protected:

  using List<Elem>::data_;
  using List<Elem>::size_;
  using List<Elem>::ensureCapacity;

  /**
   * Insert an element, optionally overwriting an existing one.
   *
   * @return Position of the inserted element.
   */
  template <typename Elem_>
  Elem& insert(Elem_&& elem, bool overwrite)
  {
    int i = Arrays::bisection<Elem, Elem_, LessFunc>(data_, size_, elem);

    if (i != size_ && data_[i] == elem) {
      if (overwrite) {
        data_[i] = static_cast<Elem_&&>(elem);
      }
    }
    else {
      ensureCapacity(size_ + 1);

      Arrays::moveBackward<Elem>(data_ + i, size_ - i, data_ + i + 1);
      data_[i] = static_cast<Elem_&&>(elem);
      ++size_;
    }
    return data_[i];
  }

public:

  using List<Elem>::cbegin;
  using List<Elem>::begin;
  using List<Elem>::cend;
  using List<Elem>::end;
  using List<Elem>::size;
  using List<Elem>::isEmpty;
  using List<Elem>::capacity;
  using List<Elem>::operator[];
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
  Set(InitialiserList<Elem> il)
    : List<Elem>(il)
  {
    List<Elem>::template sort<LessFunc>();
  }

  /**
   * Assign from an initialiser list.
   *
   * Existing storage is reused if it suffices.
   */
  Set& operator=(InitialiserList<Elem> il)
  {
    List<Elem>::operator=(il);
    List<Elem>::template sort<LessFunc>();

    return *this;
  }

  /**
   * True iff respective elements are equal.
   */
  bool operator==(const Set& other) const
  {
    return List<Elem>::operator==(other);
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
    int i = Arrays::bisection<Elem, Key, LessFunc>(data_, size_, key);
    return i == size_ || !(data_[i] == key) ? -1 : i;
  }

  /**
   * Add an element overriding any existing equal element.
   *
   * @return Position of the inserted element.
   */
  template <typename Elem_>
  Elem& add(Elem_&& elem)
  {
    return insert<Elem_>(static_cast<Elem_&&>(elem), true);
  }

  /**
   * Add an element if if there is no equal element in the set.
   *
   * @return Position of the inserted or the existing element.
   */
  template <typename Elem_>
  Elem& include(Elem_&& elem)
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
    int i = Arrays::bisection<Elem, Key, LessFunc>(data_, size_, key);

    if (i != size_ && data_[i] == key) {
      erase(i);
      return i;
    }
    return -1;
  }

};

}
