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

      static Mat33 zero()
      {
        return Mat33( 0.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 0.0f );
      }

      bool isZero() const
      {
        return
            x.x == 0.0f &&
            x.y == 0.0f &&
            x.z == 0.0f &&
            y.x == 0.0f &&
            y.y == 0.0f &&
            y.z == 0.0f &&
            z.x == 0.0f &&
            z.y == 0.0f &&
            z.z == 0.0f;
      }

      Mat33& setZero()
      {
        x.x = 0.0f;
        x.y = 0.0f;
        x.z = 0.0f;
        y.x = 0.0f;
        y.y = 0.0f;
        y.z = 0.0f;
        z.x = 0.0f;
        z.y = 0.0f;
        z.z = 0.0f;
        return *this;
      }

      static Mat33 id()
      {
        return Mat33( 1.0f, 0.0f, 0.0f,
                      0.0f, 1.0f, 0.0f,
                      0.0f, 0.0f, 1.0f );
      }

      bool isId() const
      {
        return
            x.x == 1.0f &&
            x.y == 0.0f &&
            x.z == 0.0f &&
            y.x == 0.0f &&
            y.y == 1.0f &&
            y.z == 0.0f &&
            z.x == 0.0f &&
            z.y == 0.0f &&
            z.z == 1.0f;
      }

      Mat33& setId()
      {
        x.x = 1.0f;
        x.y = 0.0f;
        x.z = 0.0f;
        y.x = 0.0f;
        y.y = 1.0f;
        y.z = 0.0f;
        z.x = 0.0f;
        z.y = 0.0f;
        z.z = 1.0f;
        return *this;
      }

      // return transposed matrix
      Mat33 operator ~ () const
      {
        return Mat33( x.x, y.x, z.x,
                      x.y, y.y, z.y,
                      x.z, y.z, z.z );
      }

      // transpose
      Mat33& trans()
      {
        swap( x.y, y.x );
        swap( x.z, z.x );
        swap( y.z, z.y );
        return *this;
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
        return Mat33( -x.x, -x.y, -x.z,
                      -y.x, -y.y, -y.z,
                      -z.x, -z.y, -z.z );
      }

      Mat33 operator + ( const Mat33& a ) const
      {
        return Mat33( x.x + a.x.x, x.y + a.x.y, x.z + a.x.z,
                      y.x + a.y.x, y.y + a.y.y, y.z + a.y.z,
                      z.x + a.z.x, z.y + a.z.y, z.z + a.z.z );
      }

      Mat33 operator - ( const Mat33& a ) const
      {
        return Mat33( x.x - a.x.x, x.y - a.x.y, x.z - a.x.z,
                      y.x - a.y.x, y.y - a.y.y, y.z - a.y.z,
                      z.x - a.z.x, z.y - a.z.y, z.z - a.z.z );
      }

      Mat33 operator * ( float k ) const
      {
        return Mat33( x.x * k, x.y * k, x.z * k,
                      y.x * k, y.y * k, y.z * k,
                      z.x * k, z.y * k, z.z * k );
      }

      Mat33 operator / ( float k ) const
      {
        assert( k != 0.0f );

        k = 1.0f / k;
        return Mat33( x.x * k, x.y * k, x.z * k,
                      y.x * k, y.y * k, y.z * k,
                      z.x * k, z.y * k, z.z * k );
      }

      Mat33& operator += ( const Mat33& a )
      {
        x.x += a.x.x;
        x.y += a.x.y;
        x.z += a.x.z;
        y.x += a.y.x;
        y.y += a.y.y;
        y.z += a.y.z;
        z.x += a.z.x;
        z.y += a.z.y;
        z.z += a.z.z;
        return *this;
      }

      Mat33& operator -= ( const Mat33& a )
      {
        x.x -= a.x.x;
        x.y -= a.x.y;
        x.z -= a.x.z;
        y.x -= a.y.x;
        y.y -= a.y.y;
        y.z -= a.y.z;
        z.x -= a.z.x;
        z.y -= a.z.y;
        z.z -= a.z.z;
        return *this;
      }

      Mat33& operator *= ( float k )
      {
        x.x *= k;
        x.y *= k;
        x.z *= k;
        y.x *= k;
        y.y *= k;
        y.z *= k;
        z.x *= k;
        z.y *= k;
        z.z *= k;
        return *this;
      }

      Mat33& operator /= ( float k )
      {
        assert( k != 0.0f );

        k = 1.0f / k;
        x.x *= k;
        x.y *= k;
        x.z *= k;
        y.x *= k;
        y.y *= k;
        y.z *= k;
        z.x *= k;
        z.y *= k;
        z.z *= k;
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
        return Mat33( a.x.x * k, a.x.y * k, a.x.z * k,
                      a.y.x * k, a.y.y * k, a.y.z * k,
                      a.z.x * k, a.z.y * k, a.z.z * k );
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

    return Mat33( 1.0f - yy - zz, xy + zw,  xz - yw,
                  xy - zw,        xx1 - zz, yz + xw,
                  xz + yw,        yz - xw,  xx1 - yy );
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

    return Mat33( 1.0f - yy - zz, xy - zw,  xz + yw,
                  xy + zw,        xx1 - zz, yz - xw,
                  xz - yw,        yz + xw,  xx1 - yy );
  }

}
