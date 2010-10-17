/*
 *  Mat44.hpp
 *
 *  Column-major 4x4 matrix
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

namespace oz
{

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

      explicit Mat44()
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

      explicit Mat44( const Mat33& m ) :
          x( m.x.x, m.x.y, m.x.z, 0.0f ),
          y( m.y.x, m.y.y, m.y.z, 0.0f ),
          z( m.z.x, m.z.y, m.z.z, 0.0f ),
          w(  0.0f,  0.0f,  0.0f, 1.0f )
      {}

      explicit Mat44( const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d ) :
          x( a.x, a.y, a.z, 0.0f ),
          y( b.x, b.y, b.z, 0.0f ),
          z( c.x, c.y, c.z, 0.0f ),
          w( d.x, d.y, d.z, 1.0f )
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
        return Mat44( x * k, y * k, z * k, w * k );
      }

      Mat44 operator / ( float k ) const
      {
        assert( k != 0.0f );

        k = 1.0f / k;
        return Mat44( x * k, y * k, z * k, w * k );
      }

      Mat44& operator += ( const Mat44& a )
      {
        x += a.x;
        y += a.y;
        z += a.z;
        w += a.w;
        return *this;
      }

      Mat44& operator -= ( const Mat44& a )
      {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        w -= a.w;
        return *this;
      }

      Mat44& operator *= ( float k )
      {
        x *= k;
        y *= k;
        z *= k;
        w *= k;
        return *this;
      }

      Mat44& operator /= ( float k )
      {
        assert( k != 0.0f );

        k = 1.0f / k;
        x *= k;
        y *= k;
        z *= k;
        w *= k;
        return *this;
      }

      Mat44 operator * ( const Mat44& a ) const
      {
        return Mat44( x.x * a.x.x + y.x * a.x.y + z.x * a.x.z + w.x * a.x.w,
                      x.y * a.x.x + y.y * a.x.y + z.y * a.x.z + w.y * a.x.w,
                      x.z * a.x.x + y.z * a.x.y + z.z * a.x.z + w.z * a.x.w,
                      x.w * a.x.x + y.w * a.x.y + z.w * a.x.z + w.w * a.x.w,

                      x.x * a.y.x + y.x * a.y.y + z.x * a.y.z + w.x * a.y.w,
                      x.y * a.y.x + y.y * a.y.y + z.y * a.y.z + w.y * a.y.w,
                      x.z * a.y.x + y.z * a.y.y + z.z * a.y.z + w.z * a.y.w,
                      x.w * a.y.x + y.w * a.y.y + z.w * a.y.z + w.w * a.y.w,

                      x.x * a.z.x + y.x * a.z.y + z.x * a.z.z + w.x * a.z.w,
                      x.y * a.z.x + y.y * a.z.y + z.y * a.z.z + w.y * a.z.w,
                      x.z * a.z.x + y.z * a.z.y + z.z * a.z.z + w.z * a.z.w,
                      x.w * a.z.x + y.w * a.z.y + z.w * a.z.z + w.w * a.z.w,

                      x.x * a.w.x + y.x * a.w.y + z.x * a.w.z + w.x * a.w.w,
                      x.y * a.w.x + y.y * a.w.y + z.y * a.w.z + w.y * a.w.w,
                      x.z * a.w.x + y.z * a.w.y + z.z * a.w.z + w.z * a.w.w,
                      x.w * a.w.x + y.w * a.w.y + z.w * a.w.z + w.w * a.w.w );
      }

      Vec3 operator * ( const Vec3& v ) const
      {
        return Vec3( x.x * v.x + y.x * v.y + z.x * v.z + w.x,
                     x.y * v.x + y.y * v.y + z.y * v.z + w.y,
                     x.z * v.x + y.z * v.y + z.z * v.z + w.z );
      }

      Vec3 operator / ( const Vec3& v ) const
      {
        return Vec3( x.x * v.x + x.y * v.y + x.z * v.z + x.w,
                     y.x * v.x + y.y * v.y + y.z * v.z + y.w,
                     z.x * v.x + z.y * v.y + z.z * v.z + z.w );
      }

      friend Mat44 operator * ( float k, const Mat44& a )
      {
        return Mat44( a.x * k, a.y * k, a.z * k, a.w * k );
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

  // declared in Mat33.hpp
  inline Mat33::Mat33( const Mat44& m ) :
      x( m.x ), y( m.y ), z( m.z )
  {}

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

}
