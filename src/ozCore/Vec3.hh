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
 * @file ozCore/Vec3.hh
 *
 * `Vec3` class.
 */

#pragma once

#include "simd.hh"
#include "Math.hh"

namespace oz
{

/**
 * 3D vector.
 *
 * @sa `oz::Vec4`, `oz::Point`
 */
class Vec3 : public VectorBase3
{
public:

  /// Zero vector, [0, 0, 0].
  static const Vec3 ZERO;

  /// Vector of ones, [1, 1, 1].
  static const Vec3 ONE;

public:

  /**
   * Create an uninitialised instance.
   */
  OZ_ALWAYS_INLINE
  explicit Vec3() :
    VectorBase3( 0.0f, 0.0f, 0.0f, 0.0f )
  {}

#ifdef OZ_SIMD_MATH

  /**
   * Create from a float SIMD vector.
   */
  OZ_ALWAYS_INLINE
  explicit Vec3( float4 f4 ) :
    VectorBase3( f4 )
  {}

  /**
   * Create from an uint SIMD vector.
   */
  OZ_ALWAYS_INLINE
  explicit Vec3( uint4 u4 ) :
    VectorBase3( u4 )
  {}

#endif

  /**
   * Create a vector with given components.
   */
  OZ_ALWAYS_INLINE
  explicit Vec3( float x, float y, float z ) :
    VectorBase3( x, y, z, 0.0f )
  {}

  /**
   * Create from an array of 3 floats.
   */
  OZ_ALWAYS_INLINE
  explicit Vec3( const float* v ) :
    VectorBase3( v[0], v[1], v[2], 0.0f )
  {}

  /**
   * Equality.
   */
  OZ_ALWAYS_INLINE
  bool operator == ( const Vec3& v ) const
  {
    return x == v.x && y == v.y && z == v.z;
  }

  /**
   * Inequality.
   */
  OZ_ALWAYS_INLINE
  bool operator != ( const Vec3& v ) const
  {
    return x != v.x || y != v.y || z != v.z;
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
   * Norm.
   */
  OZ_ALWAYS_INLINE
  float operator ! () const
  {
#ifdef OZ_SIMD_MATH
    return Math::sqrt( vFirst( vDot( f4, f4 ) ) );
#else
    return Math::sqrt( x*x + y*y + z*z );
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
    return Math::fastSqrt( x*x + y*y + z*z );
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
    return x*x + y*y + z*z;
#endif
  }

  /**
   * Unit vector.
   */
  OZ_ALWAYS_INLINE
  Vec3 operator ~ () const
  {
#ifdef OZ_SIMD_MATH
    scalar s = 1.0f / Math::sqrt( vFirst( vDot( f4, f4 ) ) );
    return Vec3( f4 * s.f4 );
#else
    hard_assert( x*x + y*y + z*z > 0.0f );

    float k = 1.0f / Math::sqrt( x*x + y*y + z*z );
    return Vec3( x * k, y * k, z * k );
#endif
  }

  /**
   * Approximate unit vector.
   */
  OZ_ALWAYS_INLINE
  Vec3 fastUnit() const
  {
#ifdef OZ_SIMD_MATH
    scalar s = Math::fastInvSqrt( vFirst( vDot( f4, f4 ) ) );
    return Vec3( f4 * s.f4 );
#else
    hard_assert( x*x + y*y + z*z > 0.0f );

    float k = Math::fastInvSqrt( x*x + y*y + z*z );
    return Vec3( x * k, y * k, z * k );
#endif
  }

  /**
   * Original vector.
   */
  OZ_ALWAYS_INLINE
  Vec3 operator + () const
  {
    return *this;
  }

  /**
   * Opposite vector.
   */
  OZ_ALWAYS_INLINE
  Vec3 operator - () const
  {
#ifdef OZ_SIMD_MATH
    return Vec3( -f4 );
#else
    return Vec3( -x, -y, -z );
#endif
  }

  /**
   * Sum.
   */
  OZ_ALWAYS_INLINE
  Vec3 operator + ( const Vec3& v ) const
  {
#ifdef OZ_SIMD_MATH
    return Vec3( f4 + v.f4 );
#else
    return Vec3( x + v.x, y + v.y, z + v.z );
#endif
  }

  /**
   * Difference.
   */
  OZ_ALWAYS_INLINE
  Vec3 operator - ( const Vec3& v ) const
  {
#ifdef OZ_SIMD_MATH
    return Vec3( f4 - v.f4 );
#else
    return Vec3( x - v.x, y - v.y, z - v.z );
#endif
  }

  /**
   * Vector multiplied by a scalar.
   */
  OZ_ALWAYS_INLINE
  Vec3 operator * ( scalar s ) const
  {
#ifdef OZ_SIMD_MATH
    return Vec3( f4 * s.f4 );
#else
    return Vec3( x * s, y * s, z * s );
#endif
  }

  /**
   * Vector multiplied by a scalar.
   */
  OZ_ALWAYS_INLINE
  friend Vec3 operator * ( scalar s, const Vec3& v )
  {
#ifdef OZ_SIMD_MATH
    return Vec3( s.f4 * v.f4 );
#else
    return Vec3( s * v.x, s * v.y, s * v.z );
#endif
  }

  /**
   * Vector divided by a scalar.
   */
  OZ_ALWAYS_INLINE
  Vec3 operator / ( scalar s ) const
  {
#ifdef OZ_SIMD_MATH
    return Vec3( f4 / s.f4 );
#else
    hard_assert( s != 0.0f );

    s = 1.0f / s;
    return Vec3( x * s, y * s, z * s );
#endif
  }

  /**
   * Addition.
   */
  OZ_ALWAYS_INLINE
  Vec3& operator += ( const Vec3& v )
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
   * Subtraction.
   */
  OZ_ALWAYS_INLINE
  Vec3& operator -= ( const Vec3& v )
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
   * Multiplication by a scalar.
   */
  OZ_ALWAYS_INLINE
  Vec3& operator *= ( scalar s )
  {
#ifdef OZ_SIMD_MATH
    f4 *= s.f4;
#else
    x *= s;
    y *= s;
    z *= s;
#endif
    return *this;
  }

  /**
   * Division by a scalar.
   */
  OZ_ALWAYS_INLINE
  Vec3& operator /= ( scalar s )
  {
#ifdef OZ_SIMD_MATH
    f4 /= s.f4;
#else
    hard_assert( s != 0.0f );

    s  = 1.0f / s;
    x *= s;
    y *= s;
    z *= s;
#endif
    return *this;
  }

  /**
   * Scalar product.
   */
  OZ_ALWAYS_INLINE
  scalar operator * ( const Vec3& v ) const
  {
#ifdef OZ_SIMD_MATH
    return vDot( f4, v.f4 );
#else
    return x*v.x + y*v.y + z*v.z;
#endif
  }

  /**
   * Vector product.
   */
  OZ_ALWAYS_INLINE
  Vec3 operator ^ ( const Vec3& v ) const
  {
#ifdef OZ_SIMD_MATH
    float4 a  = vShuffle( f4, f4, 1, 2, 0, 3 );
    float4 b  = vShuffle( f4, f4, 2, 0, 1, 3 );
    float4 va = vShuffle( v.f4, v.f4, 2, 0, 1, 3 );
    float4 vb = vShuffle( v.f4, v.f4, 1, 2, 0, 3 );

    return Vec3( a*va - b*vb );
#else
    return Vec3( y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x );
#endif
  }

  /**
   * Spherical linear interpolation.
   */
  OZ_ALWAYS_INLINE
  static Vec3 slerp( const Vec3& a, const Vec3& b, float t )
  {
#ifdef OZ_SIMD_MATH
    float angle = Math::acos( vFirst( vDot( a.f4, b.f4 ) ) );
#else
    float angle = Math::acos( a.x*b.x + a.y*b.y + a.z*b.z );
#endif
    float sine  = Math::sin( angle );

    if( sine == 0.0f ) {
      hard_assert( a * b > 0.0f );

      return a;
    }
    else {
      float alpha = Math::sin( ( 1.0f - t ) * angle ) / sine;
      float beta  = Math::sin( t * angle ) / sine;

      return alpha * a + beta * b;
    }
  }

};

/**
 * Per-component absolute value of a vector.
 */
OZ_ALWAYS_INLINE
inline Vec3 abs( const Vec3& a )
{
#ifdef OZ_SIMD_MATH
  return Vec3( vAbs( a.u4 ) );
#else
  return Vec3( abs( a.x ), abs( a.y ), abs( a.z ) );
#endif
}

/**
 * Per-component minimum of two vectors.
 */
OZ_ALWAYS_INLINE
inline Vec3 min( const Vec3& a, const Vec3& b )
{
#ifdef OZ_SIMD_MATH
  return Vec3( vMin( a.f4, b.f4 ) );
#else
  return Vec3( min( a.x, b.x ), min( a.y, b.y ), min( a.z, b.z ) );
#endif
}

/**
 * Per-component maximum of two vectors.
 */
OZ_ALWAYS_INLINE
inline Vec3 max( const Vec3& a, const Vec3& b )
{
#ifdef OZ_SIMD_MATH
  return Vec3( vMax( a.f4, b.f4 ) );
#else
  return Vec3( max( a.x, b.x ), max( a.y, b.y ), max( a.z, b.z ) );
#endif
}

/**
 * Per-component clamped value of vectors.
 */
OZ_ALWAYS_INLINE
inline Vec3 clamp( const Vec3& c, const Vec3& a, const Vec3& b )
{
#ifdef OZ_SIMD_MATH
  return Vec3( vMin( b.f4, vMax( a.f4, c.f4 ) ) );
#else
  return Vec3( clamp( c.x, a.x, b.x ), clamp( c.y, a.y, b.y ), clamp( c.z, a.z, b.z ) );
#endif
}

}
