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
 * @file ozCore/Range.hh
 *
 * `Range` class template.
 */

#pragma once

#include "System.hh"
#include "IteratorBase.hh"

namespace oz
{

/**
 * Container for begin-end iterator pair.
 *
 * `BeginIterator` must derive from `IteratorBase`.
 */
template <typename BeginIterator, typename EndIterator>
class Range
{
public:

  using Elem = typename BeginIterator::Elem;

  /**
   * Begin iterator type.
   */
  using Begin = BeginIterator;

  /**
   * End iterator type.
   */
  using End = EndIterator;

private:

  BeginIterator begin_ = BeginIterator(); ///< Begin iterator.
  EndIterator   end_   = EndIterator();   ///< End iterator.

public:

  /**
   * Create empty range with both iterators invalid.
   */
  OZ_ALWAYS_INLINE
  Range() noexcept = default;

  /**
   * Create range for given iterators.
   */
  OZ_ALWAYS_INLINE
  explicit Range(BeginIterator begin, EndIterator end) noexcept
    : begin_(begin), end_(end)
  {}

  /**
   * True iff both ranges are of the same length and all respective elements are equal.
   */
  bool operator==(const Range& other) const
  {
    return operator==<BeginIterator, EndIterator>(other);
  }

  /**
   * True iff both ranges are of the same length and all respective elements are equal.
   */
  template <typename BeginIterator_, typename EndIterator_>
  bool operator==(const Range<BeginIterator_, EndIterator_>& other) const
  {
    BeginIterator  beginA = begin_;
    BeginIterator_ beginB = other.begin();

    while (beginA != end_ && beginB != other.end()) {
      if (*beginA != *beginB) {
        return false;
      }

      ++beginA;
      ++beginB;
    }
    return beginA == end_ && beginB == other.end();
  }

  /**
   * Begin iterator.
   */
  OZ_ALWAYS_INLINE
  BeginIterator begin() const noexcept
  {
    return begin_;
  }

  /**
   * End iterator.
   */
  OZ_ALWAYS_INLINE
  EndIterator end() const noexcept
  {
    return end_;
  }

  /**
   * True iff range is empty.
   */
  OZ_ALWAYS_INLINE
  bool isEmpty() const noexcept
  {
    return begin_ == end_;
  }

  /**
   * First element in range.
   */
  OZ_ALWAYS_INLINE
  Elem& first() const
  {
    OZ_ASSERT(begin_ != end_);

    return *begin_;
  }

};

/**
 * Container for begin-end iterator pair, specialised for the case the iterators are pointers.
 */
template <typename ElemType>
class Range<ElemType*, ElemType*>
{
public:

  using Elem = ElemType;

  /**
   * Begin iterator type.
   */
  using Begin = Elem*;

  /**
   * End iterator type.
   */
  using End = Elem*;

private:

  Elem* begin_ = nullptr; ///< Begin iterator.
  Elem* end_   = nullptr; ///< End iterator.

public:

  /**
   * Create empty range with both iterators invalid.
   */
  OZ_ALWAYS_INLINE
  Range() noexcept = default;

  /**
   * Create range for given iterators.
   */
  OZ_ALWAYS_INLINE
  explicit Range(Elem* begin, Elem* end) noexcept
    : begin_(begin), end_(end)
  {}

  /**
   * True iff both ranges are of the same length and all respective elements are equal.
   */
  bool operator==(const Range& other) const
  {
    if (end_ - begin_ != other.end_ - other.begin_) {
      return false;
    }

    return operator==<Elem*, Elem*>(other);
  }

  /**
   * True iff both ranges are of the same length and all respective elements are equal.
   */
  template <typename BeginIterator_, typename EndIterator_>
  bool operator==(const Range<BeginIterator_, EndIterator_>& other) const
  {
    Elem*          beginA = begin_;
    BeginIterator_ beginB = other.begin();

    while (beginA != end_ && beginB != other.end()) {
      if (*beginA != *beginB) {
        return false;
      }

      ++beginA;
      ++beginB;
    }
    return beginA == end_ && beginB == other.end();
  }

  /**
   * Begin iterator.
   */
  OZ_ALWAYS_INLINE
  Elem* begin() const noexcept
  {
    return begin_;
  }

  /**
   * End iterator.
   */
  OZ_ALWAYS_INLINE
  Elem* end() const noexcept
  {
    return end_;
  }

  /**
   * Number of elements.
   */
  OZ_ALWAYS_INLINE
  int size() const noexcept
  {
    return end_ - begin_;
  }

  /**
   * True iff range is empty.
   */
  OZ_ALWAYS_INLINE
  bool isEmpty() const noexcept
  {
    return begin_ == end_;
  }

  /**
   * Reference to `i`-th element.
   */
  OZ_ALWAYS_INLINE
  Elem& operator[](int i) const
  {
    OZ_ASSERT(size_t(i) < size_t(end_ - begin_));

    return begin_[i];
  }

  /**
   * First element.
   */
  OZ_ALWAYS_INLINE
  Elem& first() const
  {
    OZ_ASSERT(begin_ < end_);

    return begin_[0];
  }

  /**
   * Last element.
   */
  OZ_ALWAYS_INLINE
  Elem& last() const
  {
    OZ_ASSERT(begin_ < end_);

    return end_[-1];
  }

};

/**
 * Range with constant access to container elements.
 */
template <class Container>
inline typename Container::CRangeType crange(const Container& container) noexcept
{
  return typename Container::CRangeType(container.cbegin(), container.cend());
}

/**
 * Range with constant access to constant container elements.
 */
template <class Container>
inline typename Container::CRangeType range(const Container& container) noexcept
{
  return typename Container::CRangeType(container.cbegin(), container.cend());
}

/**
 * Range with non-constant access to container elements.
 */
template <class Container>
inline typename Container::RangeType range(Container& container) noexcept
{
  return typename Container::RangeType(container.begin(), container.end());
}

/**
 * Range to access to a range of elements between two iterators.
 */
template <class BeginIterator, class EndIterator>
inline Range<BeginIterator, EndIterator> range(BeginIterator begin, EndIterator end)
{
  return Range<BeginIterator, EndIterator>(begin, end);
}

}
