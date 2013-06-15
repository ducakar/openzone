/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file ozCore/Point.hh
 *
 * `Point` class.
 */

#pragma once

#include "Vec3.hh"

namespace oz
{

/**
 * 3D point.
 *
 * @sa `oz::Vec3`
 */
class Point : public VectorBase3
{
  public:

    /// Origin, [0, 0, 0].
    static const Point ORIGIN;

  public:

    /**
     * Create an uninitialised instance.
     */
    OZ_ALWAYS_INLINE
    explicit Point() = default;

#ifdef OZ_SIMD_MATH

    /**
     * Create form a float SIMD vector.
     */
    OZ_ALWAYS_INLINE
    explicit Point( float4 f4 ) :
      VectorBase3( f4 )
    {}

    /**
     * Create from an uint SIMD vector.
     */
    OZ_ALWAYS_INLINE
    explicit Point( uint4 u4 ) :
      VectorBase3( u4 )
    {}

#endif

    /**
     * Create a point with given coordinates.
     */
    OZ_ALWAYS_INLINE
    explicit Point( float x, float y, float z ) :
      VectorBase3( x, y, z, 1.0f )
    {}

    /**
     * Create from an array of 3 floats.
     */
    OZ_ALWAYS_INLINE
    explicit Point( const float* v ) :
      VectorBase3( v[0], v[1], v[2], 1.0f )
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
     * Constant reference to the `i`-th member.
     */
    OZ_ALWAYS_INLINE
    const float& operator [] ( int i ) const
    {
      return ( &x )[i];
    }

    /**
     * Reference to the `i`-th member.
     */
    OZ_ALWAYS_INLINE
    float& operator [] ( int i )
    {
      return ( &x )[i];
    }

    /**
     * Point translated for `v`.
     */
    OZ_ALWAYS_INLINE
    Point operator + ( const Vec3& v ) const
    {
#ifdef OZ_SIMD_MATH
      return Point( f4 + v.f4 );
#else
      return Point( x + v.x, y + v.y, z + v.z );
#endif
    }

    /**
     * Point translated for `-v`.
     */
    OZ_ALWAYS_INLINE
    Point operator - ( const Vec3& v ) const
    {
#ifdef OZ_SIMD_MATH
      return Point( f4 - v.f4 );
#else
      return Point( x - v.x, y - v.y, z - v.z );
#endif
    }

    /**
     * Difference of two points.
     */
    OZ_ALWAYS_INLINE
    Vec3 operator - ( const Point& p ) const
    {
#ifdef OZ_SIMD_MATH
      return Vec3( f4 - p.f4 );
#else
      return Vec3( x - p.x, y - p.y, z - p.z );
#endif
    }

    /**
     * Translate for `v`.
     */
    OZ_ALWAYS_INLINE
    Point& operator += ( const Vec3& v )
    {
#ifdef OZ_SIMD_MATH
      f4 += v.f4;
#else
      x += v.x;
      y += v.y;
      z += v.z;
#endif
      return *this;
    }

    /**
     * Translate for `-v`.
     */
    OZ_ALWAYS_INLINE
    Point& operator -= ( const Vec3& v )
    {
#ifdef OZ_SIMD_MATH
      f4 -= v.f4;
#else
      x -= v.x;
      y -= v.y;
      z -= v.z;
#endif
      return *this;
    }

    /**
     * Projection of the point to a given vector.
     */
    OZ_ALWAYS_INLINE
#ifdef OZ_SIMD_MATH
    scalar operator * ( const Vec3& v ) const
    {
      return vDot( f4, v.f4 );
    }
#else
    float operator * ( const Vec3& v ) const
    {
      return x*v.x + y*v.y + z*v.z;
    }
#endif

};

/**
 * Per-component absolute value of a vector.
 */
OZ_ALWAYS_INLINE
inline Point abs( const Point& a )
{
#ifdef OZ_SIMD_MATH
  return Point( vAbs( a.u4 ) );
#else
  return Point( abs( a.x ), abs( a.y ), abs( a.z ) );
#endif
}

/**
 * Per-component minimum of two vectors.
 */
OZ_ALWAYS_INLINE
inline Point min( const Point& a, const Point& b )
{
#ifdef OZ_SIMD_MATH
  return Point( vMin( a.f4, b.f4 ) );
#else
  return Point( min( a.x, b.x ), min( a.y, b.y ), min( a.z, b.z ) );
#endif
}

/**
 * Per-component maximum of two vectors.
 */
OZ_ALWAYS_INLINE
inline Point max( const Point& a, const Point& b )
{
#ifdef OZ_SIMD_MATH
  return Point( vMax( a.f4, b.f4 ) );
#else
  return Point( max( a.x, b.x ), max( a.y, b.y ), max( a.z, b.z ) );
#endif
}

/**
 * Per-component clamped value of vectors.
 */
OZ_ALWAYS_INLINE
inline Point clamp( const Point& c, const Point& a, const Point& b )
{
#ifdef OZ_SIMD_MATH
  return Point( vMin( b.f4, vMax( a.f4, c.f4 ) ) );
#else
  return Point( clamp( c.x, a.x, b.x ), clamp( c.y, a.y, b.y ), clamp( c.z, a.z, b.z ) );
#endif
}

}