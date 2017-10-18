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
 * @file ozCore/Range.hh
 *
 * `Range` class template.
 */

#pragma once

#include "IteratorBase.hh"

namespace oz
{

template <typename BeginIterator, typename EndIterator, typename ElemType>
class Range
{
public:

  /**
   * Begin iterator type.
   */
  typedef BeginIterator Begin;

  /**
   * End iterator type.
   */
  typedef EndIterator End;

  /**
   * Element type.
   */
  typedef ElemType Elem;

private:

  BeginIterator begin_; ///< Begin iterator.
  EndIterator   end_;   ///< End iterator.

public:

  Range() noexcept = default;

  explicit Range(BeginIterator begin, EndIterator end) noexcept
    : begin_(begin), end_(end)
  {}

  BeginIterator& begin() noexcept
  {
    return begin_;
  }

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

}
