/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file oz/Pair.hpp
 */

#pragma once

#include "common.hpp"

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
     * Create an uninitialised instance.
     */
    Pair() = default;

    /**
     * Create a pair of given values.
     */
    template <typename ValueX_, typename ValueY_>
    OZ_ALWAYS_INLINE
    explicit Pair( ValueX_&& x_, ValueY_&& y_ ) :
        x( static_cast<ValueX_&&>( x_ ) ), y( static_cast<ValueY_&&>( y_ ) )
    {}

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
      return x != p.x || y != p.y;
    }

};

}
