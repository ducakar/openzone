/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file ozCore/Quat.hh
 *
 * `Quat` class.
 */

#pragma once

#include "Vec4.hh"

namespace oz
{

/**
 * Quaternion.
 */
class Quat : public VectorBase4
{
public:

  /// Zero quaternion.
  static const Quat ZERO;

  /// Quaternion representing rotation identity.
  static const Quat ID;

public:

  /**
   * Create an uninitialised instance.
   */
  OZ_ALWAYS_INLINE
  explicit Quat() :
    VectorBase4( 0.0f, 0.0f, 0.0f, 1.0f )
  {}

#ifdef OZ_SIMD_MATH

  /**
   * Create from a float SIMD vector.
   */
  OZ_ALWAYS_INLINE
  explicit Quat( float4 f4 ) :
    VectorBase4( f4 )
  {}

  /**
   * Create from an uint SIMD vector.
   */
  OZ_ALWAYS_INLINE
  explicit Quat( uint4 u4 ) :
    VectorBase4( u4 )
  {}

#endif

  /**
   * Create a quaternion with given components.
   */
  OZ_ALWAYS_INLINE
  explicit Quat( float x, float y, float z, float w ) :
    VectorBase4( x, y, z, w )
  {}

  /**
   * Create from an array of 4 floats.
   */
  OZ_ALWAYS_INLINE
  explicit Quat( const float* q ) :
    VectorBase4( q[0], q[1], q[2], q[3] )
  {}

  /**
   * Equality.
   */
  OZ_ALWAYS_INLINE
  bool operator == ( const Quat& v ) const
  {
    return x == v.x && y == v.y && z == v.z && w == v.w;
  }

  /**
   * Inequality.
   */
  OZ_ALWAYS_INLINE
  bool operator != ( const Quat& v ) const
  {
    return x != v.x || y != v.y || z != v.z || w != v.w;
  }

  /**
   * Cast to four-component vector.
   */
  operator Vec4 () const
  {
#ifdef OZ_SIMD_MATH
    return Vec4( f4 );
#else
    return Vec4( x, y, z, w );
#endif
  }

  /**
   * Conjugated quaternion.
   */
  OZ_ALWAYS_INLINE
  Quat operator * () const
  {
#ifdef OZ_SIMD_MATH
    return Quat( u4 ^ vFill( 0x80000000u, 0x80000000u, 0x80000000u, 0u ) );
#else
    return Quat( -x, -y, -z, w );
#endif
  }

  /**
   * Norm.
   */
  OZ_ALWAYS_INLINE
  scalar operator ! () const
  {
#ifdef OZ_SIMD_MATH
    return vDot( f4, f4 );
#else
    return x*x + y*y + z*z + w*w;
#endif
  }

  /**
   * Unit quaternion.
   */
  Quat operator ~ () const;

  /**
   * Approximate unit quaternion.
   */
  Quat fastUnit() const
  {
#ifdef OZ_SIMD_MATH
    scalar s = Math::fastInvSqrt( vFirst( vDot( f4, f4 ) ) );
    return Quat( f4 * s.f4 );
#else
    hard_assert( x*x + y*y + z*z + w*w > 0.0f );

    float k = Math::fastInvSqrt( x*x + y*y + z*z + w*w );
    return Quat( x * k, y * k, z * k, w * k );
#endif
  }

  /**
   * Original quaternion.
   */
  OZ_ALWAYS_INLINE
  Quat operator + () const
  {
    return *this;
  }

  /**
   * Opposite quaternion.
   */
  OZ_ALWAYS_INLINE
  Quat operator - () const
  {
#ifdef OZ_SIMD_MATH
    return Quat( -f4 );
#else
    return Quat( -x, -y, -z, -w );
#endif
  }

  /**
   * Sum.
   */
  OZ_ALWAYS_INLINE
  Quat operator + ( const Quat& q ) const
  {
#ifdef OZ_SIMD_MATH
    return Quat( f4 + q.f4 );
#else
    return Quat( x + q.x, y + q.y, z + q.z, w + q.w );
#endif
  }

  /**
   * Difference.
   */
  OZ_ALWAYS_INLINE
  Quat operator - ( const Quat& q ) const
  {
#ifdef OZ_SIMD_MATH
    return Quat( f4 - q.f4 );
#else
    return Quat( x - q.x, y - q.y, z - q.z, w - q.w );
#endif
  }

  /**
   * Product.
   */
  OZ_ALWAYS_INLINE
  Quat operator * ( scalar s ) const
  {
#ifdef OZ_SIMD_MATH
    return Quat( f4 * s.f4 );
#else
    return Quat( x * s, y * s, z * s, w * s );
#endif
  }

  /**
   * Product.
   */
  OZ_ALWAYS_INLINE
  friend Quat operator * ( scalar s, const Quat& q )
  {
#ifdef OZ_SIMD_MATH
    return Quat( s.f4 * q.f4 );
#else
    return Quat( s * q.x, s * q.y, s * q.z, s * q.w );
#endif
  }

  /**
   * Quaternion product.
   */
  Quat operator * ( const Quat& q ) const;

  /**
   * Quotient.
   */
  OZ_ALWAYS_INLINE
  Quat operator / ( scalar s ) const
  {
    hard_assert( s != 0.0f );

#ifdef OZ_SIMD_MATH
    return Quat( f4 / s.f4 );
#else
    s = 1.0f / s;
    return Quat( x * s, y * s, z * s, w * s );
#endif
  }

  /**
   * Quotient.
   */
  friend Quat operator / ( scalar s, const Quat& q );

  /**
   * Quaternion quotient.
   */
  Quat operator / ( const Quat& q ) const;

  /**
   * Addition.
   */
  OZ_ALWAYS_INLINE
  Quat& operator += ( const Quat& q )
  {
#ifdef OZ_SIMD_MATH
    f4 += q.f4;
#else
    x += q.x;
    y += q.y;
    z += q.z;
    w += q.w;
#endif
    return *this;
  }

  /**
   * Subtraction.
   */
  OZ_ALWAYS_INLINE
  Quat& operator -= ( const Quat& q )
  {
#ifdef OZ_SIMD_MATH
    f4 -= q.f4;
#else
    x -= q.x;
    y -= q.y;
    z -= q.z;
    w -= q.w;
#endif
    return *this;
  }

  /**
   * Multiplication.
   */
  OZ_ALWAYS_INLINE
  Quat& operator *= ( scalar s )
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
   * Quaternion multiplication.
   */
  Quat& operator *= ( const Quat& q );

  /**
   * Division.
   */
  OZ_ALWAYS_INLINE
  Quat& operator /= ( scalar s )
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
   * Quaternion division.
   */
  Quat& operator /= ( const Quat& q );

  /**
   * Vector rotation.
   */
  Vec3 operator * ( const Vec3& v ) const;

  /**
   * Create quaternion for rotation around a given axis.
   */
  static Quat rotationAxis( const Vec3& axis, float theta );

  /**
   * Create quaternion for rotation around x axis.
   */
  static Quat rotationX( float theta );

  /**
   * Create quaternion for rotation around y axis.
   */
  static Quat rotationY( float theta );

  /**
   * Create quaternion for rotation around z axis.
   */
  static Quat rotationZ( float theta );

  /**
   * `rotationZ( heading ) * rotationX( pitch ) * rotationZ( roll )`.
   */
  static Quat rotationZXZ( float heading, float pitch, float roll );

  /**
   * Get rotation axis and angle.
   */
  void toAxisAngle( Vec3* axis, float* angle ) const;

  /**
   * Spherical linear interpolation between two orientations.
   */
  static Quat slerp( const Quat& a, const Quat& b, float t );

  /**
   * Approximate but much faster spherical linear interpolation between two orientations.
   */
  static Quat fastSlerp( const Quat& a, const Quat& b, float t )
  {
    hard_assert( 0.0f <= t && t <= 1.0f );

    Quat  d = *a * b;
    float k = d.w < 0.0f ? -t : t;

    d.x *= k;
    d.y *= k;
    d.z *= k;
    d.w  = Math::fastSqrt( 1.0f - d.x*d.x - d.y*d.y - d.z*d.z );

    return a * d;
  }

};

/**
 * Per-component absolute value of a quaternion.
 */
OZ_ALWAYS_INLINE
inline Quat abs( const Quat& a )
{
#ifdef OZ_SIMD_MATH
  return Quat( vAbs( a.u4 ) );
#else
  return Quat( abs( a.x ), abs( a.y ), abs( a.z ), abs( a.w ) );
#endif
}

/**
 * Per-component minimum of two quaternions.
 */
OZ_ALWAYS_INLINE
inline Quat min( const Quat& a, const Quat& b )
{
#ifdef OZ_SIMD_MATH
  return Quat( vMin( a.f4, b.f4 ) );
#else
  return Quat( min( a.x, b.x ), min( a.y, b.y ), min( a.z, b.z ), min( a.w, b.w ) );
#endif
}

/**
 * Per-component maximum of two quaternions.
 */
OZ_ALWAYS_INLINE
inline Quat max( const Quat& a, const Quat& b )
{
#ifdef OZ_SIMD_MATH
  return Quat( vMax( a.f4, b.f4 ) );
#else
  return Quat( max( a.x, b.x ), max( a.y, b.y ), max( a.z, b.z ), max( a.w, b.w ) );
#endif
}

/**
 * Per-component clamped value of quaternions.
 */
OZ_ALWAYS_INLINE
inline Quat clamp( const Quat& c, const Quat& a, const Quat& b )
{
#ifdef OZ_SIMD_MATH
  return Quat( vMin( b.f4, vMax( a.f4, c.f4 ) ) );
#else
  return Quat( clamp( c.x, a.x, b.x ), clamp( c.y, a.y, b.y ), clamp( c.z, a.z, b.z ),
               clamp( c.w, a.w, b.w ) );
#endif
}

}
