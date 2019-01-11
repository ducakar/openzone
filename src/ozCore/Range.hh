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

#include "IteratorBase.hh"

namespace oz
{

/**
 * Container for begin-end iterator pair.
 */
template <typename BeginIterator, typename EndIterator>
class Range
{
public:

  /**
   * Element type.
   */
  using Elem = typename detail::StripRef<decltype(*BeginIterator())>::Bare;

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
   * Begin iterator.
   */
  OZ_ALWAYS_INLINE
  BeginIterator& begin() noexcept
  {
    return begin_;
  }

  /**
   * End iterator.
   */
  OZ_ALWAYS_INLINE
  EndIterator& end() noexcept
  {
    return end_;
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
