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
 * @file oz/Point3.hh
 */

#pragma once

#include "Vec3.hh"

namespace oz
{

/**
 * 3D point.
 *
 * @ingroup oz
 */
class Point3
{
  public:

    /// Origin, [0, 0, 0].
    static const Point3 ORIGIN;

    float x; ///< X component.
    float y; ///< Y component.
    float z; ///< Z component.

    /**
     * Create an uninitialised instance.
     */
    Point3() = default;

    /**
     * Create a point with the given coordinates.
     */
    OZ_ALWAYS_INLINE
    explicit Point3( float x_, float y_, float z_ ) : x( x_ ), y( y_ ), z( z_ )
    {}

    /**
     * Create from an array of 3 floats.
     */
    OZ_ALWAYS_INLINE
    explicit Point3( const float* v ) : x( v[0] ), y( v[1] ), z( v[2] )
    {}

    /**
     * Equality.
     */
    OZ_ALWAYS_INLINE
    bool operator == ( const Point3& p ) const
    {
      return x == p.x && y == p.y && z == p.z;
    }

    /**
     * Inequality.
     */
    OZ_ALWAYS_INLINE
    bool operator != ( const Point3& p ) const
    {
      return x != p.x || y != p.y || z != p.z;
    }

    /**
     * Constant float pointer to the members.
     */
    OZ_ALWAYS_INLINE
    operator const float* () const
    {
      return &x;
    }

    /**
     * Float pointer to the members.
     */
    OZ_ALWAYS_INLINE
    operator float* ()
    {
      return &x;
    }

    /**
     * Constant reference to the i-th member.
     */
    OZ_ALWAYS_INLINE
    const float& operator [] ( int i ) const
    {
      hard_assert( 0 <= i && i < 3 );

      return ( &x )[i];
    }

    /**
     * Reference to the i-th member.
     */
    OZ_ALWAYS_INLINE
    float& operator [] ( int i )
    {
      hard_assert( 0 <= i && i < 3 );

      return ( &x )[i];
    }

    /**
     * Point translated for <tt>v</tt>.
     */
    OZ_ALWAYS_INLINE
    Point3 operator + ( const Vec3& v ) const
    {
      return Point3( x + v.x, y + v.y, z + v.z );
    }

    /**
     * Point translated for <tt>-v</tt>.
     */
    OZ_ALWAYS_INLINE
    Point3 operator - ( const Vec3& v ) const
    {
      return Point3( x - v.x, y - v.y, z - v.z );
    }

    /**
     * Difference of two points.
     */
    OZ_ALWAYS_INLINE
    Vec3 operator - ( const Point3& p ) const
    {
      return Vec3( x - p.x, y - p.y, z - p.z );
    }

    /**
     * Translate for <tt>v</tt>.
     */
    OZ_ALWAYS_INLINE
    Point3& operator += ( const Vec3& v )
    {
      x += v.x;
      y += v.y;
      z += v.z;
      return *this;
    }

    /**
     * Translate for <tt>-v</tt>.
     */
    OZ_ALWAYS_INLINE
    Point3& operator -= ( const Vec3& v )
    {
      x -= v.x;
      y -= v.y;
      z -= v.z;
      return *this;
    }

    /**
     * Projection of the point to the given vector.
     */
    OZ_ALWAYS_INLINE
    float operator * ( const Vec3& v ) const
    {
      return x*v.x + y*v.y + z*v.z;
    }

};

}
