/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file ozCore/Pair.hh
 *
 * `Pair` class template.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * %Pair container.
 *
 * It holds two elements, not necessarily of the same type.
 */
template <typename ValueX, typename ValueY = ValueX>
class Pair
{
public:

  ValueX x; ///< First element.
  ValueY y; ///< Second element.

public:

  /**
   * Per-member equality operator.
   */
  OZ_ALWAYS_INLINE
  bool operator == ( const Pair& p ) const
  {
    return x == p.x && y == p.y;
  }

  /**
   * Per-member inequality operator.
   */
  OZ_ALWAYS_INLINE
  bool operator != ( const Pair& p ) const
  {
    return !( x == p.x ) || !( y == p.y );
  }

};

}
