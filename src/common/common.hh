/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2019 Davorin Učakar
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
 * Main include file for the engine.
 *
 * This header is also precompiled via `src/pch.hh` and included by all other OpenZone layers.
 */

#pragma once

#include <common/pch.hh>
#include <common/AABB.hh>
#include <common/Bounds.hh>
#include <common/Lingua.hh>
#include <common/Timer.hh>

namespace oz
{

/**
 * Maximum allowed value for world coordinates.
 */
constexpr int MAX_WORLD_COORD = 2048;

/**
 * Margin for collision detection.
 *
 * The maximum relative error for transition from world coordinates to relative coordinates is
 * \f$ |maxWorldCoord| \cdot \varepsilon \sqrt 3 \f$, where \f$ \varepsilon \f$ is maximum relative
 * rounding error (half of `Math::FLOAT_EPS`).
 * Rounding errors made during collision query should only represent a small fraction of that since
 * calculations are performed in relative coordinates on hundreds of times smaller scale.
 * When we translate an object we can introduce additional
 * \f$ \cdot |maxWorldCoord| \cdot \varepsilon \sqrt 3 \f$ error.
 * Sum of all those errors should hence be less than `|maxWorldCoord| * 2 * Math::FLOAT_EPS`.
 */
constexpr float EPSILON = float(MAX_WORLD_COORD) * 2.0f * Math::FLOAT_EPS;

/**
 * 2D integer position.
 */
struct Pos2
{
  int x; ///< X component.
  int y; ///< Y component.
};

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
 */
OZ_ALWAYS_INLINE
inline float angleWrap(float x)
{
  return Math::mod(x, Math::TAU);
}

/**
 * Difference between two angles, maps to interval \f$ [-\frac{\tau}{2}, +\frac{\tau}{2}) \f$.
 */
OZ_ALWAYS_INLINE
inline float angleDiff(float x, float y)
{
  return Math::mod(x - y + Math::TAU / 2.0f, Math::TAU) - Math::TAU / 2.0f;
}

}
