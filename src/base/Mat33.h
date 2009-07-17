/*
 *  Mat33.h
 *
 *  Column-major 3x3 matrix
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  union Mat33
  {
    // WARNING: first index is column, second is line
    float m[9];

    struct
    {
      // first column (i base vector)
      float xx, xy, xz;
      // second column (j base vector)
      float yx, yy, yz;
      // third column (k base vector)
      float zx, zy, zz;
    };

    explicit Mat33()
    {}

    explicit Mat33( float xx_, float xy_, float xz_,
                    float yx_, float yy_, float yz_,
                    float zx_, float zy_, float zz_ ) :
        xx( xx_ ), xy( xy_ ), xz( xz_ ),
        yx( yx_ ), yy( yy_ ), yz( yz_ ),
        zx( zx_ ), zy( zy_ ), zz( zz_ )
    {}

    explicit Mat33( const float *v )
    {
      *this = *(Mat33*) v;
    }

    // implemented in Mat44.h
    explicit Mat33( const Mat44 &m );

    explicit Mat33( const Vec3 &a, const Vec3 &b, const Vec3 &c ) :
        xx( a.x ), xy( a.y ), xz( a.z ),
        yx( b.x ), yy( b.y ), yz( b.z ),
        zx( c.x ), zy( c.y ), zz( c.z )
    {}

    static Mat33 zero()
    {
      return Mat33( 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f );
    }

    static Mat33 id()
    {
      return Mat33( 1.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 1.0f );
    }

    operator float* ()
    {
      return m;
    }

    operator const float* () const
    {
      return m;
    }

    float &operator [] ( int i )
    {
      return m[i];
    }

    const float &operator [] ( int i ) const
    {
      return m[i];
    }

    Vec3 &x()
    {
      return *(Vec3*) &m[0];
    }

    const Vec3 &x() const
    {
      return *(const Vec3*) &m[0];
    }

    Vec3 &y()
    {
      return *(Vec3*) &m[3];
    }

    const Vec3 &y() const
    {
      return *(const Vec3*) &m[3];
    }

    Vec3 &z()
    {
      return *(Vec3*) &m[6];
    }

    const Vec3 &z() const
    {
      return *(const Vec3*) &m[6];
    }

    bool operator == ( const Mat33 &a ) const
    {
      return
          m[0] == a.m[0] &&
          m[1] == a.m[1] &&
          m[2] == a.m[2] &&
          m[3] == a.m[3] &&
          m[4] == a.m[4] &&
          m[5] == a.m[5] &&
          m[6] == a.m[6] &&
          m[7] == a.m[7] &&
          m[8] == a.m[8];
    }

    bool operator != ( const Mat33 &a ) const
    {
      return
          m[0] != a.m[0] ||
          m[1] != a.m[1] ||
          m[2] != a.m[2] ||
          m[3] != a.m[3] ||
          m[4] != a.m[4] ||
          m[5] != a.m[5] ||
          m[6] != a.m[6] ||
          m[7] != a.m[7] ||
          m[8] != a.m[8];
    }

    Mat33 operator + () const
    {
      return *this;
    }

    Mat33 operator - () const
    {
      return Mat33( -m[0], -m[1], -m[2],
                    -m[3], -m[4], -m[5],
                    -m[6], -m[7], -m[8] );
    }

    // determinant
    float det() const
    {
      return
          m[0] * ( m[4]*m[8] - m[5]*m[7] ) -
          m[3] * ( m[1]*m[8] - m[2]*m[7] ) +
          m[6] * ( m[1]*m[5] - m[2]*m[4] );
    }

    // return transposed matrix
    Mat33 operator ~ () const
    {
      return Mat33( m[0], m[3], m[6],
                    m[1], m[4], m[7],
                    m[2], m[5], m[8] );
    }

    // transpose
    Mat33 &trans()
    {
      swap( m[1], m[3] );
      swap( m[2], m[6] );
      swap( m[5], m[7] );
      return *this;
    }

    bool isZero() const
    {
      return
          m[0] == 0.0f &&
          m[1] == 0.0f &&
          m[2] == 0.0f &&
          m[3] == 0.0f &&
          m[4] == 0.0f &&
          m[5] == 0.0f &&
          m[6] == 0.0f &&
          m[7] == 0.0f &&
          m[8] == 0.0f;
    }

    Mat33 &setZero()
    {
      m[0] = 0.0f;
      m[1] = 0.0f;
      m[2] = 0.0f;
      m[3] = 0.0f;
      m[4] = 0.0f;
      m[5] = 0.0f;
      m[6] = 0.0f;
      m[7] = 0.0f;
      m[8] = 0.0f;
      return *this;
    }

    bool isId() const
    {
      return
          m[0] == 1.0f &&
          m[1] == 0.0f &&
          m[2] == 0.0f &&
          m[3] == 0.0f &&
          m[4] == 1.0f &&
          m[5] == 0.0f &&
          m[6] == 0.0f &&
          m[7] == 0.0f &&
          m[8] == 1.0f;
    }

    Mat33 &setId()
    {
      m[0] = 1.0f;
      m[1] = 0.0f;
      m[2] = 0.0f;
      m[3] = 0.0f;
      m[4] = 1.0f;
      m[5] = 0.0f;
      m[6] = 0.0f;
      m[7] = 0.0f;
      m[8] = 1.0f;
      return *this;
    }

    // assignment operators
    Mat33 &operator += ( const Mat33 &a )
    {
      m[0] += a.m[0];
      m[1] += a.m[1];
      m[2] += a.m[2];
      m[3] += a.m[3];
      m[4] += a.m[4];
      m[5] += a.m[5];
      m[6] += a.m[6];
      m[7] += a.m[7];
      m[8] += a.m[8];
      return *this;
    }

    Mat33 &operator -= ( const Mat33 &a )
    {
      m[0] -= a.m[0];
      m[1] -= a.m[1];
      m[2] -= a.m[2];
      m[3] -= a.m[3];
      m[4] -= a.m[4];
      m[5] -= a.m[5];
      m[6] -= a.m[6];
      m[7] -= a.m[7];
      m[8] -= a.m[8];
      return *this;
    }

    Mat33 &operator *= ( float k )
    {
      m[0] *= k;
      m[1] *= k;
      m[2] *= k;
      m[3] *= k;
      m[4] *= k;
      m[5] *= k;
      m[6] *= k;
      m[7] *= k;
      m[8] *= k;
      return *this;
    }

    Mat33 &operator /= ( float k )
    {
      assert( k != 0.0f );

      k = 1.0f / k;
      m[0] *= k;
      m[1] *= k;
      m[2] *= k;
      m[3] *= k;
      m[4] *= k;
      m[5] *= k;
      m[6] *= k;
      m[7] *= k;
      m[8] *= k;
      return *this;
    }

    // binary operators
    Mat33 operator + ( const Mat33 &a ) const
    {
      return Mat33( m[0] + a.m[0], m[1] + a.m[1], m[2] + a.m[2],
                    m[3] + a.m[3], m[4] + a.m[4], m[5] + a.m[5],
                    m[6] + a.m[6], m[7] + a.m[7], m[8] + a.m[8] );
    }

    Mat33 operator - ( const Mat33 &a ) const
    {
      return Mat33( m[0] - a.m[0], m[1] - a.m[1], m[2] - a.m[2],
                    m[3] - a.m[3], m[4] - a.m[4], m[5] - a.m[5],
                    m[6] - a.m[6], m[7] - a.m[7], m[8] - a.m[8] );
    }

    Mat33 operator * ( float k ) const
    {
      return Mat33( m[0] * k, m[1] * k, m[2] * k,
                    m[3] * k, m[4] * k, m[5] * k,
                    m[6] * k, m[7] * k, m[8] * k );
    }

    Mat33 operator / ( float k ) const
    {
      assert( k != 0.0f );

      k = 1.0f / k;
      return Mat33( m[0] * k, m[1] * k, m[2] * k,
                    m[3] * k, m[4] * k, m[5] * k,
                    m[6] * k, m[7] * k, m[8] * k );
    }

    Mat33 operator * ( const Mat33 &a ) const
    {
      return Mat33( m[0] * a.m[0] + m[3] * a.m[1] + m[6] * a.m[2],
                    m[1] * a.m[0] + m[4] * a.m[1] + m[7] * a.m[2],
                    m[2] * a.m[0] + m[5] * a.m[1] + m[8] * a.m[2],

                    m[0] * a.m[3] + m[3] * a.m[4] + m[6] * a.m[5],
                    m[1] * a.m[3] + m[4] * a.m[4] + m[7] * a.m[5],
                    m[2] * a.m[3] + m[5] * a.m[4] + m[8] * a.m[5],

                    m[0] * a.m[6] + m[3] * a.m[7] + m[6] * a.m[8],
                    m[1] * a.m[6] + m[4] * a.m[7] + m[7] * a.m[8],
                    m[2] * a.m[6] + m[5] * a.m[7] + m[8] * a.m[8] );
    }

    Vec3 operator * ( const Vec3 &v ) const
    {
      return Vec3( v.x * m[0] + v.y * m[3] + v.z * m[6],
                   v.x * m[1] + v.y * m[4] + v.z * m[7],
                   v.x * m[2] + v.y * m[5] + v.z * m[8] );
    }

    Vec3 invMultiply( const Vec3 &v ) const
    {
      return Vec3( v.x * m[0] + v.y * m[1] + v.z * m[2],
                   v.x * m[3] + v.y * m[4] + v.z * m[5],
                   v.x * m[6] + v.y * m[7] + v.z * m[8] );
    }

    friend Mat33 operator * ( float k, const Mat33 &a )
    {
      return Mat33( a.m[0] * k, a.m[1] * k, a.m[2] * k,
                    a.m[3] * k, a.m[4] * k, a.m[5] * k,
                    a.m[6] * k, a.m[7] * k, a.m[8] * k );
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

  // declared in Quat.h
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

  // declared in Quat.h
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
