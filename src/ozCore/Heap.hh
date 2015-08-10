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
 * @file ozCore/Heap.hh
 *
 * `Heap` class template.
 */

#pragma once

#include "List.hh"

namespace oz
{

/**
 * Binary min-heap.
 *
 * The tree elements are stored in an array list.
 *
 * Like in `List` all allocated elements are constructed all the time so removed elements'
 * destruction is still guaranteed.
 *
 * Memory is allocated when the first element is added.
 *
 * @sa `oz::Set`, `oz::List`
 */
template <typename Elem, class LessFunc = Less<Elem>>
class Heap : private List<Elem>
{
public:

  using typename List<Elem>::CIterator;
  using typename List<Elem>::Iterator;

private:

  using List<Elem>::data;
  using List<Elem>::count;
  using List<Elem>::ensureCapacity;

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
  using List<Elem>::contains;
  using List<Elem>::index;
  using List<Elem>::reserve;
  using List<Elem>::trim;
  using List<Elem>::clear;
  using List<Elem>::free;

  /**
   * Create an empty heap.
   */
  Heap() = default;

  /**
   * Initialise from an initialiser list.
   */
  Heap(InitialiserList<Elem> l) :
    List<Elem>(int(l.size()))
  {
    count = 0;

    for (const Elem& e : l) {
      add(e);
    }
  }

  /**
   * Copy constructor, copies elements.
   */
  Heap(const Heap& h) = default;

  /**
   * Move constructor, moves element storage.
   */
  Heap(Heap&& h) = default;

  /**
   * Copy operator, copies elements.
   *
   * Existing storage is reused if it suffices.
   */
  Heap& operator = (const Heap& h) = default;

  /**
   * Move operator, moves element storage.
   */
  Heap& operator = (Heap&& h) = default;

  /**
   * Assign from an initialiser list.
   *
   * Existing storage is reused if it suffices.
   */
  Heap& operator = (InitialiserList<Elem> l)
  {
    clear();
    ensureCapacity(int(l.size()));

    for (const Elem& e : l) {
      add(e);
    }
  }

  /**
   * True iff respective elements are equal.
   */
  bool operator == (const Heap& h) const
  {
    return List<Elem>::operator == (h);
  }

  /**
   * False iff respective elements are equal.
   */
  bool operator != (const Heap& h) const
  {
    return List<Elem>::operator != (h);
  }

  /**
   * Add an element.
   */
  template <typename Elem_>
  Elem& push(Elem_&& elem)
  {
    List<Elem>::add(static_cast<Elem_&&>(elem));

    int index  = count - 1;
    int parent = (count - 2) / 2;

    while (parent >= 0 && LessFunc()(data[index], data[parent])) {
      swap(data[index], data[parent]);

      index  = parent;
      parent = (parent - 1) / 2;
    }
    return data[index];
  }

  /**
   * Remove and return the root element.
   */
  Elem pop()
  {
    Elem root = static_cast<Elem&&>(data[0]);

    --count;
    data[0] = static_cast<Elem&&>(data[count]);

    int parent = 0;
    do {
      int index = parent;
      int left  = 2 * parent + 1;
      int right = left + 1;

      if (left < count && LessFunc()(data[left], data[index])) {
        index = left;
      }
      if (right < count && LessFunc()(data[right], data[index])) {
        index = right;
      }
      if (index == parent) {
        return root;
      }

      swap(data[parent], data[index]);
      parent = index;
    }
    while (true);
  }

};

}
