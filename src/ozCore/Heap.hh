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

  using typename List<Elem>::CRange;
  using typename List<Elem>::Range;

private:

  using List<Elem>::data_;
  using List<Elem>::size_;
  using List<Elem>::ensureCapacity;

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
   * Trivial destructor.
   */
  ~Heap() = default;

  /**
   * Initialise from an initialiser list.
   */
  Heap(InitialiserList<Elem> il)
    : List<Elem>(int(il.size()))
  {
    size_ = 0;

    for (const Elem& e : il) {
      push(e);
    }
  }

  /**
   * Copy constructor, copies elements.
   */
  Heap(const Heap&) = default;

  /**
   * Move constructor, moves element storage.
   */
  Heap(Heap&&) noexcept = default;

  /**
   * Copy operator, copies elements.
   *
   * Existing storage is reused if it suffices.
   */
  Heap& operator=(const Heap&) = default;

  /**
   * Move operator, moves element storage.
   */
  Heap& operator=(Heap&&) noexcept = default;

  /**
   * Assign from an initialiser list.
   *
   * Existing storage is reused if it suffices.
   */
  Heap& operator=(InitialiserList<Elem> il)
  {
    clear();
    ensureCapacity(int(il.size()));

    for (const Elem& e : il) {
      push(e);
    }
    return *this;
  }

  /**
   * True iff respective elements are equal.
   */
  bool operator==(const Heap& other) const
  {
    return List<Elem>::operator==(other);
  }

  /**
   * Add an element.
   */
  template <typename Elem_>
  Elem& push(Elem_&& elem)
  {
    List<Elem>::add(static_cast<Elem_&&>(elem));

    int index  = size_ - 1;
    int parent = (size_ - 2) / 2;

    while (parent >= 0 && LessFunc()(data_[index], data_[parent])) {
      swap(data_[index], data_[parent]);

      index  = parent;
      parent = (parent - 1) / 2;
    }
    return data_[index];
  }

  /**
   * Remove and return the root element.
   */
  Elem pop()
  {
    Elem root = static_cast<Elem&&>(data_[0]);

    --size_;
    data_[0] = static_cast<Elem&&>(data_[size_]);

    int parent = 0;
    while (true) {
      int index = parent;
      int left  = 2 * parent + 1;
      int right = left + 1;

      if (left < size_ && LessFunc()(data_[left], data_[index])) {
        index = left;
      }
      if (right < size_ && LessFunc()(data_[right], data_[index])) {
        index = right;
      }
      if (index == parent) {
        return root;
      }

      swap(data_[parent], data_[index]);
      parent = index;
    }
  }

};

}
