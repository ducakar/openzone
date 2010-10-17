/*
 *  Mat33.hpp
 *
 *  Column-major 3x3 matrix
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

namespace oz
{

  class Mat33
  {
    public:

      static const Mat33 ZERO;
      static const Mat33 ID;

      Vec3 x;
      Vec3 y;
      Vec3 z;

      explicit Mat33()
      {}

      explicit Mat33( float xx, float xy, float xz,
                      float yx, float yy, float yz,
                      float zx, float zy, float zz ) :
          x( xx, xy, xz ),
          y( yx, yy, yz ),
          z( zx, zy, zz )
      {}

      explicit Mat33( const float* v ) : x( &v[0] ), y( &v[3] ), z( &v[6] )
      {}

      // implemented in Mat44.hpp
      explicit Mat33( const Mat44& m );

      explicit Mat33( const Vec3& a, const Vec3& b, const Vec3& c ) :
          x( a ), y( b ), z( c )
      {}

      bool operator == ( const Mat33& m ) const
      {
        return x == m.x && y == m.y && z == m.z;
      }

      bool operator != ( const Mat33& m ) const
      {
        return x != m.x || y != m.y || z != m.z;
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
        assert( 0 <= i && i < 9 );

        return ( &x.x )[i];
      }

      float& operator [] ( int i )
      {
        assert( 0 <= i && i < 9 );

        return ( &x.x )[i];
      }

      // return transposed matrix
      Mat33 operator ~ () const
      {
        return Mat33( x.x, y.x, z.x,
                      x.y, y.y, z.y,
                      x.z, y.z, z.z );
      }

      // determinant
      float det() const
      {
        return
            x.x * ( y.y*z.z - y.z*z.y ) -
            y.x * ( x.y*z.z - x.z*z.y ) +
            z.x * ( x.y*y.z - x.z*y.y );
      }

      Mat33 operator + () const
      {
        return *this;
      }

      Mat33 operator - () const
      {
        return Mat33( -x, -y, -z );
      }

      Mat33 operator + ( const Mat33& a ) const
      {
        return Mat33( x + a.x, y + a.y, z + a.z );
      }

      Mat33 operator - ( const Mat33& a ) const
      {
        return Mat33( x - a.x, y - a.y, z - a.z );
      }

      Mat33 operator * ( float k ) const
      {
        return Mat33( x * k, y * k, z * k );
      }

      Mat33 operator / ( float k ) const
      {
        assert( k != 0.0f );

        k = 1.0f / k;
        return Mat33( x * k, y * k, z * k );
      }

      Mat33& operator += ( const Mat33& a )
      {
        x += a.x;
        y += a.y;
        z += a.z;
        return *this;
      }

      Mat33& operator -= ( const Mat33& a )
      {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        return *this;
      }

      Mat33& operator *= ( float k )
      {
        x *= k;
        y *= k;
        z *= k;
        return *this;
      }

      Mat33& operator /= ( float k )
      {
        assert( k != 0.0f );

        k = 1.0f / k;
        x *= k;
        y *= k;
        z *= k;
        return *this;
      }

      Mat33 operator * ( const Mat33& a ) const
      {
        return Mat33( x.x * a.x.x + y.x * a.x.y + z.x * a.x.z,
                      x.y * a.x.x + y.y * a.x.y + z.y * a.x.z,
                      x.z * a.x.x + y.z * a.x.y + z.z * a.x.z,

                      x.x * a.y.x + y.x * a.y.y + z.x * a.y.z,
                      x.y * a.y.x + y.y * a.y.y + z.y * a.y.z,
                      x.z * a.y.x + y.z * a.y.y + z.z * a.y.z,

                      x.x * a.z.x + y.x * a.z.y + z.x * a.z.z,
                      x.y * a.z.x + y.y * a.z.y + z.y * a.z.z,
                      x.z * a.z.x + y.z * a.z.y + z.z * a.z.z );
      }

      Vec3 operator * ( const Vec3& v ) const
      {
        return Vec3( x.x * v.x + y.x * v.y + z.x * v.z,
                     x.y * v.x + y.y * v.y + z.y * v.z,
                     x.z * v.x + y.z * v.y + z.z * v.z );
      }

      Vec3 operator / ( const Vec3& v ) const
      {
        return Vec3( x.x * v.x + x.y * v.y + x.z * v.z,
                     y.x * v.x + y.y * v.y + y.z * v.z,
                     z.x * v.x + z.y * v.y + z.z * v.z );
      }

      friend Mat33 operator * ( float k, const Mat33& a )
      {
        return Mat33( a.x * k, a.y * k, a.z * k );
      }

      // make matrix for rotation around x-axis
      static Mat33 rotX( float theta )
      {
        float s, c;

        Math::sincos( theta, &s, &c );

        return Mat33( 1.0f, 0.0f, 0.0f,
                      0.0f,   c,   s,
                      0.0f,  -s,   c );
      }

      // make matrix for rotation around y-axis
      static Mat33 rotY( float theta )
      {
        float s, c;

        Math::sincos( theta, &s, &c );

        return Mat33(    c, 0.0f,   -s,
                      0.0f, 1.0f, 0.0f,
                         s, 0.0f,    c );
      }

      // make matrix for rotation around z-axis
      static Mat33 rotZ( float theta )
      {
        float s, c;

        Math::sincos( theta, &s, &c );

        return Mat33(    c,    s, 0.0f,
                        -s,    c, 0.0f,
                      0.0f, 0.0f, 1.0f );
    }

  };

  // declared in Quat.hpp
  inline Mat33 Quat::rotMat33() const
  {
    // this matrix is in column major format in implementation
    //
    // [ 1 - 2yy - 2zz    2xy - 2wz      2xz + 2wy   ]
    // [   2xy + 2wz    1 - 2xx - 2zz    2yz - 2wx   ]
    // [   2xz - 2wy      2yz + 2wx    1 - 2xx - 2yy ]
    //

    float x2 = 2.0f * x;
    float y2 = 2.0f * y;
    float z2 = 2.0f * z;

    float xx2 = x2 * x;
    float xy2 = x2 * y;
    float xz2 = x2 * z;
    float xw2 = x2 * w;
    float yy2 = y2 * y;
    float yz2 = y2 * z;
    float yw2 = y2 * w;
    float zz2 = z2 * z;
    float zw2 = z2 * w;

    float xx2_1 = 1.0f - xx2;
    float yy2_1 = 1.0f - yy2;

    return Mat33( yy2_1 - zz2, xy2 + zw2,   xz2 - yw2,
                  xy2 - zw2,   xx2_1 - zz2, yz2 + xw2,
                  xz2 + yw2,   yz2 - xw2,   xx2_1 - yy2 );
  }

  // declared in Quat.hpp
  inline Mat33 Quat::invRotMat33() const
  {
    // this matrix is in column major format in implementation
    //
    // [ 1 - 2yy - 2zz    2xy + 2wz      2xz - 2wy   ]
    // [   2xy - 2wz    1 - 2xx - 2zz    2yz + 2wx   ]
    // [   2xz + 2wy      2yz - 2wx    1 - 2xx - 2yy ]
    //

    float x2 = 2.0f * x;
    float y2 = 2.0f * y;
    float z2 = 2.0f * z;

    float xx2 = x2 * x;
    float xy2 = x2 * y;
    float xz2 = x2 * z;
    float xw2 = x2 * w;
    float yy2 = y2 * y;
    float yz2 = y2 * z;
    float yw2 = y2 * w;
    float zz2 = z2 * z;
    float zw2 = z2 * w;

    float xx2_1 = 1.0f - xx2;
    float yy2_1 = 1.0f - yy2;

    return Mat33( yy2_1 - zz2, xy2 - zw2,   xz2 + yw2,
                  xy2 + zw2,   xx2_1 - zz2, yz2 - xw2,
                  xz2 - yw2,   yz2 + xw2,   xx2_1 - yy2 );
  }

}
