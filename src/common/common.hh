/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#ifdef _WIN32
# include <windows.h>
// Fix M$ crap from Windows headers.
# undef ERROR
# undef PLANES
# undef near
# undef far
#endif

// Some standard C/C++ headers.
#include <climits>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

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
 * Extent of world bounding box (equals `MAX_WORLD_COORD` but float type).
 */
const float WORLD_DIM = float( MAX_WORLD_COORD );

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
 * 2D integer span.
 */
struct Span
{
  int minX; ///< Minimum X.
  int minY; ///< Minimum Y.
  int maxX; ///< Maximum X.
  int maxY; ///< Maximum Y.
};

/**
 * Wrap angle to the interval \f$ [0, \tau) \f$.
 *
 * This adjustment should be made after each angle change. It assumes the input angle lies on
 * the interval \f$ [-\tau, \infty) \f$.
 */
OZ_ALWAYS_INLINE
inline float angleWrap( float x )
{
  return Math::fmod( x + Math::TAU, Math::TAU );
}

/**
 * Difference between two angles, maps to interval \f$ [-\frac{\tau}{2}, +\frac{\tau}{2}) \f$.
 *
 * This function assumes that both angles lie on the interval \f$ [0, \tau) \f$.
 */
OZ_ALWAYS_INLINE
inline float angleDiff( float x, float y )
{
  return Math::fmod( x - y + 1.5f*Math::TAU, Math::TAU ) - 0.5f*Math::TAU;
}

/**
 * Wrap position to the interval \f$ [-WORLD_DIM, +WORLD_DIM) \f$.
 *
 * This adjustment should be made after each position change. It assumes the input position lies
 * on the interval \f$ [-2 WORLD_DIM, \infty) \f$.
 */
OZ_ALWAYS_INLINE
inline Point posWrap( const Point& p )
{
  return Point( Math::fmod( p.x + 3.0f*WORLD_DIM, 2.0f*WORLD_DIM ) - WORLD_DIM,
                Math::fmod( p.y + 3.0f*WORLD_DIM, 2.0f*WORLD_DIM ) - WORLD_DIM,
                Math::fmod( p.z + 3.0f*WORLD_DIM, 2.0f*WORLD_DIM ) - WORLD_DIM );
}

/**
 * Difference between two points in the world.
 *
 * This function assumes that both points lie on the interval \f$ [-WORLD_DIM, +WORLD_DIM) \f$.
 */
OZ_ALWAYS_INLINE
inline Vec3 posDiff( const Point& p0, const Point& p1 )
{
  Vec3 diff = p0 - p1;

  return Vec3( Math::fmod( diff.x + 3.0f*WORLD_DIM, 2.0f*WORLD_DIM ) - WORLD_DIM,
               Math::fmod( diff.y + 3.0f*WORLD_DIM, 2.0f*WORLD_DIM ) - WORLD_DIM,
               Math::fmod( diff.z + 3.0f*WORLD_DIM, 2.0f*WORLD_DIM ) - WORLD_DIM );
}

}
}
