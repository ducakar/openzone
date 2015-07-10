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
 * @file ozCore/Chain.hh
 *
 * `Chain` class template.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * Base class for chain nodes, implementing `next[]` member.
 */
template <class Elem, int INDICES = 1>
struct ChainNode
{
  Elem* next[INDICES]; ///< Pointer to the next element in the chain.
};

/**
 * Linked list with external storage.
 *
 * This is not a real container but a way of binding existing elements into a linked list.
 *
 * It can only be applied on classes that have a `next[]` member. This can also be provided by
 * extending `Chain::Node` struct.
 * Example:
 * @code
 * struct C
 * {
 *   C*  next[2];
 *   int value;
 * };
 *
 * // Alternatively, by extending DChain::Node
 * struct C : ChainNode<C, 2>
 * {
 *   int value;
 * };
 *
 * Chain<C, 0> chain1;
 * Chain<C, 1> chain2;
 *
 * C* c = new C();
 * chain1.add(c);
 * chain2.add(c);
 * @endcode
 * That way an objects can exist in two separate chains at once;
 * `next[0]` points to next element in `chain1` and `next[1]` points to next element in `chain2`.
 *
 * @note
 * - Removal operations (except for `free()` do not actually remove elements but only decouple them
 *   from the chain.
 * - `next[INDEX]` pointer is not cleared when an element is removed from the chain, it may still
 *   point to elements in the chain or to invalid locations.
 *
 * @sa `oz::DChain`
 */
template <class Elem, int INDEX = 0>
class Chain
{
protected:

  /**
   * %Chain iterator.
   */
  template <class ElemType>
  class ChainIterator : public detail::IteratorBase<ElemType>
  {
  private:

    using detail::IteratorBase<ElemType>::elem;

  public:

    /**
     * Create an invalid iterator.
     */
    ChainIterator() = default;

    /**
     * Create chain iterator, initially pointing to a given element.
     */
    OZ_ALWAYS_INLINE
    explicit ChainIterator(const Chain& c) :
      detail::IteratorBase<ElemType>(c.firstElem)
    {}

    /**
     * Advance to the next element.
     */
    OZ_ALWAYS_INLINE
    ChainIterator& operator ++ ()
    {
      hard_assert(elem != nullptr);

      elem = elem->next[INDEX];
      return *this;
    }

    /**
     * STL-style begin iterator.
     */
    OZ_ALWAYS_INLINE
    ChainIterator begin() const
    {
      return *this;
    }

    /**
     * STL-style end iterator.
     */
    OZ_ALWAYS_INLINE
    ChainIterator end() const
    {
      return ChainIterator();
    }

  };

public:

  /**
   * %Iterator with constant access to elements.
   */
  typedef ChainIterator<const Elem> CIterator;

  /**
   * %Iterator with non-constant access to elements.
   */
  typedef ChainIterator<Elem> Iterator;

protected:

  Elem* firstElem = nullptr; ///< Pointer to the first element in the chain.

public:

  /**
   * Create an empty chain.
   */
  Chain() = default;

  /**
   * Move constructor, rebinds elements to the new chain.
   */
  Chain(Chain&& c) :
    firstElem(c.firstElem)
  {
    c.firstElem = nullptr;
  }

  /**
   * Move operator, rebinds elements to the destination chain.
   */
  Chain& operator = (Chain&& c)
  {
    if (&c != this) {
      firstElem = c.firstElem;

      c.firstElem = nullptr;
    }
    return *this;
  }

  /**
   * True iff same size and respective elements are equal.
   *
   * `Elem` type should implement `operator ==`, otherwise comparison doesn't make sense as two
   * copies always differ in `prev[INDEX]` and `next[INDEX]` members.
   */
  bool operator == (const Chain& c) const
  {
    const Elem* e1 = firstElem;
    const Elem* e2 = c.firstElem;

    while (e1 != nullptr && e2 != nullptr && *e1 == *e2) {
      e1 = e1->next[INDEX];
      e2 = e2->next[INDEX];
    }
    return e1 == nullptr && e2 == nullptr;
  }

  /**
   * %Iterator with constant access, initially points to the first element.
   */
  OZ_ALWAYS_INLINE
  CIterator citerator() const
  {
    return CIterator(*this);
  }

  /**
   * %Iterator with non-constant access, initially points to the first element.
   */
  OZ_ALWAYS_INLINE
  Iterator iterator()
  {
    return Iterator(*this);
  }

  /**
   * STL-style constant begin iterator.
   */
  OZ_ALWAYS_INLINE
  CIterator begin() const
  {
    return CIterator(*this);
  }

  /**
   * STL-style begin iterator.
   */
  OZ_ALWAYS_INLINE
  Iterator begin()
  {
    return Iterator(*this);
  }

  /**
   * STL-style constant end iterator.
   */
  OZ_ALWAYS_INLINE
  CIterator end() const
  {
    return CIterator();
  }

  /**
   * STL-style end iterator.
   */
  OZ_ALWAYS_INLINE
  Iterator end()
  {
    return Iterator();
  }

  /**
   * Iterate through the chain and count elements.
   */
  int length() const
  {
    int   i = 0;
    Elem* p = firstElem;

    while (p != nullptr) {
      p = p->next[INDEX];
      ++i;
    }
    return i;
  }

  /**
   * True iff the chain has no elements.
   */
  OZ_ALWAYS_INLINE
  bool isEmpty() const
  {
    return firstElem == nullptr;
  }

  /**
   * Pointer to the first element.
   */
  OZ_ALWAYS_INLINE
  Elem* first() const
  {
    return firstElem;
  }

  /**
   * Pointer to the element before a given one.
   *
   * Passing `nullptr` as the parameter returns the last element in the chain.
   */
  Elem* before(const Elem* elem) const
  {
    Elem* current = firstElem;
    Elem* before  = nullptr;

    while (current != elem) {
      if (current == nullptr) {
        return nullptr;
      }

      before  = current;
      current = current->next[INDEX];
    }
    return before;
  }

  /**
   * True iff a given element is in the chain.
   */
  bool has(const Elem* elem) const
  {
    hard_assert(elem != nullptr);

    Elem* p = firstElem;

    while (p != nullptr && p != elem) {
      p = p->next[INDEX];
    }
    return p != nullptr;
  }

  /**
   * True iff an element equal to a given one is in the chain.
   *
   * `Elem` type should implement `operator ==`, otherwise comparison doesn't make sense as two
   * copies always differ in `prev[INDEX]` and `next[INDEX]` members.
   */
  bool contains(const Elem* elem) const
  {
    hard_assert(elem != nullptr);

    Elem* p = firstElem;

    while (p != nullptr && !(*p == *elem)) {
      p = p->next[INDEX];
    }
    return p != nullptr;
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
    hard_assert(elem != nullptr && prev != nullptr);

    elem->next[INDEX] = prev->next[INDEX];
    prev->next[INDEX] = elem;
  }

  /**
   * Unbind a given element from the chain.
   *
   * Since this chain is not double-linked, a pointer to the preceding element (or `nullptr` if the
   * first element) must be provided.
   */
  void eraseAfter(Elem* elem, Elem* prev)
  {
    hard_assert(prev == nullptr || prev->next[INDEX] == elem);

    if (prev == nullptr) {
      firstElem = elem->next[INDEX];
    }
    else {
      prev->next[INDEX] = elem->next[INDEX];
    }
  }

  /**
   * Bind an element to the beginning of the chain.
   */
  void pushFirst(Elem* elem)
  {
    hard_assert(elem != nullptr);

    elem->next[INDEX] = firstElem;
    firstElem = elem;
  }

  /**
   * Unbind the first element from the chain.
   */
  Elem* popFirst()
  {
    hard_assert(firstElem != nullptr);

    Elem* elem = firstElem;

    firstElem = firstElem->next[INDEX];
    return elem;
  }

  /**
   * Empty the chain but do not delete the elements.
   */
  void clear()
  {
    firstElem = nullptr;
  }

  /**
   * Empty the chain and delete all elements.
   */
  void free()
  {
    Elem* p = firstElem;

    while (p != nullptr) {
      Elem* next = p->next[INDEX];

      delete p;
      p = next;
    }

    firstElem = nullptr;
  }

};

}
