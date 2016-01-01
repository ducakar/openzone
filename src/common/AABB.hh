/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2016 Davorin Učakar
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
 * @file common/AABB.hh
 *
 * `AABB` class.
 */

#pragma once

#include <ozCore/ozCore.hh>

namespace oz
{

/**
 * Axis-aligned bounding box, represented with central point and extents.
 */
class AABB
{
public:

  Point p;   ///< Position of centre.
  Vec3  dim; ///< Extents (dimension halves).

public:

  /**
   * Create uninitialised instance.
   */
  OZ_ALWAYS_INLINE
  AABB() = default;

  /**
   * Create `AABB` with a given centre and extents.
   */
  OZ_ALWAYS_INLINE
  explicit AABB(const Point& p_, const Vec3& dim_) :
    p(p_), dim(dim_)
  {}

  /**
   * Create `AABB` enlarged for margin `eps` (can also be negative).
   */
  OZ_ALWAYS_INLINE
  explicit AABB(const AABB& a, float eps) :
    p(a.p), dim(a.dim + Vec3(eps, eps, eps))
  {}

  /**
   * Translated `AABB`.
   */
  OZ_ALWAYS_INLINE
  AABB operator + (const Vec3& v) const
  {
    return AABB(p + v, dim);
  }

  /**
   * Translated `AABB`.
   */
  OZ_ALWAYS_INLINE
  AABB operator - (const Vec3& v) const
  {
    return AABB(p - v, dim);
  }

  /**
   * Translate `AABB`.
   */
  OZ_ALWAYS_INLINE
  AABB& operator += (const Vec3& v)
  {
    p += v;
    return *this;
  }

  /**
   * Translate `AABB`.
   */
  OZ_ALWAYS_INLINE
  AABB& operator -= (const Vec3& v)
  {
    p -= v;
    return *this;
  }

  /**
   * True iff a given point is inside this `AABB`.
   *
   * @param point point.
   * @param eps margin for which this `AABB` is enlarged (can also be negative).
   */
  OZ_ALWAYS_INLINE
  bool includes(const Point& point, float eps = 0.0f) const
  {
    Vec3 relPos = abs(point - p);
    Vec3 sumDim = dim + Vec3(eps, eps, eps);

    return relPos.x <= sumDim.x && relPos.y <= sumDim.y && relPos.z <= sumDim.z;
  }

  /**
   * True iff a given `AABB` is inside this `AABB`.
   *
   * @param a other AABB.
   * @param eps margin for which this `AABB` is enlarged (can also be negative).
   */
  OZ_ALWAYS_INLINE
  bool includes(const AABB& a, float eps = 0.0f) const
  {
    Vec3 relPos = abs(a.p - p);
    Vec3 sumDim = abs(dim - a.dim) + Vec3(eps, eps, eps);

    return relPos.x <= sumDim.x && relPos.y <= sumDim.y && relPos.z <= sumDim.z;
  }

  /**
   * True iff a given `AABB` overlaps with this `AABB`.
   *
   * @param a other AABB.
   * @param eps margin for which this `AABB` is enlarged (can also be negative).
   */
  OZ_ALWAYS_INLINE
  bool overlaps(const AABB& a, float eps = 0.0f) const
  {
    Vec3 relPos = abs(a.p - p);
    Vec3 sumDim = dim + a.dim + Vec3(eps, eps, eps);

    return relPos.x <= sumDim.x && relPos.y <= sumDim.y && relPos.z <= sumDim.z;
  }

};

}
