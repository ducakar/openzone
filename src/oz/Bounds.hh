/*
 * liboz - OpenZone core library.
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
 * @file oz/Bounds.hh
 *
 * Bounds class.
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
    Bounds() = default;

    /**
     * Create from the given minimal and maximal point.
     */
    OZ_ALWAYS_INLINE
    explicit Bounds( const Point& mins_, const Point& maxs_ ) :
      mins( mins_ ), maxs( maxs_ )
    {}

    /**
     * Create `Bounds` enlarged for the given margin `eps`.
     */
    OZ_ALWAYS_INLINE
    explicit Bounds( const Bounds& b, float eps ) :
      mins( b.mins - Vec3( eps, eps, eps ) ), maxs( b.maxs + Vec3( eps, eps, eps ) )
    {}

    /**
     * Create `Bounds` that describes the same set as the given `AABB`.
     */
    OZ_ALWAYS_INLINE
    explicit Bounds( const AABB& a, float eps = 0.0f )
    {
      Vec3 epsDim = a.dim + Vec3( eps, eps, eps );

      mins = a.p - epsDim;
      maxs = a.p + epsDim;
    }

    /**
     * Create `Bounds` that cover the trace described by the given point move.
     */
    OZ_ALWAYS_INLINE
    explicit Bounds( const Point& p, const Vec3& move, float eps = 0.0f )
    {
      Vec3 epsDim = Vec3( eps, eps, eps );

      mins = p - epsDim;
      maxs = p + epsDim;

      if( move.x < 0.0f ) {
        mins.x += move.x;
      }
      else {
        maxs.x += move.x;
      }
      if( move.y < 0.0f ) {
        mins.y += move.y;
      }
      else {
        maxs.y += move.y;
      }
      if( move.z < 0.0f ) {
        mins.z += move.z;
      }
      else {
        maxs.z += move.z;
      }
    }

    /**
     * Create `Bounds` that cover the trace described by the given `AABB` move.
     */
    OZ_ALWAYS_INLINE
    explicit Bounds( const AABB& a, const Vec3& move, float eps = 0.0f )
    {
      Vec3 epsDim = a.dim + Vec3( eps, eps, eps );

      mins = a.p - epsDim;
      maxs = a.p + epsDim;

      if( move.x < 0.0f ) {
        mins.x += move.x;
      }
      else {
        maxs.x += move.x;
      }
      if( move.y < 0.0f ) {
        mins.y += move.y;
      }
      else {
        maxs.y += move.y;
      }
      if( move.z < 0.0f ) {
        mins.z += move.z;
      }
      else {
        maxs.z += move.z;
      }
    }

    /**
     * Compute centre point.
     */
    Point p() const
    {
      return Point( ( mins.x + maxs.x ) * 0.5f,
                    ( mins.y + maxs.y ) * 0.5f,
                    ( mins.z + maxs.z ) * 0.5f );
    }

    /**
     * Compute extents from centre (same as `dim` member in `AABB`).
     */
    Vec3 dim( float eps = 0.0f ) const
    {
      return Vec3( ( maxs.x - mins.x ) * 0.5f + eps,
                   ( maxs.y - mins.y ) * 0.5f + eps,
                   ( maxs.z - mins.z ) * 0.5f + eps );
    }

    /**
     * Create `AABB` that describes the same set as this `Bounds`.
     */
    OZ_ALWAYS_INLINE
    AABB toAABB( float eps = 0.0f ) const
    {
      return AABB( p(), dim( eps ) );
    }

    /**
     * Translated `Bounds`.
     */
    OZ_ALWAYS_INLINE
    Bounds operator + ( const Vec3& v ) const
    {
      return Bounds( mins + v, maxs + v );
    }

    /**
     * Translated `Bounds`.
     */
    OZ_ALWAYS_INLINE
    Bounds operator - ( const Vec3& v ) const
    {
      return Bounds( mins - v, maxs - v );
    }

    /**
     * Translate `Bounds`.
     */
    OZ_ALWAYS_INLINE
    Bounds& operator += ( const Vec3& v )
    {
      mins += v;
      maxs += v;
      return *this;
    }

    /**
     * Translate `Bounds`.
     */
    OZ_ALWAYS_INLINE
    Bounds& operator -= ( const Vec3& v )
    {
      mins -= v;
      maxs -= v;
      return *this;
    }

    /**
     * True iff the given point is inside this `Bounds`.
     *
     * @param p
     * @param eps margin for which this `Bounds` are enlarged (can also be negative).
     */
    OZ_ALWAYS_INLINE
    bool includes( const Point& p, float eps = 0.0f ) const
    {
      return mins.x - eps <= p.x && p.x <= maxs.x + eps &&
             mins.y - eps <= p.y && p.y <= maxs.y + eps &&
             mins.z - eps <= p.z && p.z <= maxs.z + eps;
    }

    /**
     * True iff the given `AABB` is inside this `Bounds`.
     *
     * @param a
     * @param eps margin for which this `Bounds` are enlarged (can also be negative).
     */
    OZ_ALWAYS_INLINE
    bool includes( const AABB& a, float eps = 0.0f ) const
    {
      Vec3 epsDim = a.dim + Vec3( eps, eps, eps );

      return mins.x <= a.p.x - epsDim.x && a.p.x + epsDim.x <= maxs.x &&
             mins.y <= a.p.y - epsDim.y && a.p.y + epsDim.y <= maxs.y &&
             mins.z <= a.p.z - epsDim.z && a.p.z + epsDim.z <= maxs.z;
    }

    /**
     * True iff the given `AABB` overlaps with this `Bounds`.
     *
     * @param a
     * @param eps margin for which this `Bounds` are enlarged (can also be negative).
     */
    OZ_ALWAYS_INLINE
    bool overlaps( const AABB& a, float eps = 0.0f ) const
    {
      Vec3 epsDim = a.dim + Vec3( eps, eps, eps );

      return mins.x <= a.p.x + epsDim.x && a.p.x - epsDim.x <= maxs.x &&
             mins.y <= a.p.y + epsDim.y && a.p.y - epsDim.y <= maxs.y &&
             mins.z <= a.p.z + epsDim.z && a.p.z - epsDim.z <= maxs.z;
    }

    /**
     * True iff the given `Bounds` is inside this `Bounds`.
     *
     * @param b
     * @param eps margin for which this `Bounds` are enlarged (can also be negative).
     */
    OZ_ALWAYS_INLINE
    bool includes( const Bounds& b, float eps = 0.0f ) const
    {
      return mins.x - eps <= b.mins.x && b.maxs.x <= maxs.x + eps &&
             mins.y - eps <= b.mins.y && b.maxs.y <= maxs.y + eps &&
             mins.z - eps <= b.mins.z && b.maxs.z <= maxs.z + eps;
    }

    /**
     * True iff the given `Bounds` overlaps with this `Bounds`.
     *
     * @param b
     * @param eps margin for which this `Bounds` are enlarged (can also be negative).
     */
    OZ_ALWAYS_INLINE
    bool overlaps( const Bounds& b, float eps = 0.0f ) const
    {
      return mins.x - eps <= b.maxs.x && b.mins.x <= maxs.x + eps &&
             mins.y - eps <= b.maxs.y && b.mins.y <= maxs.y + eps &&
             mins.z - eps <= b.maxs.z && b.mins.z <= maxs.z + eps;
    }

};

}
