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
 * @file oz/Plane.hh
 */

#pragma once

#include "Point3.hh"

namespace oz
{

/**
 * 3D plane.
 *
 * @ingroup oz
 */
class Plane
{
  public:

    Vec3  n; ///< Normal.
    float d; ///< Distance from origin.

    /**
     * Create an uninitialised instance.
     */
    Plane() = default;

    /**
     * Create form a pair of normal and distance from the origin.
     */
    OZ_ALWAYS_INLINE
    explicit Plane( const Vec3& n_, float d_ ) : n( n_ ), d( d_ )
    {}

    /**
     * Create from an array of 4 floats.
     */
    OZ_ALWAYS_INLINE
    explicit Plane( const float* p ) : n( p[0], p[1], p[2] ), d( p[3] )
    {}

    /**
     * Create with the given member values.
     */
    OZ_ALWAYS_INLINE
    explicit Plane( float nx, float ny, float nz, float d_ ) : n( nx, ny, nz ), d( d_ )
    {}

    /**
     * Equality.
     */
    OZ_ALWAYS_INLINE
    bool operator == ( const Plane& p ) const
    {
      return n.x == p.n.x && n.y == p.n.y && n.z == p.n.z && d == p.d;
    }

    /**
     * Inequality.
     */
    OZ_ALWAYS_INLINE
    bool operator != ( const Plane& p ) const
    {
      return n.x != p.n.x || n.y != p.n.y || n.z != p.n.z || d != p.d;
    }

    /**
     * Constant float pointer to the members.
     */
    OZ_ALWAYS_INLINE
    operator const float* () const
    {
      return &n.x;
    }

    /**
     * Float pointer to the members.
     */
    OZ_ALWAYS_INLINE
    operator float* ()
    {
      return &n.x;
    }

    /**
     * Constant reference to the i-th member.
     */
    OZ_ALWAYS_INLINE
    const float& operator [] ( int i ) const
    {
      hard_assert( 0 <= i && i < 4 );

      return ( &n.x )[i];
    }

    /**
     * Reference to the i-th member.
     */
    OZ_ALWAYS_INLINE
    float& operator [] ( int i )
    {
      hard_assert( 0 <= i && i < 4 );

      return ( &n.x )[i];
    }

    /**
     * %Plane with the same distance but normal that has absolute components.
     */
    OZ_ALWAYS_INLINE
    Plane abs() const
    {
      return Plane( Math::fabs( n.x ), Math::fabs( n.y ), Math::fabs( n.z ), d );
    }

    /**
     * Projection of a vector to the plane's normal.
     */
    OZ_ALWAYS_INLINE
    friend float operator * ( const Vec3& v, const Plane& plane )
    {
      return v.x*plane.n.x + v.y*plane.n.y + v.z*plane.n.z;
    }

    /**
     * Distance between a point and the plane.
     */
    OZ_ALWAYS_INLINE
    friend float operator * ( const Point3& p, const Plane& plane )
    {
      return p.x*plane.n.x + p.y*plane.n.y + p.z*plane.n.z - plane.d;
    }

};

}
