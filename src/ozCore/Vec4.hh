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
 * @file ozCore/Vec4.hh
 *
 * `Vec4` class.
 */

#pragma once

#include "Point.hh"

namespace oz
{

/**
 * Generic four-component vector.
 *
 * @sa `oz::Vec3`
 */
class Vec4 : public VectorBase4
{
  public:

    /// Zero vector, [0, 0, 0, 0].
    static const Vec4 ZERO;

    /// Vector of ones, [1, 1, 1, 1].
    static const Vec4 ONE;

    /// Equivalent to origin point or identity quaternion, [0, 0, 0, 1].
    static const Vec4 ID;

  public:

    /**
     * Create an uninitialised instance.
     */
    OZ_ALWAYS_INLINE
    explicit Vec4() = default;

#ifdef OZ_SIMD_MATH

    /**
     * Create from a float SIMD vector.
     */
    OZ_ALWAYS_INLINE
    explicit Vec4( float4 f4 ) :
      VectorBase4( f4 )
    {}

    /**
     * Create from an uint SIMD vector.
     */
    OZ_ALWAYS_INLINE
    explicit Vec4( uint4 u4 ) :
      VectorBase4( u4 )
    {}

#endif

    /**
     * Create a vector with given components.
     */
    OZ_ALWAYS_INLINE
    explicit Vec4( float x, float y, float z, float w ) :
      VectorBase4( x, y, z, w )
    {}

    /**
     * Create from an array of 4 floats.
     */
    OZ_ALWAYS_INLINE
    explicit Vec4( const float* v ) :
      VectorBase4( v[0], v[1], v[2], v[3] )
    {}

    /**
     * Create vector from a 3D vector (the additional component is zero).
     */
    OZ_ALWAYS_INLINE
    explicit Vec4( const Vec3& v ) :
#ifdef OZ_SIMD_MATH
      VectorBase4( v.f4 )
#else
      VectorBase4( v.x, v.y, v.z, 0.0f )
#endif
    {}

    /**
     * Create vector from a point (the additional component is one).
     */
    OZ_ALWAYS_INLINE
    explicit Vec4( const Point& p ) :
#ifdef OZ_SIMD_MATH
      VectorBase4( p.f4 )
#else
      VectorBase4( p.x, p.y, p.z, 1.0f )
#endif
    {}

    /**
     * Equality.
     */
    OZ_ALWAYS_INLINE
    bool operator == ( const Vec4& v ) const
    {
      return x == v.x && y == v.y && z == v.z && w == v.w;
    }

    /**
     * Inequality.
     */
    OZ_ALWAYS_INLINE
    bool operator != ( const Vec4& v ) const
    {
      return x != v.x || y != v.y || z != v.z || w != v.w;
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
     * Return the 3D vector this vector represents (last component should be 0).
     */
    Vec3 vec3() const
    {
#ifdef OZ_SIMD_MATH
      return Vec3( f4 );
#else
      hard_assert( w == 0.0f );

      return Vec3( x, y, z );
#endif
    }

    /**
     * Return the point vector represents (last component should be 1).
     */
    Point point() const
    {
#ifdef OZ_SIMD_MATH
      return Point( f4 );
#else
      hard_assert( w == 1.0f );

      return Point( x, y, z );
#endif
    }

    /**
     * Norm.
     */
    OZ_ALWAYS_INLINE
    float operator ! () const
    {
#ifdef OZ_SIMD_MATH
      return Math::sqrt( vFirst( vDot( f4, f4 ) ) );
#else
      return Math::sqrt( x*x + y*y + z*z + w*w );
#endif
    }

    /**
     * Approximate norm.
     */
    OZ_ALWAYS_INLINE
    float fastN() const
    {
#ifdef OZ_SIMD_MATH
      return Math::fastSqrt( vFirst( vDot( f4, f4 ) ) );
#else
      return Math::fastSqrt( x*x + y*y + z*z + w*w );
#endif
    }

    /**
     * Squared norm.
     */
    OZ_ALWAYS_INLINE
    scalar sqN() const
    {
#ifdef OZ_SIMD_MATH
      return vDot( f4, f4 );
#else
      return x*x + y*y + z*z + w*w;
#endif
    }

    /**
     * Unit vector.
     */
    OZ_ALWAYS_INLINE
    Vec4 operator ~ () const
    {
#ifdef OZ_SIMD_MATH
      scalar s = 1.0f / Math::sqrt( vFirst( vDot( f4, f4 ) ) );
      return Vec4( f4 * s.f4 );
#else
      hard_assert( x*x + y*y + z*z + w*w > 0.0f );

      float k = 1.0f / Math::sqrt( x*x + y*y + z*z + w*w );
      return Vec4( x * k, y * k, z * k, w * k );
#endif
    }

    /**
     * Approximate unit vector.
     */
    OZ_ALWAYS_INLINE
    Vec4 fastUnit() const
    {
#ifdef OZ_SIMD_MATH
      scalar s = Math::fastInvSqrt( vFirst( vDot( f4, f4 ) ) );
      return Vec4( f4 * s.f4 );
#else
      hard_assert( x*x + y*y + z*z + w*w > 0.0f );

      float k = Math::fastInvSqrt( x*x + y*y + z*z + w*w );
      return Vec4( x * k, y * k, z * k, w * k );
#endif
    }

    /**
     * Original vector.
     */
    OZ_ALWAYS_INLINE
    Vec4 operator + () const
    {
      return *this;
    }

    /**
     * Opposite vector.
     */
    OZ_ALWAYS_INLINE
    Vec4 operator - () const
    {
#ifdef OZ_SIMD_MATH
      return Vec4( -f4 );
#else
      return Vec4( -x, -y, -z, -w );
#endif
    }

    /**
     * Sum.
     */
    OZ_ALWAYS_INLINE
    Vec4 operator + ( const Vec4& v ) const
    {
#ifdef OZ_SIMD_MATH
      return Vec4( f4 + v.f4 );
#else
      return Vec4( x + v.x, y + v.y, z + v.z, w + v.w );
#endif
    }

    /**
     * Difference.
     */
    OZ_ALWAYS_INLINE
    Vec4 operator - ( const Vec4& v ) const
    {
#ifdef OZ_SIMD_MATH
      return Vec4( f4 - v.f4 );
#else
      return Vec4( x - v.x, y - v.y, z - v.z, w - v.w );
#endif
    }

    /**
     * Vector multiplied by a scalar.
     */
    OZ_ALWAYS_INLINE
    Vec4 operator * ( scalar s ) const
    {
#ifdef OZ_SIMD_MATH
      return Vec4( f4 * s.f4 );
#else
      return Vec4( x * s, y * s, z * s, w * s );
#endif
    }

    /**
     * Vector multiplied by a scalar.
     */
    OZ_ALWAYS_INLINE
    friend Vec4 operator * ( scalar s, const Vec4& v )
    {
#ifdef OZ_SIMD_MATH
      return Vec4( s.f4 * v.f4 );
#else
      return Vec4( s * v.x, s * v.y, s * v.z, s * v.w );
#endif
    }

    /**
     * Vector divided by a scalar.
     */
    OZ_ALWAYS_INLINE
    Vec4 operator / ( scalar s ) const
    {
#ifdef OZ_SIMD_MATH
      return Vec4( f4 / s.f4 );
#else
      hard_assert( s != 0.0f );

      s = 1.0f / s;
      return Vec4( x * s, y * s, z * s, w * s );
#endif
    }

    /**
     * Addition.
     */
    OZ_ALWAYS_INLINE
    Vec4& operator += ( const Vec4& v )
    {
#ifdef OZ_SIMD_MATH
      f4 += v.f4;
#else
      x += v.x;
      y += v.y;
      z += v.z;
      w += v.w;
#endif
      return *this;
    }

    /**
     * Subtraction.
     */
    OZ_ALWAYS_INLINE
    Vec4& operator -= ( const Vec4& v )
    {
#ifdef OZ_SIMD_MATH
      f4 -= v.f4;
#else
      x -= v.x;
      y -= v.y;
      z -= v.z;
      w -= v.w;
#endif
      return *this;
    }

    /**
     * Multiplication by a scalar.
     */
    OZ_ALWAYS_INLINE
    Vec4& operator *= ( scalar s )
    {
#ifdef OZ_SIMD_MATH
      f4 *= s.f4;
#else
      x *= s;
      y *= s;
      z *= s;
      w *= s;
#endif
      return *this;
    }

    /**
     * Division by a scalar.
     */
    OZ_ALWAYS_INLINE
    Vec4& operator /= ( scalar s )
    {
#ifdef OZ_SIMD_MATH
      f4 /= s.f4;
#else
      hard_assert( s != 0.0f );

      s  = 1.0f / s;
      x *= s;
      y *= s;
      z *= s;
      w *= s;
#endif
      return *this;
    }

    /**
     * Scalar product.
     */
    OZ_ALWAYS_INLINE
    scalar operator * ( const Vec4& v ) const
    {
#ifdef OZ_SIMD_MATH
      return vDot( f4, v.f4 );
#else
      return x*v.x + y*v.y + z*v.z + w*v.w;
#endif
    }

};

/**
 * Per-component absolute value of a vector.
 */
OZ_ALWAYS_INLINE
inline Vec4 abs( const Vec4& a )
{
#ifdef OZ_SIMD_MATH
  return Vec4( vAbs( a.u4 ) );
#else
  return Vec4( abs( a.x ), abs( a.y ), abs( a.z ), abs( a.w ) );
#endif
}

/**
 * Per-component minimum of two vectors.
 */
OZ_ALWAYS_INLINE
inline Vec4 min( const Vec4& a, const Vec4& b )
{
#ifdef OZ_SIMD_MATH
  return Vec4( vMin( a.f4, b.f4 ) );
#else
  return Vec4( min( a.x, b.x ), min( a.y, b.y ), min( a.z, b.z ), min( a.w, b.w ) );
#endif
}

/**
 * Per-component maximum of two vectors.
 */
OZ_ALWAYS_INLINE
inline Vec4 max( const Vec4& a, const Vec4& b )
{
#ifdef OZ_SIMD_MATH
  return Vec4( vMax( a.f4, b.f4 ) );
#else
  return Vec4( max( a.x, b.x ), max( a.y, b.y ), max( a.z, b.z ), max( a.w, b.w ) );
#endif
}

/**
 * Per-component clamped value of vectors.
 */
OZ_ALWAYS_INLINE
inline Vec4 clamp( const Vec4& c, const Vec4& a, const Vec4& b )
{
#ifdef OZ_SIMD_MATH
  return Vec4( vMin( b.f4, vMax( a.f4, c.f4 ) ) );
#else
  return Vec4( clamp( c.x, a.x, b.x ), clamp( c.y, a.y, b.y ), clamp( c.z, a.z, b.z ),
               clamp( c.w, a.w, b.w ) );
#endif
}

}
