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
 * @file oz/Point.hh
 *
 * Point class.
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
class Point
{
  public:

    /// Origin, [0, 0, 0].
    static const Point ORIGIN;

    float x; ///< X component.
    float y; ///< Y component.
    float z; ///< Z component.

    /**
     * Create an uninitialised instance.
     */
    Point() = default;

    /**
     * Create a point with the given coordinates.
     */
    OZ_ALWAYS_INLINE
    explicit Point( float x_, float y_, float z_ ) :
      x( x_ ), y( y_ ), z( z_ )
    {}

    /**
     * Create from an array of 3 floats.
     */
    OZ_ALWAYS_INLINE
    explicit Point( const float* v ) :
      x( v[0] ), y( v[1] ), z( v[2] )
    {}

    /**
     * Equality.
     */
    OZ_ALWAYS_INLINE
    bool operator == ( const Point& p ) const
    {
      return x == p.x && y == p.y && z == p.z;
    }

    /**
     * Inequality.
     */
    OZ_ALWAYS_INLINE
    bool operator != ( const Point& p ) const
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
    Point operator + ( const Vec3& v ) const
    {
      return Point( x + v.x, y + v.y, z + v.z );
    }

    /**
     * Point translated for <tt>-v</tt>.
     */
    OZ_ALWAYS_INLINE
    Point operator - ( const Vec3& v ) const
    {
      return Point( x - v.x, y - v.y, z - v.z );
    }

    /**
     * Difference of two points.
     */
    OZ_ALWAYS_INLINE
    Vec3 operator - ( const Point& p ) const
    {
      return Vec3( x - p.x, y - p.y, z - p.z );
    }

    /**
     * Translate for <tt>v</tt>.
     */
    OZ_ALWAYS_INLINE
    Point& operator += ( const Vec3& v )
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
    Point& operator -= ( const Vec3& v )
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
