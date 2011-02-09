/*
 *  Mat44.hpp
 *
 *  Column-major 4x4 matrix
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "Quat.hpp"
#include "Point3.hpp"

namespace oz
{

#ifdef OZ_SIMD

  class Mat44
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

      OZ_ALWAYS_INLINE
      explicit Mat44()
      {}

      OZ_ALWAYS_INLINE
      explicit Mat44( float4 a, float4 b, float4 c, float4 d ) : x( a ), y( b ), z( c ), w( d )
      {}

      OZ_ALWAYS_INLINE
      explicit Mat44( int4 a, int4 b, int4 c, int4 d ) : x( a ), y( b ), z( c ), w( d )
      {}

      OZ_ALWAYS_INLINE
      explicit Mat44( const Quat& a, const Quat& b, const Quat& c, const Quat& d ) :
          x( a ), y( b ), z( c ), w( d )
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

        return Mat44( x / k, y / k, z / k, w / k );
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

        x /= k;
        y /= k;
        z /= k;
        w /= k;
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
        return Vec3( x * v.x + y * v.y + z * v.z );
      }

      OZ_ALWAYS_INLINE
      Point3 operator * ( const Point3& p ) const
      {
        return Point3( x * p.x + y * p.y + z * p.z + w );
      }

      // transformation matrices
      static Mat44 transl( float dx, float dy, float dz )
      {
        return Mat44( float4( 1.0f, 0.0f, 0.0f, 0.0f ),
                      float4( 0.0f, 1.0f, 0.0f, 0.0f ),
                      float4( 0.0f, 0.0f, 1.0f, 0.0f ),
                      float4(   dx,   dy,   dz, 1.0f ) );
      }

      static Mat44 transl( const Vec3& v )
      {
        return Mat44( float4( 1.0f, 0.0f, 0.0f, 0.0f ),
                      float4( 0.0f, 1.0f, 0.0f, 0.0f ),
                      float4( 0.0f, 0.0f, 1.0f, 0.0f ),
                      float4(  v.x,  v.y,  v.z, 1.0f ) );
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

#else

  class Mat44
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

      OZ_ALWAYS_INLINE
      explicit Mat44()
      {}

      OZ_ALWAYS_INLINE
      explicit Mat44( const Quat& a, const Quat& b, const Quat& c, const Quat& d ) :
          x( a ), y( b ), z( c ), w( d )
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
        return Vec3( x * v.x + y * v.y + z * v.z );
      }

      OZ_ALWAYS_INLINE
      Point3 operator * ( const Point3& p ) const
      {
        return Point3( x * p.x + y * p.y + z * p.z + w );
      }

      // transformation matrices
      static Mat44 transl( float dx, float dy, float dz )
      {
        return Mat44( 1.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 1.0f, 0.0f,
                        dx,   dy,   dz, 1.0f );
      }

      static Mat44 transl( const Vec3& v )
      {
        return Mat44( 1.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 1.0f, 0.0f,
                       v.x,  v.y,  v.z, 1.0f );
      }

      static Mat44 rotX( float theta )
      {
        float s, c;

        Math::sincos( theta, &s, &c );

        return Mat44( 1.0f, 0.0f, 0.0f, 0.0f,
                      0.0f,    c,    s, 0.0f,
                      0.0f,   -s,    c, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f );
      }

      static Mat44 rotY( float theta )
      {
        float s, c;

        Math::sincos( theta, &s, &c );

        return Mat44(    c, 0.0f,   -s, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f,
                         s, 0.0f,    c, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f );
      }

      static Mat44 rotZ( float theta )
      {
        float s, c;

        Math::sincos( theta, &s, &c );

        return Mat44(    c,    s, 0.0f, 0.0f,
                        -s,    c, 0.0f, 0.0f,
                      0.0f, 0.0f, 1.0f, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f );
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

    return Mat44( 1.0f - yy - zz, xy + zw,  xz - yw,  0.0f,
                  xy - zw,        xx1 - zz, yz + xw,  0.0f,
                  xz + yw,        yz - xw,  xx1 - yy, 0.0f,
                  0.0f,           0.0f,     0.0f,     1.0f );
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

    return Mat44( 1.0f - yy - zz, xy - zw,  xz + yw,  0.0f,
                  xy + zw,        xx1 - zz, yz - xw,  0.0f,
                  xz - yw,        yz + xw,  xx1 - yy, 0.0f,
                  0.0f,           0.0f,     0.0f,     1.0f );
  }

#endif

}
