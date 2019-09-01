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
 * @file ozCore/Chain.hh
 *
 * `Chain` class template.
 */

#pragma once

#include "Range.hh"
#include "System.hh"

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
 * Intrusive linked list.
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

    using detail::IteratorBase<ElemType>::elem_;

  public:

    /**
     * Create an invalid iterator.
     */
    OZ_ALWAYS_INLINE
    ChainIterator() noexcept = default;

    /**
     * Create chain iterator, initially pointing to a given element.
     */
    OZ_ALWAYS_INLINE
    explicit ChainIterator(const Chain& chain) noexcept
      : detail::IteratorBase<ElemType>(chain.first_)
    {}

    /**
     * Advance to the next element.
     */
    OZ_ALWAYS_INLINE
    ChainIterator& operator++() noexcept
    {
      OZ_ASSERT(elem_ != nullptr);

      elem_ = elem_->next[INDEX];
      return *this;
    }

  };

public:

  /**
   * %Range with constant access to elements.
   */
  using CRangeType = Range<ChainIterator<const Elem>, nullptr_t>;

  /**
   * %Range with non-constant access to elements.
   */
  using RangeType = Range<ChainIterator<Elem>, nullptr_t>;

protected:

  Elem* first_ = nullptr; ///< Pointer to the first element in the chain.

public:

  /**
   * Create an empty chain.
   */
  Chain() = default;

  /**
   * Move constructor, rebinds elements to the new chain.
   */
  Chain(Chain&& other) noexcept
    : Chain()
  {
    swap(*this, other);
  }

  /**
   * Move operator, rebinds elements to the destination chain.
   */
  Chain& operator=(Chain&& other) noexcept
  {
    swap(*this, other);
    return *this;
  }

  /**
   * STL-style constant begin iterator.
   */
  OZ_ALWAYS_INLINE
  ChainIterator<const Elem> cbegin() const noexcept
  {
    return ChainIterator<const Elem>(*this);
  }

  /**
   * STL-style constant begin iterator.
   */
  OZ_ALWAYS_INLINE
  ChainIterator<const Elem> begin() const noexcept
  {
    return ChainIterator<const Elem>(*this);
  }

  /**
   * STL-style begin iterator.
   */
  OZ_ALWAYS_INLINE
  ChainIterator<Elem> begin() noexcept
  {
    return ChainIterator<Elem>(*this);
  }

  /**
   * STL-style constant end iterator.
   */
  OZ_ALWAYS_INLINE
  nullptr_t cend() const noexcept
  {
    return nullptr;
  }

  /**
   * STL-style constant end iterator.
   */
  OZ_ALWAYS_INLINE
  nullptr_t end() const noexcept
  {
    return nullptr;
  }

  /**
   * STL-style end iterator.
   */
  OZ_ALWAYS_INLINE
  nullptr_t end() noexcept
  {
    return nullptr;
  }

  /**
   * Iterate through the chain and count elements.
   */
  int size() const noexcept
  {
    int   i = 0;
    Elem* e = first_;

    while (e != nullptr) {
      e = e->next[INDEX];
      ++i;
    }
    return i;
  }

  /**
   * True iff the chain has no elements.
   */
  OZ_ALWAYS_INLINE
  bool isEmpty() const noexcept
  {
    return first_ == nullptr;
  }

  /**
   * Constant pointer to the first element.
   */
  OZ_ALWAYS_INLINE
  const Elem* first() const
  {
    return first_;
  }

  /**
   * Pointer to the first element.
   */
  OZ_ALWAYS_INLINE
  Elem* first()
  {
    return first_;
  }

  /**
   * True iff a given element is in the chain.
   */
  bool has(const Elem* elem) const
  {
    OZ_ASSERT(elem != nullptr);

    Elem* p = first_;

    while (p != nullptr && p != elem) {
      p = p->next[INDEX];
    }
    return p != nullptr;
  }

  /**
   * True iff an element equal to a given one is in the chain.
   *
   * `Elem` type should implement `operator==`, otherwise comparison doesn't make sense as two
   * copies always differ in `prev[INDEX]` and `next[INDEX]` members.
   */
  bool contains(const Elem* elem) const
  {
    OZ_ASSERT(elem != nullptr);

    Elem* e = first_;

    while (e != nullptr && !(*e == *elem)) {
      e = e->next[INDEX];
    }
    return e != nullptr;
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
    OZ_ASSERT(prev == nullptr || prev->next[INDEX] == elem);

    if (prev == nullptr) {
      first_ = elem->next[INDEX];
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
    OZ_ASSERT(elem != nullptr);

    elem->next[INDEX] = first_;
    first_ = elem;
  }

  /**
   * Unbind the first element from the chain.
   */
  Elem* popFirst()
  {
    OZ_ASSERT(first_ != nullptr);

    Elem* e = first_;

    first_ = first_->next[INDEX];
    return e;
  }

  /**
   * Empty the chain but do not delete the elements.
   */
  void clear()
  {
    first_ = nullptr;
  }

  /**
   * Empty the chain and delete all elements.
   */
  void free()
  {
    Elem* e = first_;

    while (e != nullptr) {
      Elem* next = e->next[INDEX];

      delete e;
      e = next;
    }

    first_ = nullptr;
  }

};

}
