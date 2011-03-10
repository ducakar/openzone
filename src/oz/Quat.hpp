/*
 *  Quat.hpp
 *
 *  Quaternion
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "Vec4.hpp"

namespace oz
{

  class Quat : public Vec4
  {
    public:

      static const Quat ZERO;
      static const Quat ID;

      OZ_ALWAYS_INLINE
      explicit Quat()
      {}

      OZ_ALWAYS_INLINE
      explicit Quat( float x, float y, float z, float w ) : Vec4( x, y, z, w )
      {}

      OZ_ALWAYS_INLINE
      explicit Quat( const float* q ) : Vec4( q )
      {}

      OZ_ALWAYS_INLINE
      explicit Quat( const Vec4& v ) : Vec4( v )
      {}

      OZ_ALWAYS_INLINE
      explicit Quat( const Vec3& v, float w ) : Vec4( v, w )
      {}

      OZ_ALWAYS_INLINE
      Quat abs() const
      {
        return Quat( Math::abs( x ), Math::abs( y ), Math::abs( z ), Math::abs( w ) );
      }

      OZ_ALWAYS_INLINE
      Quat operator * () const
      {
        return Quat( -x, -y, -z, w );
      }

      OZ_ALWAYS_INLINE
      Quat operator ~ () const
      {
        hard_assert( x*x + y*y + z*z + w*w > 0.0f );

        float k = 1.0f / Math::sqrt( x*x + y*y + z*z + w*w );
        return Quat( x * k, y * k, z * k, w * k );
      }

      OZ_ALWAYS_INLINE
      Quat fastUnit() const
      {
        hard_assert( x*x + y*y + z*z + w*w > 0.0f );

        float k = Math::fastInvSqrt( x*x + y*y + z*z + w*w );
        return Quat( x * k, y * k, z * k, w * k );
      }

      OZ_ALWAYS_INLINE
      Quat operator + () const
      {
        return *this;
      }

      OZ_ALWAYS_INLINE
      Quat operator - () const
      {
        return Quat( -x, -y, -z, -w );
      }

      OZ_ALWAYS_INLINE
      Quat operator + ( const Quat& q ) const
      {
        return Quat( x + q.x, y + q.y, z + q.z, w + q.w );
      }

      OZ_ALWAYS_INLINE
      Quat operator - ( const Quat& q ) const
      {
        return Quat( x - q.x, y - q.y, z - q.z, w - q.w );
      }

      OZ_ALWAYS_INLINE
      Quat operator * ( float k ) const
      {
        return Quat( x * k, y * k, z * k, w * k );
      }

      OZ_ALWAYS_INLINE
      friend Quat operator * ( float k, const Quat& q )
      {
        return Quat( q.x * k, q.y * k, q.z * k, q.w * k );
      }

      OZ_ALWAYS_INLINE
      Quat operator / ( float k ) const
      {
        hard_assert( k != 0.0f );

        k = 1.0f / k;
        return Quat( x * k, y * k, z * k, w * k );
      }

      OZ_ALWAYS_INLINE
      Quat& operator += ( const Quat& q )
      {
        x += q.x;
        y += q.y;
        z += q.z;
        w += q.w;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Quat& operator -= ( const Quat& q )
      {
        x -= q.x;
        y -= q.y;
        z -= q.z;
        w -= q.w;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Quat& operator *= ( float k )
      {
        x *= k;
        y *= k;
        z *= k;
        w *= k;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Quat& operator /= ( float k )
      {
        hard_assert( k != 0.0f );

        k = 1.0f / k;
        x *= k;
        y *= k;
        z *= k;
        w *= k;
        return *this;
      }

      // quaternion multiplication
      OZ_ALWAYS_INLINE
      Quat operator ^ ( const Quat& a ) const
      {

        return Quat( w*a.x + x*a.w + y*a.z - z*a.y,
                     w*a.y + y*a.w + z*a.x - x*a.z,
                     w*a.z + z*a.w + x*a.y - y*a.x,
                     w*a.w - x*a.x - y*a.y - z*a.z );
      }

      // quaternion multiplication
      OZ_ALWAYS_INLINE
      Quat& operator ^= ( const Quat& a )
      {
        float tx = x, ty = y, tz = z;

        x = w*a.x + tx*a.w + ty*a.z - tz*a.y;
        y = w*a.y + ty*a.w + tz*a.x - tx*a.z;
        z = w*a.z + tz*a.w + tx*a.y - ty*a.x;
        w = w*a.w - tx*a.x - ty*a.y - tz*a.z;

        return *this;
      }

      // make quaternion for rotation around given axis
      static Quat rotAxis( const Vec3& axis, float theta )
      {
        float s, c;
        Math::sincos( theta * 0.5f, &s, &c );
        return Quat( s * axis.x, s * axis.y, s * axis.z, c );
      }

      // make quaternion for rotation around x axis
      static Quat rotX( float theta )
      {
        float s, c;
        Math::sincos( theta * 0.5f, &s, &c );
        return Quat( s, 0.0f, 0.0f, c );
      }

      // make quaternion for rotation around y axis
      static Quat rotY( float theta )
      {
        float s, c;
        Math::sincos( theta * 0.5f, &s, &c );
        return Quat( 0.0f, s, 0.0f, c );
      }

      // make quaternion for rotation around z axis
      static Quat rotZ( float theta )
      {
        float s, c;
        Math::sincos( theta * 0.5f, &s, &c );
        return Quat( 0.0f, 0.0f, s, c );
      }

      // rotZ ^ rotX ^ rotY
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

  };

}
