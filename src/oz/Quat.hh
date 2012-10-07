/*
 * liboz - OpenZone Core Library.
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
 * @file oz/Quat.hh
 *
 * Quat class.
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
    explicit Quat() = default;

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
     * Create a quaternion with the given components.
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
      hard_assert( 0 <= i && i < 4 );

      return ( &x )[i];
    }

    /**
     * Reference to the `i`-th member.
     */
    OZ_ALWAYS_INLINE
    float& operator [] ( int i )
    {
      hard_assert( 0 <= i && i < 4 );

      return ( &x )[i];
    }

    /**
     * Quaternion with absolute components.
     */
    OZ_ALWAYS_INLINE
    Quat abs() const
    {
#ifdef OZ_SIMD_MATH
      return Quat( vAbs( u4 ) );
#else
      return Quat( oz::abs( x ), oz::abs( y ), oz::abs( z ), oz::abs( w ) );
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
    OZ_ALWAYS_INLINE
    Quat operator ~ () const
    {
#ifdef OZ_SIMD_MATH
      scalar s = 1.0f / Math::sqrt( vFirst( vDot( f4, f4 ) ) );
      return Quat( f4 * s.f4 );
#else
      hard_assert( x*x + y*y + z*z + w*w > 0.0f );

      float k = 1.0f / Math::sqrt( x*x + y*y + z*z + w*w );
      return Quat( x * k, y * k, z * k, w * k );
#endif
    }

    /**
     * Approximate unit quaternion.
     */
    OZ_ALWAYS_INLINE
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
    OZ_ALWAYS_INLINE
    Quat operator * ( const Quat& q ) const
    {
#ifdef OZ_SIMD_MATH
      float4 k0 = vFill( w );
      float4 k1 = f4;
      float4 k2 = vShuffle( f4, f4, 1, 2, 0, 3 );
      float4 k3 = vShuffle( f4, f4, 2, 0, 1, 3 );

      float4 q0 = q.f4;
      float4 q1 = vFill( q.w );
      float4 q2 = vShuffle( q.f4, q.f4, 2, 0, 1, 3 );
      float4 q3 = vShuffle( q.f4, q.f4, 1, 2, 0, 3 );

      Quat tq = Quat( k0*q0 + k1*q1 + k2*q2 - k3*q3 );
      tq.w   -= vFirst( vDot( k1, q.f4 ) );

      return tq;
#else
      return Quat( w*q.x + x*q.w + y*q.z - z*q.y,
                   w*q.y + y*q.w + z*q.x - x*q.z,
                   w*q.z + z*q.w + x*q.y - y*q.x,
                   w*q.w - x*q.x - y*q.y - z*q.z );
#endif
    }

    /**
     * Quotient.
     */
    OZ_ALWAYS_INLINE
    Quat operator / ( scalar s ) const
    {
#ifdef OZ_SIMD_MATH
      return Quat( f4 / s.f4 );
#else
      hard_assert( s != 0.0f );

      s = 1.0f / s;
      return Quat( x * s, y * s, z * s, w * s );
#endif
    }

    /**
     * Quotient.
     */
    OZ_ALWAYS_INLINE
    friend Quat operator / ( scalar s, const Quat& q )
    {
#ifdef OZ_SIMD_MATH
      s.f4 = s.f4 / vDot( q.f4, q.f4 );
      s.f4 = vShuffle( s.f4, -s.f4, 0, 0, 0, 0 );
      s.f4 = vShuffle( s.f4, s.f4, 0, 0, 0, 2 );

      return Quat( q.f4 * s.f4 );
#else
      s = s / ( q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w );
      float ns = -s;
      return Quat( q.x * ns, q.y * ns, q.z * ns, q.w * s );
#endif
    }

    /**
     * Quaternion quotient.
     */
    OZ_ALWAYS_INLINE
    Quat operator / ( const Quat& q ) const
    {
#ifdef OZ_SIMD_MATH
      float4 k0 = f4;
      float4 k1 = vFill( w );
      float4 k2 = vShuffle( f4, f4, 2, 0, 1, 3 );
      float4 k3 = vShuffle( f4, f4, 1, 2, 0, 3 );

      float4 q0 = vFill( q.w );
      float4 q1 = q.f4;
      float4 q2 = vShuffle( q.f4, q.f4, 1, 2, 0, 3 );
      float4 q3 = vShuffle( q.f4, q.f4, 2, 0, 1, 3 );

      Quat tq = Quat( k0*q0 - k1*q1 + k2*q2 - k3*q3 );
      tq.w    = vFirst( vDot( k0, q.f4 ) );
      tq.f4  *= vDot( q.f4, q.f4 );

      return tq;
#else
      float k = q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
      return Quat( k * ( x*q.w - w*q.x + z*q.y - y*q.z ),
                   k * ( y*q.w - w*q.y + x*q.z - z*q.x ),
                   k * ( z*q.w - w*q.z + y*q.x - x*q.y ),
                   k * ( w*q.w + x*q.x + y*q.y + z*q.z ) );
#endif
    }

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
    OZ_ALWAYS_INLINE
    Quat& operator *= ( const Quat& q )
    {
#ifdef OZ_SIMD_MATH
      float4 k0 = vFill( w );
      float4 k1 = f4;
      float4 k2 = vShuffle( f4, f4, 1, 2, 0, 3 );
      float4 k3 = vShuffle( f4, f4, 2, 0, 1, 3 );

      float4 q0 = q.f4;
      float4 q1 = vFill( q.w );
      float4 q2 = vShuffle( q.f4, q.f4, 2, 0, 1, 3 );
      float4 q3 = vShuffle( q.f4, q.f4, 1, 2, 0, 3 );

      f4 = k0*q0 + k1*q1 + k2*q2 - k3*q3;
      w -= vFirst( vDot( k1, q0 ) );
#else
      float tx = x, ty = y, tz = z;

      x = w*q.x + tx*q.w + ty*q.z - tz*q.y;
      y = w*q.y + ty*q.w + tz*q.x - tx*q.z;
      z = w*q.z + tz*q.w + tx*q.y - ty*q.x;
      w = w*q.w - tx*q.x - ty*q.y - tz*q.z;
#endif
      return *this;
    }

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
    OZ_ALWAYS_INLINE
    Quat& operator /= ( const Quat& q )
    {
#ifdef OZ_SIMD_MATH
      float4 k0 = f4;
      float4 k1 = vFill( w );
      float4 k2 = vShuffle( f4, f4, 2, 0, 1, 3 );
      float4 k3 = vShuffle( f4, f4, 1, 2, 0, 3 );

      float4 q0 = vFill( q.w );
      float4 q1 = q.f4;
      float4 q2 = vShuffle( q.f4, q.f4, 1, 2, 0, 3 );
      float4 q3 = vShuffle( q.f4, q.f4, 2, 0, 1, 3 );

      f4  = k0*q0 - k1*q1 + k2*q2 - k3*q3;
      w   = vFirst( vDot( k0, q.f4 ) );
      f4 *= vDot( q.f4, q.f4 );
#else
      float k = q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
      Quat  t = Quat( x*q.w - w*q.x + z*q.y - y*q.z,
                      y*q.w - w*q.y + x*q.z - z*q.x,
                      z*q.w - w*q.z + y*q.x - x*q.y,
                      w*q.w + x*q.x + y*q.y + z*q.z );

      x = k * t.x;
      y = k * t.y;
      z = k * t.z;
      w = k * t.w;
#endif
      return *this;
    }

    /**
     * Vector rotation.
     */
    Vec3 operator * ( const Vec3& v ) const
    {
      float x2  = x + x;
      float y2  = y + y;
      float z2  = z + z;
      float xx2 = x2 * x;
      float yy2 = y2 * y;
      float zz2 = z2 * z;
      float xy2 = x2 * y;
      float xz2 = x2 * z;
      float xw2 = x2 * w;
      float yz2 = y2 * z;
      float yw2 = y2 * w;
      float zw2 = z2 * w;
      float yy1 = 1.0f - yy2;
      float xx1 = 1.0f - xx2;

      return Vec3( ( yy1 - zz2 ) * v.x + ( xy2 - zw2 ) * v.y + ( xz2 + yw2 ) * v.z,
                   ( xy2 + zw2 ) * v.x + ( xx1 - zz2 ) * v.y + ( yz2 - xw2 ) * v.z,
                   ( xz2 - yw2 ) * v.x + ( yz2 + xw2 ) * v.y + ( xx1 - yy2 ) * v.z );
    }

    /**
     * Create quaternion for rotation around the given axis.
     */
    static Quat rotationAxis( const Vec3& axis, float theta )
    {
      float s, c;
      Math::sincos( theta * 0.5f, &s, &c );
      return Quat( s * axis.x, s * axis.y, s * axis.z, c );
    }

    /**
     * Create quaternion for rotation around x axis.
     */
    static Quat rotationX( float theta )
    {
      float s, c;
      Math::sincos( theta * 0.5f, &s, &c );
      return Quat( s, 0.0f, 0.0f, c );
    }

    /**
     * Create quaternion for rotation around y axis.
     */
    static Quat rotationY( float theta )
    {
      float s, c;
      Math::sincos( theta * 0.5f, &s, &c );
      return Quat( 0.0f, s, 0.0f, c );
    }

    /**
     * Create quaternion for rotation around z axis.
     */
    static Quat rotationZ( float theta )
    {
      float s, c;
      Math::sincos( theta * 0.5f, &s, &c );
      return Quat( 0.0f, 0.0f, s, c );
    }

    /**
     * `rotationZ( heading ) * rotationX( pitch ) * rotationZ( roll )`.
     */
    static Quat rotationZXZ( float heading, float pitch, float roll )
    {
      float hs, hc, ps, pc, rs, rc;

      Math::sincos( heading * 0.5f, &hs, &hc );
      Math::sincos( pitch   * 0.5f, &ps, &pc );
      Math::sincos( roll    * 0.5f, &rs, &rc );

      float hsps = hs * ps;
      float hcpc = hc * pc;
      float hspc = hs * pc;
      float hcps = hc * ps;

      return Quat( hcps * rc + hsps * rs,
                   hsps * rc - hcps * rs,
                   hspc * rc + hcpc * rs,
                   hcpc * rc - hspc * rs );
    }

    /**
     * Get rotation axis and angle.
     */
    void toAxisAngle( Vec3* axis, float* angle ) const
    {
      *angle = 2.0f * Math::acos( w );

      float k = Math::sqrt( 1.0f - w*w );
      if( k == 0.0f ) {
        *axis = Vec3::ZERO;
      }
      else {
        k = 1.0f / k;
        *axis = Vec3( x * k, y * k, z * k );
      }
    }

    /**
     * Approximate spherical linear interpolation between two rotations.
     */
    static Quat slerp( const Quat& a, const Quat& b, float t )
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

}
