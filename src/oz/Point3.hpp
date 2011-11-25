/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file oz/Point3.hpp
 */

#pragma once

#include "Vec3.hpp"

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

    /// Origin, [0, 0, 0] or [0, 0, 0, 1] for SIMD.
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
