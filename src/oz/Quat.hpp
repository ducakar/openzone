/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
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
 * @file oz/Quat.hpp
 */

#pragma once

#include "Vec4.hpp"

namespace oz
{

/**
 * Quaternion.
 *
 * @ingroup oz
 */
class Quat : public Vec4
{
  public:

    /// Zero quaternion.
    static const Quat ZERO;

    /// Quaternion representing rotation identity.
    static const Quat ID;

    /**
     * Create an uninitialised instance.
     */
    Quat() = default;

#ifdef OZ_SIMD
  protected:

    /**
     * Create from an uint SIMD vector.
     */
    OZ_ALWAYS_INLINE
    explicit Quat( uint4 u4 ) : Vec4( u4 )
    {}

    /**
     * Create from a float SIMD vector.
     */
    OZ_ALWAYS_INLINE
    explicit Quat( float4 f4 ) : Vec4( f4 )
    {}

  public:
#endif

    /**
     * Create a quaternion with the given components.
     */
#ifdef OZ_SIMD
    OZ_ALWAYS_INLINE
    explicit Quat( float x, float y, float z, float w ) : Vec4( float4( x, y, z, w ) )
    {}
#else
    OZ_ALWAYS_INLINE
    explicit Quat( float x, float y, float z, float w ) : Vec4( x, y, z, w )
    {}
#endif

    /**
     * Create from an array of 4 floats.
     */
    OZ_ALWAYS_INLINE
    explicit Quat( const float* q ) : Vec4( q )
    {}

    /**
     * Create quaternion from a four component vector.
     */
    OZ_ALWAYS_INLINE
    explicit Quat( const Vec4& v ) : Vec4( v )
    {}

    /**
     * Quaternion with absolute components.
     */
    OZ_ALWAYS_INLINE
    Quat abs() const
    {
#ifdef OZ_SIMD
      return Quat( u4 & uint4( 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff ) );
#else
      return Quat( Math::abs( x ), Math::abs( y ), Math::abs( z ), Math::abs( w ) );
#endif
    }

    /**
     * Conjugate quaternion.
     */
    OZ_ALWAYS_INLINE
    Quat operator * () const
    {
#ifdef OZ_SIMD
      return Quat( u4 ^ uint4( 0x80000000, 0x80000000, 0x80000000, 0x00000000 ) );
#else
      return Quat( -x, -y, -z, w );
#endif
    }

    /**
     * Unit quaternion.
     */
    OZ_ALWAYS_INLINE
    Quat operator ~ () const
    {
      hard_assert( x*x + y*y + z*z + w*w > 0.0f );

#ifdef OZ_SIMD
      float k = Math::sqrt( x*x + y*y + z*z + w*w );
      return Quat( f4 / float4( k, k, k, k ) );
#else
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
      hard_assert( x*x + y*y + z*z + w*w > 0.0f );

#ifdef OZ_SIMD
      float k = Math::fastInvSqrt( x*x + y*y + z*z + w*w );
      return Quat( f4 * float4( k, k, k, k ) );
#else
      float k = Math::fastInvSqrt( x*x + y*y + z*z + w*w );
      return Quat( x * k, y * k, z * k, w * k );
#endif
    }

    /**
     * Original quaternion.
     */
    OZ_ALWAYS_INLINE
    const Quat& operator + () const
    {
      return *this;
    }

    /**
     * Opposite quaternion.
     */
    OZ_ALWAYS_INLINE
    Quat operator - () const
    {
#ifdef OZ_SIMD
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
#ifdef OZ_SIMD
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
#ifdef OZ_SIMD
      return Quat( f4 - q.f4 );
#else
      return Quat( x - q.x, y - q.y, z - q.z, w - q.w );
#endif
    }

    /**
     * Product.
     */
    OZ_ALWAYS_INLINE
    Quat operator * ( float k ) const
    {
#ifdef OZ_SIMD
      return Quat( f4 * float4( k, k, k, k ) );
#else
      return Quat( x * k, y * k, z * k, w * k );
#endif
    }

    /**
     * Product.
     */
    OZ_ALWAYS_INLINE
    friend Quat operator * ( float k, const Quat& q )
    {
#ifdef OZ_SIMD
      return Quat( float4( k, k, k, k ) * q.f4 );
#else
      return Quat( k * q.x, k * q.y, k * q.z, k * q.w );
#endif
    }

    /**
     * Product.
     */
    OZ_ALWAYS_INLINE
    Quat operator ^ ( const Quat& q ) const
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

#ifdef OZ_SIMD
      return Quat( f4 / float4( k, k, k, k ) );
#else
      k = 1.0f / k;
      return Quat( x * k, y * k, z * k, w * k );
#endif
    }

    /**
     * Addition.
     */
    OZ_ALWAYS_INLINE
    Quat& operator += ( const Quat& q )
    {
#ifdef OZ_SIMD
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
#ifdef OZ_SIMD
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
    Quat& operator *= ( float k )
    {
#ifdef OZ_SIMD
      f4 *= float4( k, k, k, k );
#else
      x *= k;
      y *= k;
      z *= k;
      w *= k;
#endif
      return *this;
    }

    /**
     * Multiplication.
     */
    OZ_ALWAYS_INLINE
    Quat& operator ^= ( const Quat& q )
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

#ifdef OZ_SIMD
      f4 /= float4( k, k, k, k );
#else
      k = 1.0f / k;
      x *= k;
      y *= k;
      z *= k;
      w *= k;
#endif
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
     * rotZ ^ rotX ^ rotY
     */
    static Quat rotZYX( float z, float y, float x )
    {
      float sx, cx, sy, cy, sz, cz;

      Math::sincos( x * 0.5f, &sx, &cx );
      Math::sincos( y * 0.5f, &sy, &cy );
      Math::sincos( z * 0.5f, &sz, &cz );

      float cxcy = cx * cy;
      float cxsy = cx * sy;
      float sxcy = sx * cy;
      float sxsy = sx * sy;

      return Quat( sxcy * cz - cxsy * sz,
                   cxsy * cz + sxcy * sz,
                   cxcy * sz + sxsy * cz,
                   cxcy * cz - sxsy * sz );
    }

    /**
     * Spherical linear interpolation between two rotations.
     */
    static Quat slerp( const Quat& a, const Quat& b, float t )
    {
      Quat d = *a ^ b;
      float sine  = Math::sqrt( 1.0f - d.w*d.w );
      float angle = 2.0f * Math::acos( d.w );

      hard_assert( sine != 0.0f );

      float k = 1.0f / sine;
      return rotAxis( Vec3( d.x * k, d.y * k, d.z * k ), t * angle );
    }

    /**
     * Approximate spherical linear interpolation between two similar rotations.
     */
    static Quat fastSlerp( const Quat& a, const Quat& b, float t )
    {
      Quat d = *a ^ b;
      float k = d.w < 0.0f ? -t : t;

      d.x *= k;
      d.y *= k;
      d.z *= k;
      d.w = Math::fastSqrt( 1.0f - d.x*d.x - d.y*d.y - d.z*d.z );

      return a ^ d;
    }

};

}
