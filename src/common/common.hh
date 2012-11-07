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
 * Main include file for the engine. Included via precompiled header `stable.hh`.
 */

#pragma once

#include <ozCore/ozCore.hh>
#include <ozDynamics/collision/Bounds.hh>
#include <config.hh>

#ifdef _WIN32
# include <windows.h>
// Fix M$ crap from Windows headers.
# undef ERROR
# undef PLANES
# undef near
# undef far
#endif

// We want to use C++ wrapped C headers, not pure C ones that are included via SDL.
#include <climits>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#if defined( __ANDROID__ )
# include <SDL2/SDL.h>
#else
# include <SDL/SDL.h>
#endif

namespace oz
{
/**
 * Common layer.
 */
namespace common
{

/**
 * Maximum allowed value for world coordinates.
 */
const int MAX_WORLD_COORD = 2048;

/**
 * Margin for collision detection.
 *
 * The maximum relative error for transition from world coordinates to relative coordinates is
 * \f$ |maxWorldCoord| \cdot \varepsilon \sqrt 3 \f$, where \f$ \varepsilon \f$ is maximum relative
 * rounding error (half of `Math::FLOAT_EPS`).
 * Rounding errors made during collision query should only represent a small fraction of that since
 * calculations are performed in relative coordinates on hundreds of times smaller scale.
 * When we translate an object additional errors are introduces.
 * `position += collider.hit.ratio * move` can introduce at most
 * \f$ 2 \cdot |maxWorldCoord| \cdot \varepsilon \sqrt 3 \f$ error.
 * Sum of all those errors should be less than `|maxWorldCoord| * 3 * Math::FLOAT_EPS`.
 */
const float EPSILON = float( MAX_WORLD_COORD ) * 4.0f * Math::FLOAT_EPS;

/**
 * Wrap angle to interval \f$ [0, \tau) \f$.
 *
 * This adjustment should be made after each angle addition/subtraction. It assumes the input angle
 * lies on interval \f$ [-\tau, \infty) \f$.
 */
OZ_ALWAYS_INLINE
inline float angleWrap( float x )
{
  return Math::fmod( x + Math::TAU, Math::TAU );
}

/**
 * Difference between two angles, maps to interval \f$ [-\frac{\tau}{2}, +\frac{\tau}{2}) \f$.
 *
 * This function assumes that both angles lie on interval \f$ [0, \tau) \f$.
 */
OZ_ALWAYS_INLINE
inline float angleDiff( float x, float y )
{
  return Math::fmod( x - y + 1.5f*Math::TAU, Math::TAU ) - 0.5f*Math::TAU;
}

/**
 * 2D integer span.
 */
struct Span
{
  int minX; ///< Minimum X.
  int minY; ///< Minimum Y.
  int maxX; ///< Maximum X.
  int maxY; ///< Maximum Y.
};

}
}
