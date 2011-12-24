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
 * @file oz/AABB.hh
 */

#pragma once

#include "Point3.hh"

namespace oz
{

/**
 * Axis-aligned bounding box, represented with central point and extents.
 *
 * @ingroup oz
 */
class AABB
{
  public:

    Point3 p;   ///< Position.
    Vec3   dim; ///< Extents.

    /**
     * Create uninitialised instance.
     */
    AABB() = default;

    /**
     * Create <tt>%AABB</tt> with the given centre and extents.
     */
    OZ_ALWAYS_INLINE
    explicit AABB( const Point3& p_, const Vec3& dim_ ) : p( p_ ), dim( dim_ )
    {}

    /**
     * Create <tt>%AABB</tt> enlarged for margin <tt>eps</tt> (can also be negative).
     */
    OZ_ALWAYS_INLINE
    explicit AABB( const AABB& a, float eps ) : p( a.p ), dim( a.dim + Vec3( eps, eps, eps ) )
    {}

    /**
     * Translated <tt>%AABB</tt>.
     */
    OZ_ALWAYS_INLINE
    AABB operator + ( const Vec3& v ) const
    {
      return AABB( p + v, dim );
    }

    /**
     * Translated <tt>%AABB</tt>.
     */
    OZ_ALWAYS_INLINE
    AABB operator - ( const Vec3& v ) const
    {
      return AABB( p - v, dim );
    }

    /**
     * Translate <tt>%AABB</tt>.
     */
    OZ_ALWAYS_INLINE
    AABB& operator += ( const Vec3& v )
    {
      p += v;
      return *this;
    }

    /**
     * Translate <tt>%AABB</tt>.
     */
    OZ_ALWAYS_INLINE
    AABB& operator -= ( const Vec3& v )
    {
      p -= v;
      return *this;
    }

    /**
     * True iff the given point is inside this <tt>%AABB</tt>.
     *
     * @param point
     * @param eps margin for which this <tt>%AABB</tt> is enlarged (can also be negative).
     */
    OZ_ALWAYS_INLINE
    bool includes( const Point3& point, float eps = 0.0f ) const
    {
      Vec3 relPos = ( point - p ).abs();
      Vec3 sumDim = dim + Vec3( eps, eps, eps );

      return relPos.x <= sumDim.x && relPos.y <= sumDim.y && relPos.z <= sumDim.z;
    }

    /**
     * True iff the given <tt>%AABB</tt> is inside this <tt>%AABB</tt>.
     *
     * @param a
     * @param eps margin for which this <tt>%AABB</tt> is enlarged (can also be negative).
     */
    OZ_ALWAYS_INLINE
    bool includes( const AABB& a, float eps = 0.0f ) const
    {
      Vec3 relPos = ( a.p - p ).abs();
      Vec3 sumDim = ( dim - a.dim ).abs() + Vec3( eps, eps, eps );

      return relPos.x <= sumDim.x && relPos.y <= sumDim.y && relPos.z <= sumDim.z;
    }

    /**
     * True iff the given <tt>%AABB</tt> overlaps with this <tt>%AABB</tt>.
     *
     * @param a
     * @param eps margin for which this <tt>%AABB</tt> is enlarged (can also be negative).
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
