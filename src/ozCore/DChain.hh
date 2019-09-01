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
 * @file ozCore/DChain.hh
 *
 * `DChain` class template.
 */

#pragma once

#include "Chain.hh"

namespace oz
{

/**
 * Base class for chain nodes, implementing `prev[]` and `next[]` members.
 */
template <class Elem, int INDICES = 1>
struct DChainNode
{
  Elem* next[INDICES]; ///< Pointer to the next element in the chain.
  Elem* prev[INDICES]; ///< Pointer to the previous element in the chain.
};

/**
 * Intrusive doubly linked list.
 *
 * This is not a real container but a way of binding existing elements into a linked list.
 *
 * It can only be applied on classes that have `prev[]` and `next[]` members. These can also be
 * provided by extending `DChain::Node` struct.
 * Example:
 * @code
 * struct C
 * {
 *   C*  prev[2];
 *   C*  next[2];
 *   int value;
 * };
 *
 * // Alternatively, by extending DChain::Node
 * struct C : DChainNode<C, 2>
 * {
 *   int value;
 * };
 *
 * DChain<C, 0> chain1;
 * DChain<C, 1> chain2;
 *
 * C* c = new C();
 * chain1.add(c);
 * chain2.add(c);
 * @endcode
 * That way an objects can exist in two separate chains at once;
 * `prev[0]` and `next[0]` point to previous and next element respectively in `chain1` while
 * `prev[1]` and `next[1]` point to previous and next element respectively in `chain2`.
 *
 * @note
 * - Removal operations (except for `free()` do not actually remove elements but only decouple them
 *   from the chain.
 * - `prev[INDEX]` and `next[INDEX]` pointers are not cleared when an element is removed from the
 *   chain, they may still point to elements in the chain or to invalid locations.
 *
 * @sa `oz::Chain`
 */
template <class Elem, int INDEX = 0>
class DChain : private Chain<Elem, INDEX>
{
public:

  using typename Chain<Elem, INDEX>::CRangeType;
  using typename Chain<Elem, INDEX>::RangeType;

private:

  using Chain<Elem, INDEX>::first_;

  Elem* last_ = nullptr; ///< Pointer to the last element in the chain.

public:

  using Chain<Elem, INDEX>::cbegin;
  using Chain<Elem, INDEX>::begin;
  using Chain<Elem, INDEX>::cend;
  using Chain<Elem, INDEX>::end;
  using Chain<Elem, INDEX>::first;
  using Chain<Elem, INDEX>::size;
  using Chain<Elem, INDEX>::isEmpty;
  using Chain<Elem, INDEX>::has;
  using Chain<Elem, INDEX>::contains;

  /**
   * Create an empty chain.
   */
  DChain() = default;

  /**
   * Move constructor, rebinds elements to the new chain.
   */
  DChain(DChain&& other) noexcept
    : DChain()
  {
    swap(*this, other);
  }

  /**
   * Move operator, rebinds elements to the destination chain.
   */
  DChain& operator=(DChain&& other) noexcept
  {
    swap(*this, other);
    return *this;
  }

  /**
   * Constant pointer to the last element.
   */
  OZ_ALWAYS_INLINE
  const Elem* last() const
  {
    return last_;
  }

  /**
   * Pointer to the last element.
   */
  OZ_ALWAYS_INLINE
  Elem* last()
  {
    return last_;
  }

  /**
   * Bind an element to the beginning of the chain.
   *
   * For efficiency reasons, elements are added to the beginning of a chain.
   */
  void add(Elem* elem)
  {
    pushFirst(elem);
  }

  /**
   * Bind an element after some given element in the chain.
   */
  void insertAfter(Elem* elem, Elem* prev)
  {
    OZ_ASSERT(elem != nullptr && prev != nullptr);

    Elem* next = prev->next[INDEX];

    elem->prev[INDEX] = prev;
    elem->next[INDEX] = prev->next[INDEX];
    prev->next[INDEX] = elem;

    if (next == nullptr) {
      last_ = elem;
    }
    else {
      next->prev[INDEX] = elem;
    }
  }

  /**
   * Bind an element before some given element in the chain.
   */
  void insertBefore(Elem* elem, Elem* next)
  {
    OZ_ASSERT(elem != nullptr && next != nullptr);

    Elem* prev = next->prev[INDEX];

    elem->next[INDEX] = next;
    elem->prev[INDEX] = prev;
    next->prev[INDEX] = elem;

    if (prev == nullptr) {
      first_ = elem;
    }
    else {
      next->prev[INDEX] = elem;
    }
  }

  /**
   * Unbind a given element from the chain.
   */
  void erase(Elem* elem)
  {
    if (elem->prev[INDEX] == nullptr) {
      first_ = elem->next[INDEX];
    }
    else {
      elem->prev[INDEX]->next[INDEX] = elem->next[INDEX];
    }
    if (elem->next[INDEX] == nullptr) {
      last_ = elem->prev[INDEX];
    }
    else {
      elem->next[INDEX]->prev[INDEX] = elem->prev[INDEX];
    }
  }

  /**
   * Bind an element to the beginning of the chain.
   */
  void pushFirst(Elem* elem)
  {
    OZ_ASSERT(elem != nullptr);

    elem->prev[INDEX] = nullptr;
    elem->next[INDEX] = first_;

    if (first_ == nullptr) {
      first_ = elem;
      last_  = elem;
    }
    else {
      first_->prev[INDEX] = elem;
      first_ = elem;
    }
  }

  /**
   * Bind an element to the end of the chain.
   */
  void pushLast(Elem* elem)
  {
    OZ_ASSERT(elem != nullptr);

    elem->prev[INDEX] = last_;
    elem->next[INDEX] = nullptr;

    if (last_ == nullptr) {
      first_ = elem;
      last_  = elem;
    }
    else {
      last_->next[INDEX] = elem;
      last_ = elem;
    }
  }

  /**
   * Pop the first element from the chain.
   */
  Elem* popFirst()
  {
    OZ_ASSERT(first_ != nullptr);

    Elem* e = first_;

    first_ = first_->next[INDEX];

    if (first_ == nullptr) {
      last_ = nullptr;
    }
    else {
      first_->prev[INDEX] = nullptr;
    }
    return e;
  }

  /**
   * Pop the last element from the chain.
   */
  Elem* popLast()
  {
    OZ_ASSERT(last_ != nullptr);

    Elem* e = last_;

    last_ = last_->prev[INDEX];

    if (last_ == nullptr) {
      first_ = nullptr;
    }
    else {
      last_->next[INDEX] = nullptr;
    }
    return e;
  }

  /**
   * Empty the chain but do not delete the elements.
   */
  void clear()
  {
    first_ = nullptr;
    last_  = nullptr;
  }

  /**
   * Empty the chain and delete all elements.
   */
  void free()
  {
    Chain<Elem, INDEX>::free();
    last_ = nullptr;
  }

};

}
