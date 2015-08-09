/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file common/Bounds.hh
 *
 * `Bounds` class.
 */

#pragma once

#include "AABB.hh"

namespace oz
{

/**
 * Axis-aligned bounding box, represented with minimal and maximal point.
 */
class Bounds
{
public:

  Point mins; ///< Minimums.
  Point maxs; ///< Maximums.

public:

  /**
   * Create uninitialised instance.
   */
  OZ_ALWAYS_INLINE
  Bounds() = default;

  /**
   * Create from a given minimal and maximal point.
   */
  OZ_ALWAYS_INLINE
  explicit Bounds(const Point& mins_, const Point& maxs_) :
    mins(mins_), maxs(maxs_)
  {}

  /**
   * Create `Bounds` from a point expanded for a given margin `eps`.
   */
  OZ_ALWAYS_INLINE
  explicit Bounds(const Point& p, float eps)
  {
    Vec3 epsDim = Vec3(eps, eps, eps);

    mins = p - epsDim;
    maxs = p + epsDim;
  }

  /**
   * Create `Bounds` from an `AABB` expanded for a given margin `eps`.
   */
  OZ_ALWAYS_INLINE
  explicit Bounds(const AABB& a, float eps)
  {
    Vec3 epsDim = a.dim + Vec3(eps, eps, eps);

    mins = a.p - epsDim;
    maxs = a.p + epsDim;
  }

  /**
   * Create `Bounds` expanded for a given margin `eps`.
   */
  OZ_ALWAYS_INLINE
  explicit Bounds(const Bounds& b, float eps)
  {
    Vec3 epsDim = Vec3(eps, eps, eps);

    mins = b.mins - epsDim;
    maxs = b.maxs + epsDim;
  }

  /**
   * Compute centre point.
   */
  Point p() const
  {
    return Math::mix<Point>(mins, maxs, 0.5f);
  }

  /**
   * Compute extents from centre (same as `dim` member in `AABB`).
   */
  Vec3 dim(float eps = 0.0f) const
  {
    return 0.5f * (maxs - mins) + Vec3(eps, eps, eps);
  }

  /**
   * Expand bounds to describe trace of bounds moved by a given vector.
   */
  Bounds& expand(const Vec3& move)
  {
    if (move.x < 0.0f) {
      mins.x += move.x;
    }
    else {
      maxs.x += move.x;
    }
    if (move.y < 0.0f) {
      mins.y += move.y;
    }
    else {
      maxs.y += move.y;
    }
    if (move.z < 0.0f) {
      mins.z += move.z;
    }
    else {
      maxs.z += move.z;
    }

    return *this;
  }

  /**
   * Create `AABB` that describes the same set as this `Bounds`.
   */
  OZ_ALWAYS_INLINE
  AABB toAABB(float eps = 0.0f) const
  {
    return AABB(p(), dim(eps));
  }

  /**
   * Translated `Bounds`.
   */
  OZ_ALWAYS_INLINE
  Bounds operator + (const Vec3& v) const
  {
    return Bounds(mins + v, maxs + v);
  }

  /**
   * Translated `Bounds`.
   */
  OZ_ALWAYS_INLINE
  Bounds operator - (const Vec3& v) const
  {
    return Bounds(mins - v, maxs - v);
  }

  /**
   * `Bounds` that covers union of two `Bounds`.
   */
  OZ_ALWAYS_INLINE
  Bounds operator | (const Bounds& b) const
  {
    return Bounds(min(mins, b.mins), max(maxs, b.maxs));
  }

  /**
   * Translate `Bounds`.
   */
  OZ_ALWAYS_INLINE
  Bounds& operator += (const Vec3& v)
  {
    mins += v;
    maxs += v;
    return *this;
  }

  /**
   * Translate `Bounds`.
   */
  OZ_ALWAYS_INLINE
  Bounds& operator -= (const Vec3& v)
  {
    mins -= v;
    maxs -= v;
    return *this;
  }

  /**
   * Extend `Bounds` to cover union of both `Bounds`.
   */
  OZ_ALWAYS_INLINE
  Bounds& operator |= (const Bounds& b)
  {
    mins = min(mins, b.mins);
    maxs = max(maxs, b.maxs);
    return *this;
  }

  /**
   * True iff a given point is inside this `Bounds`.
   *
   * @param p point.
   * @param eps margin for which this `Bounds` are enlarged (can also be negative).
   */
  OZ_ALWAYS_INLINE
  bool includes(const Point& p, float eps = 0.0f) const
  {
    return mins.x - eps <= p.x && p.x <= maxs.x + eps &&
           mins.y - eps <= p.y && p.y <= maxs.y + eps &&
           mins.z - eps <= p.z && p.z <= maxs.z + eps;
  }

  /**
   * True iff a given `AABB` is inside this `Bounds`.
   *
   * @param a AABB.
   * @param eps margin for which this `Bounds` are enlarged (can also be negative).
   */
  OZ_ALWAYS_INLINE
  bool includes(const AABB& a, float eps = 0.0f) const
  {
    Vec3 epsDim = a.dim + Vec3(eps, eps, eps);

    return mins.x <= a.p.x - epsDim.x && a.p.x + epsDim.x <= maxs.x &&
           mins.y <= a.p.y - epsDim.y && a.p.y + epsDim.y <= maxs.y &&
           mins.z <= a.p.z - epsDim.z && a.p.z + epsDim.z <= maxs.z;
  }

  /**
   * True iff a given `AABB` overlaps with this `Bounds`.
   *
   * @param a AABB.
   * @param eps margin for which this `Bounds` are enlarged (can also be negative).
   */
  OZ_ALWAYS_INLINE
  bool overlaps(const AABB& a, float eps = 0.0f) const
  {
    Vec3 epsDim = a.dim + Vec3(eps, eps, eps);

    return mins.x <= a.p.x + epsDim.x && a.p.x - epsDim.x <= maxs.x &&
           mins.y <= a.p.y + epsDim.y && a.p.y - epsDim.y <= maxs.y &&
           mins.z <= a.p.z + epsDim.z && a.p.z - epsDim.z <= maxs.z;
  }

  /**
   * True iff a given `Bounds` is inside this `Bounds`.
   *
   * @param b other `Bounds`.
   * @param eps margin for which this `Bounds` are enlarged (can also be negative).
   */
  OZ_ALWAYS_INLINE
  bool includes(const Bounds& b, float eps = 0.0f) const
  {
    return mins.x - eps <= b.mins.x && b.maxs.x <= maxs.x + eps &&
           mins.y - eps <= b.mins.y && b.maxs.y <= maxs.y + eps &&
           mins.z - eps <= b.mins.z && b.maxs.z <= maxs.z + eps;
  }

  /**
   * True iff a given `Bounds` overlaps with this `Bounds`.
   *
   * @param b other `Bounds`.
   * @param eps margin for which this `Bounds` are enlarged (can also be negative).
   */
  OZ_ALWAYS_INLINE
  bool overlaps(const Bounds& b, float eps = 0.0f) const
  {
    return mins.x - eps <= b.maxs.x && b.mins.x <= maxs.x + eps &&
           mins.y - eps <= b.maxs.y && b.mins.y <= maxs.y + eps &&
           mins.z - eps <= b.maxs.z && b.mins.z <= maxs.z + eps;
  }

};

}
