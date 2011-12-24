/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2011 Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file oz/Bounds.hh
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

    Point3 mins; ///< Minimums.
    Point3 maxs; ///< Maximums.

    /**
     * Create uninitialised instance.
     */
    Bounds() = default;

    /**
     * Create from the given minimal and maximal point.
     */
    OZ_ALWAYS_INLINE
    explicit Bounds( const Point3& mins_, const Point3& maxs_ ) : mins( mins_ ), maxs( maxs_ )
    {}

    /**
     * Create <tt>%Bounds</tt> enlarged for the given margin <tt>eps</tt>.
     */
    OZ_ALWAYS_INLINE
    explicit Bounds( const Bounds& b, float eps ) :
        mins( b.mins - Vec3( eps, eps, eps ) ), maxs( b.maxs + Vec3( eps, eps, eps ) )
    {}

    /**
     * Create <tt>%Bounds</tt> that describes the same set as the given <tt>AABB</tt>.
     */
    OZ_ALWAYS_INLINE
    explicit Bounds( const AABB& a, float eps = 0.0f )
    {
      Vec3 epsDim = a.dim + Vec3( eps, eps, eps );

      mins = a.p - epsDim;
      maxs = a.p + epsDim;
    }

    /**
     * Create <tt>%Bounds</tt> that cover the trace described by the given point move.
     */
    OZ_ALWAYS_INLINE
    explicit Bounds( const Point3& p, const Vec3& move, float eps = 0.0f )
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
     * Create <tt>%Bounds</tt> that cover the trace described by the given <tt>AABB</tt> move.
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
     * Create <tt>AABB</tt> that describes the same set as this <tt>%Bounds</tt>.
     */
    OZ_ALWAYS_INLINE
    AABB toAABB( float eps = 0.0f ) const
    {
      return AABB( Point3( ( mins.x + maxs.x ) * 0.5f,
                           ( mins.y + maxs.y ) * 0.5f,
                           ( mins.z + maxs.z ) * 0.5f ),
                   Vec3( ( maxs.x - mins.x ) * 0.5f + eps,
                         ( maxs.y - mins.y ) * 0.5f + eps,
                         ( maxs.z - mins.z ) * 0.5f + eps ) );
    }

    /**
     * Translated <tt>%Bounds</tt>.
     */
    OZ_ALWAYS_INLINE
    Bounds operator + ( const Vec3& v ) const
    {
      return Bounds( mins + v, maxs + v );
    }

    /**
     * Translated <tt>%Bounds</tt>.
     */
    OZ_ALWAYS_INLINE
    Bounds operator - ( const Vec3& v ) const
    {
      return Bounds( mins - v, maxs - v );
    }

    /**
     * Translate <tt>%Bounds</tt>.
     */
    OZ_ALWAYS_INLINE
    Bounds& operator += ( const Vec3& v )
    {
      mins += v;
      maxs += v;
      return *this;
    }

    /**
     * Translate <tt>%Bounds</tt>.
     */
    OZ_ALWAYS_INLINE
    Bounds& operator -= ( const Vec3& v )
    {
      mins -= v;
      maxs -= v;
      return *this;
    }

    /**
     * True iff the given point is inside this <tt>%Bounds</tt>.
     *
     * @param p
     * @param eps margin for which this <tt>%Bounds</tt> are enlarged (can also be negative).
     */
    OZ_ALWAYS_INLINE
    bool includes( const Point3& p, float eps = 0.0f ) const
    {
      return
          mins.x - eps <= p.x && p.x <= maxs.x + eps &&
          mins.y - eps <= p.y && p.y <= maxs.y + eps &&
          mins.z - eps <= p.z && p.z <= maxs.z + eps;
    }

    /**
     * True iff the given <tt>AABB</tt> is inside this <tt>%Bounds</tt>.
     *
     * @param a
     * @param eps margin for which this <tt>%Bounds</tt> are enlarged (can also be negative).
     */
    OZ_ALWAYS_INLINE
    bool includes( const AABB& a, float eps = 0.0f ) const
    {
      Vec3 epsDim = a.dim + Vec3( eps, eps, eps );

      return
          mins.x <= a.p.x - epsDim.x && a.p.x + epsDim.x <= maxs.x &&
          mins.y <= a.p.y - epsDim.y && a.p.y + epsDim.y <= maxs.y &&
          mins.z <= a.p.z - epsDim.z && a.p.z + epsDim.z <= maxs.z;
    }

    /**
     * True iff the given <tt>AABB</tt> overlaps with this <tt>%Bounds</tt>.
     *
     * @param a
     * @param eps margin for which this <tt>%Bounds</tt> are enlarged (can also be negative).
     */
    OZ_ALWAYS_INLINE
    bool overlaps( const AABB& a, float eps = 0.0f ) const
    {
      Vec3 epsDim = a.dim + Vec3( eps, eps, eps );

      return
          mins.x <= a.p.x + epsDim.x && a.p.x - epsDim.x <= maxs.x &&
          mins.y <= a.p.y + epsDim.y && a.p.y - epsDim.y <= maxs.y &&
          mins.z <= a.p.z + epsDim.z && a.p.z - epsDim.z <= maxs.z;
    }

    /**
     * True iff the given <tt>%Bounds</tt> is inside this <tt>%Bounds</tt>.
     *
     * @param b
     * @param eps margin for which this <tt>%Bounds</tt> are enlarged (can also be negative).
     */
    OZ_ALWAYS_INLINE
    bool includes( const Bounds& b, float eps = 0.0f ) const
    {
      return
          mins.x - eps <= b.mins.x && b.maxs.x <= maxs.x + eps &&
          mins.y - eps <= b.mins.y && b.maxs.y <= maxs.y + eps &&
          mins.z - eps <= b.mins.z && b.maxs.z <= maxs.z + eps;
    }

    /**
     * True iff the given <tt>%Bounds</tt> overlaps with this <tt>%Bounds</tt>.
     *
     * @param b
     * @param eps margin for which this <tt>%Bounds</tt> are enlarged (can also be negative).
     */
    OZ_ALWAYS_INLINE
    bool overlaps( const Bounds& b, float eps = 0.0f ) const
    {
      return
          mins.x - eps <= b.maxs.x && b.mins.x <= maxs.x + eps &&
          mins.y - eps <= b.maxs.y && b.mins.y <= maxs.y + eps &&
          mins.z - eps <= b.maxs.z && b.mins.z <= maxs.z + eps;
    }

};

}
