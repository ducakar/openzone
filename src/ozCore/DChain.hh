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

  using Chain<Elem, INDEX>::firstElem;

  Elem* lastElem = nullptr; ///< Pointer to the last element in the chain.

public:

  using Chain<Elem, INDEX>::citerator;
  using Chain<Elem, INDEX>::iterator;
  using Chain<Elem, INDEX>::begin;
  using Chain<Elem, INDEX>::end;
  using Chain<Elem, INDEX>::first;
  using Chain<Elem, INDEX>::length;
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
  DChain(DChain&& c) :
    Chain<Elem, INDEX>(static_cast<DChain&&>(c)), lastElem(c.lastElem)
  {
    c.lastElem = nullptr;
  }

  /**
   * Move operator, rebinds elements to the destination chain.
   */
  DChain& operator = (DChain&& c)
  {
    if (&c != this) {
      firstElem = c.firstElem;
      lastElem  = c.lastElem;

      c.firstElem = nullptr;
      c.lastElem  = nullptr;
    }
    return *this;
  }

  /**
   * True iff same size and respective elements are equal.
   *
   * `Elem` type should implement `operator ==`, otherwise comparison doesn't make sense as two
   * copies always differ in `prev[INDEX]` and `next[INDEX]` members.
   */
  bool equals(const DChain& c) const
  {
    return Chain<Elem, INDEX>::equals(c);
  }

  /**
   * Pointer to the last element.
   */
  OZ_ALWAYS_INLINE
  Elem* last() const
  {
    return lastElem;
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
  void insertAfter(Elem* e, Elem* p)
  {
    hard_assert(e != nullptr && p != nullptr);

    Elem* next = p->next[INDEX];

    e->prev[INDEX] = p;
    e->next[INDEX] = p->next[INDEX];
    p->next[INDEX] = e;

    if (next == nullptr) {
      lastElem = e;
    }
    else {
      next->prev[INDEX] = e;
    }
  }

  /**
   * Bind an element before some given element in the chain.
   */
  void insertBefore(Elem* e, Elem* p)
  {
    hard_assert(e != nullptr && p != nullptr);

    Elem* prev = p->prev[INDEX];

    e->next[INDEX] = p;
    e->prev[INDEX] = prev;
    p->prev[INDEX] = e;

    if (prev == nullptr) {
      firstElem = e;
    }
    else {
      p->prev[INDEX] = e;
    }
  }

  /**
   * Unbind a given element from the chain.
   */
  void erase(Elem* elem)
  {
    if (elem->prev[INDEX] == nullptr) {
      firstElem = elem->next[INDEX];
    }
    else {
      elem->prev[INDEX]->next[INDEX] = elem->next[INDEX];
    }
    if (elem->next[INDEX] == nullptr) {
      lastElem = elem->prev[INDEX];
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
    hard_assert(elem != nullptr);

    elem->prev[INDEX] = nullptr;
    elem->next[INDEX] = firstElem;

    if (firstElem == nullptr) {
      firstElem = elem;
      lastElem  = elem;
    }
    else {
      firstElem->prev[INDEX] = elem;
      firstElem = elem;
    }
  }

  /**
   * Bind an element to the end of the chain.
   */
  void pushLast(Elem* elem)
  {
    hard_assert(elem != nullptr);

    elem->prev[INDEX] = lastElem;
    elem->next[INDEX] = nullptr;

    if (lastElem == nullptr) {
      firstElem = elem;
      lastElem  = elem;
    }
    else {
      lastElem->next[INDEX] = elem;
      lastElem = elem;
    }
  }

  /**
   * Pop the first element from the chain.
   */
  Elem* popFirst()
  {
    hard_assert(firstElem != nullptr);

    Elem* elem = firstElem;

    firstElem = firstElem->next[INDEX];

    if (firstElem == nullptr) {
      lastElem = nullptr;
    }
    else {
      firstElem->prev[INDEX] = nullptr;
    }
    return elem;
  }

  /**
   * Pop the last element from the chain.
   */
  Elem* popLast()
  {
    hard_assert(lastElem != nullptr);

    Elem* elem = lastElem;

    lastElem = lastElem->prev[INDEX];

    if (lastElem == nullptr) {
      firstElem = nullptr;
    }
    else {
      lastElem->next[INDEX] = nullptr;
    }
    return elem;
  }

  /**
   * Empty the chain but do not delete the elements.
   */
  void clear()
  {
    firstElem = nullptr;
    lastElem  = nullptr;
  }

  /**
   * Empty the chain and delete all elements.
   */
  void free()
  {
    Chain<Elem, INDEX>::free();
    lastElem = nullptr;
  }

};

}
