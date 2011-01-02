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

  class Mat33;
  class Mat44;

  class __attribute__(( aligned( 16 ) )) Quat : public Vec4
  {
    public:

      static const Quat ZERO;
      static const Quat ID;

      explicit Quat()
      {}

      explicit Quat( float4 f4 ) : Vec4( f4 )
      {}

      explicit Quat( float x_, float y_, float z_, float w_ ) : Vec4( x_, y_, z_, w_ )
      {}

      explicit Quat( const float* q ) : Vec4( q[0], q[1], q[2], q[3] )
      {}

      bool operator == ( const Quat& q ) const
      {
        return x == q.x && y == q.y && z == q.z && w == q.w;
      }

      bool operator != ( const Quat& q ) const
      {
        return x != q.x || y != q.y || z != q.z || w != q.w;
      }

      operator const float* () const
      {
        return &x;
      }

      operator float* ()
      {
        return &x;
      }

      const float& operator [] ( int i ) const
      {
        assert( 0 <= i && i < 4 );

        return ( &x )[i];
      }

      float& operator [] ( int i )
      {
        assert( 0 <= i && i < 4 );

        return ( &x )[i];
      }

      Quat operator * () const
      {
        return Quat( -x, -y, -z, w );
      }

      float operator ! () const
      {
        return Math::sqrt( x*x + y*y + z*z + w*w );
      }

      float sqL() const
      {
        return x*x + y*y + z*z + w*w;
      }

      bool isUnit() const
      {
        return x*x + y*y + z*z + w*w == 1.0f;
      }

      Quat operator ~ () const
      {
        assert( x*x + y*y + z*z + w*w > 0.0f );

        float k = 1.0f / Math::sqrt( x*x + y*y + z*z + w*w );
        return Quat( f4 * float4( k, k, k, k ) );
      }

      Quat fastUnit() const
      {
        assert( x*x + y*y + z*z + w*w > 0.0f );

        float k = Math::fastInvSqrt( x*x + y*y + z*z + w*w );
        return Quat( f4 * float4( k, k, k, k ) );
      }

      Quat operator + () const
      {
        return *this;
      }

      Quat operator - () const
      {
        return Quat( -f4 );
      }

      Quat operator + ( const Quat& v ) const
      {
        return Quat( f4 + v.f4 );
      }

      Quat operator - ( const Quat& v ) const
      {
        return Quat( f4 - v.f4 );
      }

      Quat operator * ( float k ) const
      {
        return Quat( f4 * float4( k, k, k, k ) );
      }

      friend Quat operator * ( float k, const Quat& v )
      {
        return Quat( float4( k, k, k, k ) * v.f4 );
      }

      Quat operator / ( float k ) const
      {
        assert( k != 0.0f );

        return Quat( f4 / float4( k, k, k, k ) );
      }

      Quat& operator += ( const Quat& v )
      {
        f4 += v.f4;
        return *this;
      }

      Quat& operator -= ( const Quat& v )
      {
        f4 -= v.f4;
        return *this;
      }

      Quat& operator *= ( float k )
      {
        f4 *= float4( k, k, k, k );
        return *this;
      }

      Quat& operator /= ( float k )
      {
        assert( k != 0.0f );

        f4 /= float4( k, k, k, k );
        return *this;
      }

      // quaternion multiplication
      Quat operator ^ ( const Quat& a ) const
      {

        return Quat( w*a.x + x*a.w + y*a.z - z*a.y,
                     w*a.y + y*a.w + z*a.x - x*a.z,
                     w*a.z + z*a.w + x*a.y - y*a.x,
                     w*a.w - x*a.x - y*a.y - z*a.z );
      }

      // quaternion multiplication
      Quat& operator ^= ( const Quat& a )
      {
        float tx = x, ty = y, tz = z;

        x = w*a.x + tx*a.w + ty*a.z - tz*a.y;
        y = w*a.y + ty*a.w + tz*a.x - tx*a.z;
        z = w*a.z + tz*a.w + tx*a.y - ty*a.x;
        w = w*a.w - tx*a.x - ty*a.y - tz*a.z;

        return *this;
      }

      // make rotation matrix (implemented in Mat33.hpp)
      Mat33 rotMat33() const;

      // make transposed (inverse) rotation matrix (implemented in Mat33.hpp)
      Mat33 invRotMat33() const;

      // make rotation matrix (implemented in Mat44.hpp)
      Mat44 rotMat44() const;

      // make transposed (inverse) rotation matrix (implemented in Mat44.hpp)
      Mat44 invRotMat44() const;

      // make quaternion for rotation around given axis
      static Quat rotAxis( const Vec4& axis, float theta )
      {
        float s, c;
        Math::sincos( theta * 0.5f, &s, &c );
        Vec4 qv = s * axis;
        return Quat( qv.f4 + float4( 0.0f, 0.0f, 0.0f, c ) );
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
