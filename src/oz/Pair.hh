/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2011 Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file oz/Pair.hh
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * %Pair container.
 *
 * It holds two elements, not necessarily of the same type.
 *
 * @ingroup oz
 */
template <typename ValueX, typename ValueY = ValueX>
class Pair
{
  public:

    ValueX x; ///< First element.
    ValueY y; ///< Second element.

    /**
     * Per-member equality operator.
     */
    OZ_ALWAYS_INLINE
    constexpr bool operator == ( const Pair& p ) const
    {
      return x == p.x && y == p.y;
    }

    /**
     * Per-member inequality operator.
     */
    OZ_ALWAYS_INLINE
    constexpr bool operator != ( const Pair& p ) const
    {
      return x != p.x || y != p.y;
    }

};

}
