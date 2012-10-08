/*
 * liboz - OpenZone Core Library.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file oz/AABB.hh
 *
 * AABB class.
 */

#pragma once

#include "Point.hh"

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
    explicit AABB() = default;

    /**
     * Create `AABB` with the given centre and extents.
     */
    OZ_ALWAYS_INLINE
    explicit AABB( const Point& p_, const Vec3& dim_ ) :
      p( p_ ), dim( dim_ )
    {}

    /**
     * Create `AABB` enlarged for margin `eps` (can also be negative).
     */
    OZ_ALWAYS_INLINE
    explicit AABB( const AABB& a, float eps ) :
      p( a.p ), dim( a.dim + Vec3( eps, eps, eps ) )
    {}

    /**
     * Translated `AABB`.
     */
    OZ_ALWAYS_INLINE
    AABB operator + ( const Vec3& v ) const
    {
      return AABB( p + v, dim );
    }

    /**
     * Translated `AABB`.
     */
    OZ_ALWAYS_INLINE
    AABB operator - ( const Vec3& v ) const
    {
      return AABB( p - v, dim );
    }

    /**
     * Translate `AABB`.
     */
    OZ_ALWAYS_INLINE
    AABB& operator += ( const Vec3& v )
    {
      p += v;
      return *this;
    }

    /**
     * Translate `AABB`.
     */
    OZ_ALWAYS_INLINE
    AABB& operator -= ( const Vec3& v )
    {
      p -= v;
      return *this;
    }

    /**
     * True iff the given point is inside this `AABB`.
     *
     * @param point point.
     * @param eps margin for which this `AABB` is enlarged (can also be negative).
     */
    OZ_ALWAYS_INLINE
    bool includes( const Point& point, float eps = 0.0f ) const
    {
      Vec3 relPos = ( point - p ).abs();
      Vec3 sumDim = dim + Vec3( eps, eps, eps );

      return relPos.x <= sumDim.x && relPos.y <= sumDim.y && relPos.z <= sumDim.z;
    }

    /**
     * True iff the given `AABB` is inside this `AABB`.
     *
     * @param a other AABB.
     * @param eps margin for which this `AABB` is enlarged (can also be negative).
     */
    OZ_ALWAYS_INLINE
    bool includes( const AABB& a, float eps = 0.0f ) const
    {
      Vec3 relPos = ( a.p - p ).abs();
      Vec3 sumDim = ( dim - a.dim ).abs() + Vec3( eps, eps, eps );

      return relPos.x <= sumDim.x && relPos.y <= sumDim.y && relPos.z <= sumDim.z;
    }

    /**
     * True iff the given `AABB` overlaps with this `AABB`.
     *
     * @param a other AABB.
     * @param eps margin for which this `AABB` is enlarged (can also be negative).
     */
    OZ_ALWAYS_INLINE
    bool overlaps( const AABB& a, float eps = 0.0f ) const
    {
      Vec3 relPos = ( a.p - p ).abs();
      Vec3 sumDim = dim + a.dim + Vec3( eps, eps, eps );

      return relPos.x <= sumDim.x && relPos.y <= sumDim.y && relPos.z <= sumDim.z;
    }

};

}
