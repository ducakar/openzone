/*
 *  Mat44.hpp
 *
 *  Column-major 4x4 matrix
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "Quat.hpp"
#include "Point3.hpp"

namespace oz
{

  class Mat44
  {
    public:

      static const Mat44 ZERO;
      static const Mat44 ID;

      // first column (i base vector)
      Vec4 x;
      // second column (j base vector)
      Vec4 y;
      // third column (k base vector)
      Vec4 z;
      // last column (translation)
      Vec4 w;

      OZ_ALWAYS_INLINE
      Mat44()
      {}

      OZ_ALWAYS_INLINE
      explicit Mat44( const Vec4& a, const Vec4& b, const Vec4& c, const Vec4& d ) :
          x( a ), y( b ), z( c ), w( d )
      {}

      OZ_ALWAYS_INLINE
      explicit Mat44( const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d ) :
          x( a, 0.0f ), y( b, 0.0f ), z( c, 0.0f ), w( d, 1.0f )
      {}

      OZ_ALWAYS_INLINE
      explicit Mat44( float xx, float xy, float xz, float xw,
                      float yx, float yy, float yz, float yw,
                      float zx, float zy, float zz, float zw,
                      float wx, float wy, float wz, float ww ) :
          x( xx, xy, xz, xw ),
          y( yx, yy, yz, yw ),
          z( zx, zy, zz, zw ),
          w( wx, wy, wz, ww )
      {}

      OZ_ALWAYS_INLINE
      explicit Mat44( const float* v ) : x( &v[0] ), y( &v[4] ), z( &v[8] ), w( &v[12] )
      {}

      OZ_ALWAYS_INLINE
      bool operator == ( const Mat44& m ) const
      {
        return x == m.x && y == m.y && z == m.z && w == m.w;
      }

      OZ_ALWAYS_INLINE
      bool operator != ( const Mat44& m ) const
      {
        return x != m.x || y != m.y || z != m.z || w != m.w;
      }

      OZ_ALWAYS_INLINE
      operator const float* () const
      {
        return &x.x;
      }

      OZ_ALWAYS_INLINE
      operator float* ()
      {
        return &x.x;
      }

      OZ_ALWAYS_INLINE
      const float& operator [] ( int i ) const
      {
        hard_assert( 0 <= i && i < 16 );

        return ( &x.x )[i];
      }

      OZ_ALWAYS_INLINE
      float& operator [] ( int i )
      {
        hard_assert( 0 <= i && i < 16 );

        return ( &x.x )[i];
      }

      OZ_ALWAYS_INLINE
      Mat44 operator ~ () const
      {
        return Mat44( x.x, y.x, z.x, w.x,
                      x.y, y.y, z.y, w.y,
                      x.z, y.z, z.z, w.z,
                      x.w, y.w, z.w, w.w );
      }

      OZ_ALWAYS_INLINE
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

      OZ_ALWAYS_INLINE
      Mat44 operator + () const
      {
        return *this;
      }

      OZ_ALWAYS_INLINE
      Mat44 operator - () const
      {
        return Mat44( -x, -y, -z, -w );
      }

      OZ_ALWAYS_INLINE
      Mat44 operator + ( const Mat44& a ) const
      {
        return Mat44( x + a.x, y + a.y, z + a.z, w + a.w );
      }

      OZ_ALWAYS_INLINE
      Mat44 operator - ( const Mat44& a ) const
      {
        return Mat44( x - a.x, y - a.y, z - a.z, w - a.w );
      }

      OZ_ALWAYS_INLINE
      Mat44 operator * ( float k ) const
      {
        return Mat44( x * k, y * k, z * k, w * k );
      }

      OZ_ALWAYS_INLINE
      Mat44 operator / ( float k ) const
      {
        hard_assert( k != 0.0f );

        k = 1.0f / k;
        return Mat44( x * k, y * k, z * k, w * k );
      }

      OZ_ALWAYS_INLINE
      Mat44& operator += ( const Mat44& a )
      {
        x += a.x;
        y += a.y;
        z += a.z;
        w += a.w;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Mat44& operator -= ( const Mat44& a )
      {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        w -= a.w;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Mat44& operator *= ( float k )
      {
        x *= k;
        y *= k;
        z *= k;
        w *= k;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Mat44& operator /= ( float k )
      {
        hard_assert( k != 0.0f );

        k = 1.0f / k;
        x *= k;
        y *= k;
        z *= k;
        w *= k;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Mat44 operator * ( const Mat44& m ) const
      {
        return Mat44( x * m.x.x + y * m.x.y + z * m.x.z + w * m.x.w,
                      x * m.y.x + y * m.y.y + z * m.y.z + w * m.y.w,
                      x * m.z.x + y * m.z.y + z * m.z.z + w * m.z.w,
                      x * m.w.x + y * m.w.y + z * m.w.z + w * m.w.w );
      }

      OZ_ALWAYS_INLINE
      Vec3 operator * ( const Vec3& v ) const
      {
        const Vec3& i = x;
        const Vec3& j = y;
        const Vec3& k = z;

        return i * v.x + j * v.y + k * v.z;
      }

      OZ_ALWAYS_INLINE
      Point3 operator * ( const Point3& p ) const
      {
        const Vec3& i = x;
        const Vec3& j = y;
        const Vec3& k = z;
        const Vec3& t = w;

        return Point3::ORIGIN + i * p.x + j * p.y + k * p.z + t;
      }

      OZ_ALWAYS_INLINE
      void translate( const Vec3& v )
      {
        const Vec3& i = x;
        const Vec3& j = y;
        const Vec3& k = z;
        const Vec3& t = w;

        w = Vec4( i * v.x + j * v.y + k * v.z + t, 1.0f );
      }

      OZ_ALWAYS_INLINE
      void rotate( const Quat& q )
      {
        *this = *this * rotation( q );
      }

      OZ_ALWAYS_INLINE
      void rotateX( float theta )
      {
        Vec3 j = y;
        Vec3 k = z;

        float s, c;
        Math::sincos( theta, &s, &c );

        y = Vec4( j * c + k * s, 0.0f );
        z = Vec4( k * c - j * s, 0.0f );
      }

      OZ_ALWAYS_INLINE
      void rotateY( float theta )
      {
        Vec3 i = x;
        Vec3 k = z;

        float s, c;
        Math::sincos( theta, &s, &c );

        x = Vec4( i * c - k * s, 0.0f );
        z = Vec4( k * c + i * s, 0.0f );
      }

      OZ_ALWAYS_INLINE
      void rotateZ( float theta )
      {
        Vec3 i = x;
        Vec3 j = y;

        float s, c;
        Math::sincos( theta, &s, &c );

        x = Vec4( i * c + j * s, 0.0f );
        y = Vec4( j * c - i * s, 0.0f );
      }

      OZ_ALWAYS_INLINE
      void scale( const Vec3& v )
      {
        Vec3& i = x;
        Vec3& j = y;
        Vec3& k = z;

        i *= v.x;
        j *= v.y;
        k *= v.z;
      }

      // translation for vector v
      OZ_ALWAYS_INLINE
      static Mat44 translation( const Vec3& v )
      {
        return Mat44( 1.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 1.0f, 0.0f,
                       v.x,  v.y,  v.z, 1.0f );
      }

      // rotation in matrix form for quaternion q
      OZ_ALWAYS_INLINE
      static Mat44 rotation( const Quat& q )
      {
        //
        // [ 1 - 2yy - 2zz    2xy - 2wz      2xz + 2wy    0 ]
        // [   2xy + 2wz    1 - 2xx - 2zz    2yz - 2wx    0 ]
        // [   2xz - 2wy      2yz + 2wx    1 - 2xx - 2yy  0 ]
        // [       0              0              0        1 ]
        //

        float x2 = q.x + q.x;
        float y2 = q.y + q.y;
        float z2 = q.z + q.z;
        float xx1 = 1.0f - x2 * q.x;
        float yy = y2 * q.y;
        float zz = z2 * q.z;
        float xy = x2 * q.y;
        float xz = x2 * q.z;
        float xw = x2 * q.w;
        float yz = y2 * q.z;
        float yw = y2 * q.w;
        float zw = z2 * q.w;

        return Mat44( 1.0f - yy - zz, xy + zw,  xz - yw,  0.0f,
                      xy - zw,        xx1 - zz, yz + xw,  0.0f,
                      xz + yw,        yz - xw,  xx1 - yy, 0.0f,
                      0.0f,           0.0f,     0.0f,     1.0f );
      }

      OZ_ALWAYS_INLINE
      static Mat44 rotationX( float theta )
      {
        float s, c;
        Math::sincos( theta, &s, &c );

        return Mat44( 1.0f, 0.0f, 0.0f, 0.0f,
                      0.0f,    c,    s, 0.0f,
                      0.0f,   -s,    c, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f );
      }

      OZ_ALWAYS_INLINE
      static Mat44 rotationY( float theta )
      {
        float s, c;
        Math::sincos( theta, &s, &c );

        return Mat44(    c, 0.0f,   -s, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f,
                         s, 0.0f,    c, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f );
      }

      OZ_ALWAYS_INLINE
      static Mat44 rotationZ( float theta )
      {
        float s, c;
        Math::sincos( theta, &s, &c );

        return Mat44(    c,    s, 0.0f, 0.0f,
                        -s,    c, 0.0f, 0.0f,
                      0.0f, 0.0f, 1.0f, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f );
      }

      OZ_ALWAYS_INLINE
      static Mat44 scaling( const Vec3& v )
      {
        return Mat44(  v.x, 0.0f, 0.0f, 0.0f,
                      0.0f,  v.y, 0.0f, 0.0f,
                      0.0f, 0.0f,  v.z, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f );
      }

  };

}
