/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 *
 * @ingroup oz
 */
class Quat
{
  public:

    /// Zero quaternion.
    static const Quat ZERO;

    /// Quaternion representing rotation identity.
    static const Quat ID;

    float x; ///< X component.
    float y; ///< Y component.
    float z; ///< Z component.
    float w; ///< W component.

    /**
     * Create an uninitialised instance.
     */
    Quat() = default;

    /**
     * Create a quaternion with the given components.
     */
    OZ_ALWAYS_INLINE
    explicit Quat( float x_, float y_, float z_, float w_ ) :
      x( x_ ), y( y_ ), z( z_ ), w( w_ )
    {}

    /**
     * Create from an array of 4 floats.
     */
    OZ_ALWAYS_INLINE
    explicit Quat( const float* q ) :
      x( q[0] ), y( q[1] ), z( q[2] ), w( q[3] )
    {}

    /**
     * Create quaternion from a four-component vector.
     */
    OZ_ALWAYS_INLINE
    explicit Quat( const Vec4& v ) :
      x( v.x ), y( v.y ), z( v.z ), w( v.w )
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
      return Vec4( x, y, z, w );
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
      hard_assert( 0 <= i && i < 4 );

      return ( &x )[i];
    }

    /**
     * Reference to the i-th member.
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
      return Quat( Math::fabs( x ), Math::fabs( y ), Math::fabs( z ), Math::fabs( w ) );
    }

    /**
     * Conjugate quaternion.
     */
    OZ_ALWAYS_INLINE
    Quat operator * () const
    {
      return Quat( -x, -y, -z, w );
    }

    /**
     * Length.
     */
    OZ_ALWAYS_INLINE
    float operator ! () const
    {
      return Math::sqrt( x*x + y*y + z*z + w*w );
    }

    /**
     * Approximate length.
     */
    OZ_ALWAYS_INLINE
    float fastL() const
    {
      return Math::fastSqrt( x*x + y*y + z*z + w*w );
    }

    /**
     * Square of length.
     */
    OZ_ALWAYS_INLINE
    float sqL() const
    {
      return x*x + y*y + z*z + w*w;
    }

    /**
     * Unit quaternion.
     */
    OZ_ALWAYS_INLINE
    Quat operator ~ () const
    {
      hard_assert( x*x + y*y + z*z + w*w > 0.0f );

      float k = 1.0f / Math::sqrt( x*x + y*y + z*z + w*w );
      return Quat( x * k, y * k, z * k, w * k );
    }

    /**
     * Approximate unit quaternion.
     */
    OZ_ALWAYS_INLINE
    Quat fastUnit() const
    {
      hard_assert( x*x + y*y + z*z + w*w > 0.0f );

      float k = Math::fastInvSqrt( x*x + y*y + z*z + w*w );
      return Quat( x * k, y * k, z * k, w * k );
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
      return Quat( -x, -y, -z, -w );
    }

    /**
     * Sum.
     */
    OZ_ALWAYS_INLINE
    Quat operator + ( const Quat& q ) const
    {
      return Quat( x + q.x, y + q.y, z + q.z, w + q.w );
    }

    /**
     * Difference.
     */
    OZ_ALWAYS_INLINE
    Quat operator - ( const Quat& q ) const
    {
      return Quat( x - q.x, y - q.y, z - q.z, w - q.w );
    }

    /**
     * Product.
     */
    OZ_ALWAYS_INLINE
    Quat operator * ( float k ) const
    {
      return Quat( x * k, y * k, z * k, w * k );
    }

    /**
     * Product.
     */
    OZ_ALWAYS_INLINE
    friend Quat operator * ( float k, const Quat& q )
    {
      return Quat( k * q.x, k * q.y, k * q.z, k * q.w );
    }

    /**
     * Quaternion product.
     */
    OZ_ALWAYS_INLINE
    Quat operator * ( const Quat& q ) const
    {

      return Quat( w*q.x + x*q.w + y*q.z - z*q.y,
                   w*q.y + y*q.w + z*q.x - x*q.z,
                   w*q.z + z*q.w + x*q.y - y*q.x,
                   w*q.w - x*q.x - y*q.y - z*q.z );
    }

    /**
     * Quotient.
     */
    OZ_ALWAYS_INLINE
    Quat operator / ( float k ) const
    {
      hard_assert( k != 0.0f );

      k = 1.0f / k;
      return Quat( x * k, y * k, z * k, w * k );
    }

    /**
     * Addition.
     */
    OZ_ALWAYS_INLINE
    Quat& operator += ( const Quat& q )
    {
      x += q.x;
      y += q.y;
      z += q.z;
      w += q.w;
      return *this;
    }

    /**
     * Subtraction.
     */
    OZ_ALWAYS_INLINE
    Quat& operator -= ( const Quat& q )
    {
      x -= q.x;
      y -= q.y;
      z -= q.z;
      w -= q.w;
      return *this;
    }

    /**
     * Multiplication.
     */
    OZ_ALWAYS_INLINE
    Quat& operator *= ( float k )
    {
      x *= k;
      y *= k;
      z *= k;
      w *= k;
      return *this;
    }

    /**
     * Quaternion multiplication.
     */
    OZ_ALWAYS_INLINE
    Quat& operator *= ( const Quat& q )
    {
      float tx = x, ty = y, tz = z;

      x = w*q.x + tx*q.w + ty*q.z - tz*q.y;
      y = w*q.y + ty*q.w + tz*q.x - tx*q.z;
      z = w*q.z + tz*q.w + tx*q.y - ty*q.x;
      w = w*q.w - tx*q.x - ty*q.y - tz*q.z;

      return *this;
    }

    /**
     * Division.
     */
    OZ_ALWAYS_INLINE
    Quat& operator /= ( float k )
    {
      hard_assert( k != 0.0f );

      k  = 1.0f / k;
      x *= k;
      y *= k;
      z *= k;
      w *= k;
      return *this;
    }

    /**
     * Create quaternion for rotation around the given axis.
     */
    static Quat rotAxis( const Vec3& axis, float theta )
    {
      float s, c;
      Math::sincos( theta * 0.5f, &s, &c );
      return Quat( s * axis.x, s * axis.y, s * axis.z, c );
    }

    /**
     * Create quaternion for rotation around x axis.
     */
    static Quat rotX( float theta )
    {
      float s, c;
      Math::sincos( theta * 0.5f, &s, &c );
      return Quat( s, 0.0f, 0.0f, c );
    }

    /**
     * Create quaternion for rotation around y axis.
     */
    static Quat rotY( float theta )
    {
      float s, c;
      Math::sincos( theta * 0.5f, &s, &c );
      return Quat( 0.0f, s, 0.0f, c );
    }

    /**
     * Create quaternion for rotation around z axis.
     */
    static Quat rotZ( float theta )
    {
      float s, c;
      Math::sincos( theta * 0.5f, &s, &c );
      return Quat( 0.0f, 0.0f, s, c );
    }

    /**
     * rotZ ^ rotX ^ rotZ
     */
    static Quat rotZXZ( float heading, float pitch, float roll )
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
     * Spherical linear interpolation between two rotations.
     */
    static Quat slerp( const Quat& a, const Quat& b, float t )
    {
      Quat  diff  = *a * b;
      float sine  = Math::sqrt( 1.0f - diff.w*diff.w );
      float angle = 2.0f * Math::acos( diff.w );

      hard_assert( sine != 0.0f );

      float k = 1.0f / sine;
      return rotAxis( Vec3( diff.x * k, diff.y * k, diff.z * k ), t * angle );
    }

    /**
     * Approximate spherical linear interpolation between two similar rotations.
     */
    static Quat fastSlerp( const Quat& a, const Quat& b, float t )
    {
      Quat  diff = *a * b;
      float k    = diff.w < 0.0f ? -t : t;

      diff.x *= k;
      diff.y *= k;
      diff.z *= k;
      diff.w  = Math::fastSqrt( 1.0f - diff.x*diff.x - diff.y*diff.y - diff.z*diff.z );

      return a * diff;
    }

};

}
