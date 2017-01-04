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
 * Double-linked list with external storage.
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

  using typename Chain<Elem, INDEX>::CIterator;
  using typename Chain<Elem, INDEX>::Iterator;

private:

  using Chain<Elem, INDEX>::firstElem_;

  Elem* lastElem_ = nullptr; ///< Pointer to the last element in the chain.

public:

  using Chain<Elem, INDEX>::citerator;
  using Chain<Elem, INDEX>::iterator;
  using Chain<Elem, INDEX>::begin;
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
  DChain(DChain&& other) :
    Chain<Elem, INDEX>(static_cast<DChain&&>(other)), lastElem_(other.lastElem_)
  {
    other.lastElem_ = nullptr;
  }

  /**
   * Move operator, rebinds elements to the destination chain.
   */
  DChain& operator=(DChain&& other)
  {
    if (&other != this) {
      firstElem_ = other.firstElem_;
      lastElem_  = other.lastElem_;

      other.firstElem_ = nullptr;
      other.lastElem_  = nullptr;
    }
    return *this;
  }

  /**
   * Pointer to the last element.
   */
  OZ_ALWAYS_INLINE
  Elem* last() const
  {
    return lastElem_;
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
      lastElem_ = elem;
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
      firstElem_ = elem;
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
      firstElem_ = elem->next[INDEX];
    }
    else {
      elem->prev[INDEX]->next[INDEX] = elem->next[INDEX];
    }
    if (elem->next[INDEX] == nullptr) {
      lastElem_ = elem->prev[INDEX];
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
    elem->next[INDEX] = firstElem_;

    if (firstElem_ == nullptr) {
      firstElem_ = elem;
      lastElem_  = elem;
    }
    else {
      firstElem_->prev[INDEX] = elem;
      firstElem_ = elem;
    }
  }

  /**
   * Bind an element to the end of the chain.
   */
  void pushLast(Elem* elem)
  {
    OZ_ASSERT(elem != nullptr);

    elem->prev[INDEX] = lastElem_;
    elem->next[INDEX] = nullptr;

    if (lastElem_ == nullptr) {
      firstElem_ = elem;
      lastElem_  = elem;
    }
    else {
      lastElem_->next[INDEX] = elem;
      lastElem_ = elem;
    }
  }

  /**
   * Pop the first element from the chain.
   */
  Elem* popFirst()
  {
    OZ_ASSERT(firstElem_ != nullptr);

    Elem* e = firstElem_;

    firstElem_ = firstElem_->next[INDEX];

    if (firstElem_ == nullptr) {
      lastElem_ = nullptr;
    }
    else {
      firstElem_->prev[INDEX] = nullptr;
    }
    return e;
  }

  /**
   * Pop the last element from the chain.
   */
  Elem* popLast()
  {
    OZ_ASSERT(lastElem_ != nullptr);

    Elem* e = lastElem_;

    lastElem_ = lastElem_->prev[INDEX];

    if (lastElem_ == nullptr) {
      firstElem_ = nullptr;
    }
    else {
      lastElem_->next[INDEX] = nullptr;
    }
    return e;
  }

  /**
   * Empty the chain but do not delete the elements.
   */
  void clear()
  {
    firstElem_ = nullptr;
    lastElem_  = nullptr;
  }

  /**
   * Empty the chain and delete all elements.
   */
  void free()
  {
    Chain<Elem, INDEX>::free();
    lastElem_ = nullptr;
  }

};

}
