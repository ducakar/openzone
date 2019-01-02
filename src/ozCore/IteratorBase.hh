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
 * @file ozCore/IteratorBase.hh
 *
 * `IteratorBase` class template.
 */

#pragma once

#include "common.hh"

namespace oz::detail
{

/**
 * Base class for iterators.
 *
 * It should only be used as a base class. The derived class must implement `operator++`.
 */
template <typename ElemType>
class IteratorBase
{
public:

  /**
   * Element type.
   */
  using Elem = ElemType;

protected:

  ElemType* elem_ = nullptr; ///< The element the iterator is currently pointing at.

protected:

  /**
   * Create an invalid iterator.
   */
  OZ_ALWAYS_INLINE
  IteratorBase() noexcept = default;

  /**
   * Create an iterator pointing to a given element.
   */
  OZ_ALWAYS_INLINE
  constexpr explicit IteratorBase(ElemType* first) noexcept
    : elem_(first)
  {}

public:

  /**
   * True as long as iterator has not passed all elements.
   */
  OZ_ALWAYS_INLINE
  bool operator==(nullptr_t) const noexcept
  {
    return elem_ == nullptr;
  }

  /**
   * False as long as iterator has not passed all elements.
   */
  OZ_ALWAYS_INLINE
  bool operator!=(nullptr_t) const noexcept
  {
    return elem_ != nullptr;
  }

  /**
   * Pointer to the current element's member.
   */
  OZ_ALWAYS_INLINE
  ElemType* operator->() const noexcept
  {
    return elem_;
  }

  /**
   * Reference to the current element.
   */
  OZ_ALWAYS_INLINE
  ElemType& operator*() const noexcept
  {
    return *elem_;
  }

  /**
   * Advance to the next element; should be implemented in derived classes.
   */
  IteratorBase& operator++() noexcept = delete;

};

}
