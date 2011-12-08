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
 * @file common/Span.hh
 */

#pragma once

#include "oz/oz.hh"

namespace oz
{

/**
 * Span.
 *
 * 2D integer span.
 */
struct Span
{
  public:

    int minX; ///< Minimum X.
    int minY; ///< Minimum Y.
    int maxX; ///< Maximum X.
    int maxY; ///< Maximum Y.

    /**
     * Uninitialised instance.
     */
    Span() = default;

    /**
     * Construct with the given values.
     */
    OZ_ALWAYS_INLINE
    explicit Span( int minX_, int minY_, int maxX_, int maxY_ ) :
        minX( minX_ ), minY( minY_ ), maxX( maxX_ ), maxY( maxY_ )
    {}

};

}
