/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 */

/**
 * @file common/common.hh
 *
 * Main include file for the engine. Included via precompiled header <tt>stable.hh</tt>.
 *
 * @defgroup common Common layer
 */

#pragma once

#include "oz/oz.hh"
#include "oz/windefs.h"

#include "configuration.hh"

#include "common/Span.hh"
#include "common/Timer.hh"
#include "common/Lingua.hh"

// We want to use C++ wrapped C headers, not vanilla ones that are included via SDL.
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// Doxygen should skip those macros when generating documentation.
#ifndef OZ_DOXYGEN

#ifdef _WIN32
# include <windows.h>
// Fix M$ crap from Windows headers.
# undef ERROR
# undef PLANES
# undef near
# undef far
#endif

#define SDL_NO_COMPAT
#include <SDL.h>

#endif // OZ_DOXYGEN

// Forward declaration for Lua state, needed for Lua API declarations, to prevent pollution from Lua
// headers.
struct lua_State;

namespace oz
{

/**
 * Maximum allowed value for world coordinates.
 *
 * @ingroup common
 */
const int MAX_WORLD_COORD = 2048;

/**
 * Margin for collision detection.
 *
 * The maximum relative error for transition from world coordinates to relative coordinates is
 * \f$ |maxWorldCoord| \cdot \varepsilon \sqrt 3 \f$, where \f$ \varepsilon \f$ is maximum relative
 * rounding error (half of <tt>Math::FLOAT_EPS</tt>).
 * Rounding errors made during collision query should only represent a small fraction of that since
 * calculations are performed in relative coordinates on hundreds of times smaller scale.
 * When we translate an object additional errors are introduces.
 * <tt>position += collider.hit.ratio * move</tt> can introduce at most
 * \f$ 2 \cdot |maxWorldCoord| \cdot \varepsilon \sqrt 3 \f$ error.
 * Sum of all those errors should be less than <tt>|maxWorldCoord| * 3.0f * Math::FLOAT_EPS</tt>.
 *
 * @ingroup common
 */
const float EPSILON = 2048.0f * 3.0f * Math::FLOAT_EPS;

/**
 * Lua C API.
 *
 * @ingroup common
 */
typedef int LuaAPI( lua_State* );

}

/**
 * @page CommonLayer Common Layer
 */

/**
 * @mainpage OpenZone
 *
 * @li @subpage CommonLayer
 * @li @subpage MatrixLayer
 *
 */
