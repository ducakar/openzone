/*
 *  Mat44.hpp
 *
 *  Column-major 4x4 matrix
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "Point4.hpp"
#include "Quat.hpp"
#include "Vec3.hpp"

namespace oz
{

  class __attribute__(( aligned( 16 ) )) Mat44
  {
    public:

      static const Mat44 ZERO;
      static const Mat44 ID;

      // first column (i base vector)
      Quat x;
      // second column (j base vector)
      Quat y;
      // third column (k base vector)
      Quat z;
      // last column (usually used for position)
      Quat w;

      explicit Mat44()
      {}

      explicit Mat44( const Quat& a, const Quat& b, const Quat& c, const Quat& d ) :
          x( a ), y( b ), z( c ), w( d )
      {}

      explicit Mat44( float4 a, float4 b, float4 c, float4 d ) :
          x( a ), y( b ), z( c ), w( d )
      {}

      explicit Mat44( float xx, float xy, float xz, float xw,
                      float yx, float yy, float yz, float yw,
                      float zx, float zy, float zz, float zw,
                      float wx, float wy, float wz, float ww ) :
          x( xx, xy, xz, xw ),
          y( yx, yy, yz, yw ),
          z( zx, zy, zz, zw ),
          w( wx, wy, wz, ww )
      {}

      explicit Mat44( const float* v ) : x( &v[0] ), y( &v[4] ), z( &v[8] ), w( &v[12] )
      {}

      bool operator == ( const Mat44& m ) const
      {
        return x == m.x && y == m.y && z == m.z && w == m.w;
      }

      bool operator != ( const Mat44& m ) const
      {
        return x != m.x || y != m.y || z != m.z || w != m.w;
      }

      operator const float* () const
      {
        return &x.x;
      }

      operator float* ()
      {
        return &x.x;
      }

      const float& operator [] ( int i ) const
      {
        assert( 0 <= i && i < 16 );

        return ( &x.x )[i];
      }

      float& operator [] ( int i )
      {
        assert( 0 <= i && i < 16 );

        return ( &x.x )[i];
      }

      Mat44 operator ~ () const
      {
        return Mat44( float4( x.x, y.x, z.x, w.x ),
                      float4( x.y, y.y, z.y, w.y ),
                      float4( x.z, y.z, z.z, w.z ),
                      float4( x.w, y.w, z.w, w.w ) );
      }

      float det() const
      {
        float klop = z.z * w.w - w.z * z.w;
        float jlnp = y.z * w.w - w.z * y.w;
        float jkno = y.z * z.w - z.z * y.w;
        float ilmp = x.z * w.w - w.z * x.w;
        float ikmo = x.z * z.w - z.z * x.w;
        float ijmn = x.z * y.w - y.z * x.w;
        return
            x.x * y.y * klop -
            x.x * z.y * jlnp +
            x.x * w.y * jkno -
            y.x * x.y * klop +
            y.x * z.y * ilmp -
            y.x * w.y * ikmo +
            z.x * x.y * jlnp -
            z.x * y.y * ilmp +
            z.x * w.y * ijmn -
            w.x * x.y * jkno +
            w.x * y.y * ikmo -
            w.x * z.y * ijmn;
      }

      Mat44 operator + () const
      {
        return *this;
      }

      Mat44 operator - () const
      {
        return Mat44( -x, -y, -z, -w );
      }

      Mat44 operator + ( const Mat44& a ) const
      {
        return Mat44( x + a.x, y + a.y, z + a.z, w + a.w );
      }

      Mat44 operator - ( const Mat44& a ) const
      {
        return Mat44( x - a.x, y - a.y, z - a.z, w - a.w );
      }

      Mat44 operator * ( float k ) const
      {
        float4 k4 = float4( k, k, k, k );

        return Mat44( x.f4 * k4, y.f4 * k4, z.f4 * k4, w.f4 * k4 );
      }

      Mat44 operator / ( float k ) const
      {
        assert( k != 0.0f );

        k = 1.0f / k;
        float4 k4 = float4( k, k, k, k );

        return Mat44( x.f4 * k4, y.f4 * k4, z.f4 * k4, w.f4 * k4 );
      }

      Mat44& operator += ( const Mat44& a )
      {
        x.f4 += a.x.f4;
        y.f4 += a.y.f4;
        z.f4 += a.z.f4;
        w.f4 += a.w.f4;
        return *this;
      }

      Mat44& operator -= ( const Mat44& a )
      {
        x.f4 -= a.x.f4;
        y.f4 -= a.y.f4;
        z.f4 -= a.z.f4;
        w.f4 -= a.w.f4;
        return *this;
      }

      Mat44& operator *= ( float k )
      {
        float4 k4 = float4( k, k, k, k );

        x.f4 *= k4;
        y.f4 *= k4;
        z.f4 *= k4;
        w.f4 *= k4;
        return *this;
      }

      Mat44& operator /= ( float k )
      {
        assert( k != 0.0f );

        k = 1.0f / k;
        float4 k4 = float4( k, k, k, k );

        x.f4 *= k4;
        y.f4 *= k4;
        z.f4 *= k4;
        w.f4 *= k4;
        return *this;
      }

      Mat44 operator * ( const Mat44& m ) const
      {
        float4 xx = float4( m.x.x, m.x.x, m.x.x, m.x.x );
        float4 xy = float4( m.x.y, m.x.y, m.x.y, m.x.y );
        float4 xz = float4( m.x.z, m.x.z, m.x.z, m.x.z );
        float4 xw = float4( m.x.w, m.x.w, m.x.w, m.x.w );

        float4 yx = float4( m.y.x, m.y.x, m.y.x, m.y.x );
        float4 yy = float4( m.y.y, m.y.y, m.y.y, m.y.y );
        float4 yz = float4( m.y.z, m.y.z, m.y.z, m.y.z );
        float4 yw = float4( m.y.w, m.y.w, m.y.w, m.y.w );

        float4 zx = float4( m.z.x, m.z.x, m.z.x, m.z.x );
        float4 zy = float4( m.z.y, m.z.y, m.z.y, m.z.y );
        float4 zz = float4( m.z.z, m.z.z, m.z.z, m.z.z );
        float4 zw = float4( m.z.w, m.z.w, m.z.w, m.z.w );

        float4 wx = float4( m.w.x, m.w.x, m.w.x, m.w.x );
        float4 wy = float4( m.w.y, m.w.y, m.w.y, m.w.y );
        float4 wz = float4( m.w.z, m.w.z, m.w.z, m.w.z );
        float4 ww = float4( m.w.w, m.w.w, m.w.w, m.w.w );

        return Mat44( x.f4 * xx + y.f4 * xy + z.f4 * xz + w.f4 * xw,
                      x.f4 * yx + y.f4 * yy + z.f4 * yz + w.f4 * yw,
                      x.f4 * zx + y.f4 * zy + z.f4 * zz + w.f4 * zw,
                      x.f4 * wx + y.f4 * wy + z.f4 * wz + w.f4 * ww );
      }

      Vec4 operator * ( const Vec4& v ) const
      {
        float4 vx = float4( v.x, v.x, v.x, v.x );
        float4 vy = float4( v.y, v.y, v.y, v.y );
        float4 vz = float4( v.z, v.z, v.z, v.z );

        return Vec4( x.f4 * vx + y.f4 * vy + z.f4 * vz );
      }

      Point4 operator * ( const Point4& p ) const
      {
        float4 px = float4( p.x, p.x, p.x, p.x );
        float4 py = float4( p.y, p.y, p.y, p.y );
        float4 pz = float4( p.z, p.z, p.z, p.z );

        return Point4( x.f4 * px + y.f4 * py + z.f4 * pz + w.f4 );
      }

      Vec3 operator * ( const Vec3& v ) const
      {
        float4 vx = float4( v.x, v.x, v.x, v.x );
        float4 vy = float4( v.y, v.y, v.y, v.y );
        float4 vz = float4( v.z, v.z, v.z, v.z );

        Vec4 r = Vec4( x.f4 * vx + y.f4 * vy + z.f4 * vz );
        return Vec3( r );
      }

      // transformation matrices
      static Mat44 transl( float dx, float dy, float dz )
      {
        return Mat44( float4( 1.0f, 0.0f, 0.0f, 0.0f ),
                      float4( 0.0f, 1.0f, 0.0f, 0.0f ),
                      float4( 0.0f, 0.0f, 1.0f, 0.0f ),
                      float4(   dx,   dy,   dz, 1.0f ) );
      }

      static Mat44 transl( const Vec4& v )
      {
        return Mat44( float4( 1.0f, 0.0f, 0.0f, 0.0f ),
                      float4( 0.0f, 1.0f, 0.0f, 0.0f ),
                      float4( 0.0f, 0.0f, 1.0f, 0.0f ),
                      float4( 0.0f, 0.0f, 0.0f, 1.0f ) + v.f4 );
      }

      static Mat44 rotX( float theta )
      {
        float s, c;

        Math::sincos( theta, &s, &c );

        return Mat44( float4( 1.0f, 0.0f, 0.0f, 0.0f ),
                      float4( 0.0f,    c,    s, 0.0f ),
                      float4( 0.0f,   -s,    c, 0.0f ),
                      float4( 0.0f, 0.0f, 0.0f, 1.0f ) );
      }

      static Mat44 rotY( float theta )
      {
        float s, c;

        Math::sincos( theta, &s, &c );

        return Mat44( float4(    c, 0.0f,   -s, 0.0f ),
                      float4( 0.0f, 1.0f, 0.0f, 0.0f ),
                      float4(    s, 0.0f,    c, 0.0f ),
                      float4( 0.0f, 0.0f, 0.0f, 1.0f ) );
      }

      static Mat44 rotZ( float theta )
      {
        float s, c;

        Math::sincos( theta, &s, &c );

        return Mat44( float4(    c,    s, 0.0f, 0.0f ),
                      float4(   -s,    c, 0.0f, 0.0f ),
                      float4( 0.0f, 0.0f, 1.0f, 0.0f ),
                      float4( 0.0f, 0.0f, 0.0f, 1.0f ) );
      }

  };

  // declared in Quat.hpp
  inline Mat44 Quat::rotMat44() const
  {
    // this matrix is in column major format in implementation
    //
    // [ 1 - 2yy - 2zz    2xy - 2wz      2xz + 2wy    0 ]
    // [   2xy + 2wz    1 - 2xx - 2zz    2yz - 2wx    0 ]
    // [   2xz - 2wy      2yz + 2wx    1 - 2xx - 2yy  0 ]
    // [       0              0              0        1 ]
    //

    float x2 = x + x;
    float y2 = y + y;
    float z2 = z + z;
    float xx1 = 1.0f - x2 * x;
    float yy = y2 * y;
    float zz = z2 * z;
    float xy = x2 * y;
    float xz = x2 * z;
    float xw = x2 * w;
    float yz = y2 * z;
    float yw = y2 * w;
    float zw = z2 * w;

    return Mat44( float4( 1.0f - yy - zz, xy + zw,  xz - yw,  0.0f ),
                  float4( xy - zw,        xx1 - zz, yz + xw,  0.0f ),
                  float4( xz + yw,        yz - xw,  xx1 - yy, 0.0f ),
                  float4( 0.0f,           0.0f,     0.0f,     1.0f ) );
  }

  // declared in Quat.hpp
  inline Mat44 Quat::invRotMat44() const
  {
    // this matrix is in column major format in implementation
    //
    // [ 1 - 2yy - 2zz    2xy + 2wz      2xz - 2wy    0 ]
    // [   2xy - 2wz    1 - 2xx - 2zz    2yz + 2wx    0 ]
    // [   2xz + 2wy      2yz - 2wx    1 - 2xx - 2yy  0 ]
    // [       0              0              0        1 ]
    //

    float x2 = x + x;
    float y2 = y + y;
    float z2 = z + z;
    float xx1 = 1.0f - x2 * x;
    float yy = y2 * y;
    float zz = z2 * z;
    float xy = x2 * y;
    float xz = x2 * z;
    float xw = x2 * w;
    float yz = y2 * z;
    float yw = y2 * w;
    float zw = z2 * w;

    return Mat44( float4( 1.0f - yy - zz, xy - zw,  xz + yw,  0.0f ),
                  float4( xy + zw,        xx1 - zz, yz - xw,  0.0f ),
                  float4( xz - yw,        yz + xw,  xx1 - yy, 0.0f ),
                  float4( 0.0f,           0.0f,     0.0f,     1.0f ) );
  }

}
