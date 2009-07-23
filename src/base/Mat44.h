/*
 *  Mat44.h
 *
 *  Column-major 4x4 matrix
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  union Mat44
  {
    // WARNING: first index is column, second is line
    float m[16];

    struct
    {
      // first column (i base vector)
      float xx, xy, xz, xw;
      // second column (j base vector)
      float yx, yy, yz, yw;
      // third column (k base vector)
      float zx, zy, zz, zw;
      // last column (usually used for position)
      float px, py, pz, pw;
    };

    explicit Mat44()
    {}

    explicit Mat44( float xx_, float xy_, float xz_, float xw_,
                    float yx_, float yy_, float yz_, float yw_,
                    float zx_, float zy_, float zz_, float zw_,
                    float px_, float py_, float pz_, float pw_ ) :
        xx( xx_ ), xy( xy_ ), xz( xz_ ), xw( xw_ ),
        yx( yx_ ), yy( yy_ ), yz( yz_ ), yw( yw_ ),
        zx( zx_ ), zy( zy_ ), zz( zz_ ), zw( zw_ ),
        px( px_ ), py( py_ ), pz( pz_ ), pw( pw_ )
    {}

    explicit Mat44( const float *v )
    {
      *this = *(Mat44*) v;
    }

    explicit Mat44( const Mat33 &m ) :
        xx( m.xx ), xy( m.xy ), xz( m.xz ), xw( 0.0f ),
        yx( m.yx ), yy( m.yy ), yz( m.yz ), yw( 0.0f ),
        zx( m.zx ), zy( m.zy ), zz( m.zz ), zw( 0.0f ),
        px( 0.0f ), py( 0.0f ), pz( 0.0f ), pw( 1.0f )
    {}

    explicit Mat44( const Vec3 &a, const Vec3 &b, const Vec3 &c, const Vec3 &d ) :
        xx( a.x ), xy( a.y ), xz( a.z ), xw( 0.0f ),
        yx( b.x ), yy( b.y ), yz( b.z ), yw( 0.0f ),
        zx( c.x ), zy( c.y ), zz( c.z ), zw( 0.0f ),
        px( d.x ), py( d.y ), pz( d.z ), pw( 1.0f )
    {}

    explicit Mat44( const Quat &a, const Quat &b, const Quat &c, const Quat &d ) :
        xx( a.x ), xy( a.y ), xz( a.z ), xw( a.w ),
        yx( b.x ), yy( b.y ), yz( b.z ), yw( b.w ),
        zx( c.x ), zy( c.y ), zz( c.z ), zw( c.w ),
        px( d.x ), py( d.y ), pz( d.z ), pw( d.w )
    {}

    static Mat44 zero()
    {
      return Mat44( 0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f );
    }

    static Mat44 id()
    {
      return Mat44( 1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f );
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
      return *(Vec3*) &m[4];
    }

    const Vec3 &y() const
    {
      return *(const Vec3*) &m[4];
    }

    Vec3 &z()
    {
      return *(Vec3*) &m[8];
    }

    const Vec3 &z() const
    {
      return *(const Vec3*) &m[8];
    }

    Vec3 &p()
    {
      return *(Vec3*) &m[12];
    }

    const Vec3 &p() const
    {
      return *(const Vec3*) &m[12];
    }

    Mat44 operator + () const
    {
      return *this;
    }

    Mat44 operator - () const
    {
      return Mat44( -m[ 0], -m[ 1], -m[ 2], -m[ 3],
                    -m[ 4], -m[ 5], -m[ 6], -m[ 7],
                    -m[ 8], -m[ 9], -m[10], -m[11],
                    -m[12], -m[13], -m[14], -m[15] );
    }

    float det() const
    {
      float klop = m[10] * m[15] - m[14] * m[11];
      float jlnp = m[ 6] * m[15] - m[14] * m[ 7];
      float jkno = m[ 6] * m[11] - m[10] * m[ 7];
      float ilmp = m[ 2] * m[15] - m[14] * m[ 3];
      float ikmo = m[ 2] * m[11] - m[10] * m[ 3];
      float ijmn = m[ 2] * m[ 7] - m[ 6] * m[ 3];
      return
          m[ 0] * m[ 5] * klop -
          m[ 0] * m[ 9] * jlnp +
          m[ 0] * m[13] * jkno -
          m[ 4] * m[ 1] * klop +
          m[ 4] * m[ 9] * ilmp -
          m[ 4] * m[13] * ikmo +
          m[ 8] * m[ 1] * jlnp -
          m[ 8] * m[ 5] * ilmp +
          m[ 8] * m[13] * ijmn -
          m[12] * m[ 1] * jkno +
          m[12] * m[ 5] * ikmo -
          m[12] * m[ 9] * ijmn;
    }

    Mat44 operator ~ () const
    {
      return Mat44( m[ 0], m[ 4], m[ 8], m[12],
                    m[ 1], m[ 5], m[ 9], m[13],
                    m[ 2], m[ 6], m[10], m[14],
                    m[ 3], m[ 7], m[11], m[15] );
    }

    Mat44 &trans()
    {
      swap( m[ 1], m[ 4] );
      swap( m[ 2], m[ 8] );
      swap( m[ 3], m[12] );
      swap( m[ 6], m[ 9] );
      swap( m[ 7], m[13] );
      swap( m[11], m[14] );
      return *this;
    }

    bool isZero() const
    {
      return
          m[ 0] == 0.0f &&
          m[ 1] == 0.0f &&
          m[ 2] == 0.0f &&
          m[ 3] == 0.0f &&
          m[ 4] == 0.0f &&
          m[ 5] == 0.0f &&
          m[ 6] == 0.0f &&
          m[ 7] == 0.0f &&
          m[ 8] == 0.0f &&
          m[ 9] == 0.0f &&
          m[10] == 0.0f &&
          m[11] == 0.0f &&
          m[12] == 0.0f &&
          m[13] == 0.0f &&
          m[14] == 0.0f &&
          m[15] == 0.0f;
    }

    Mat44 &setZero()
    {
      m[ 0] = 0.0f;
      m[ 1] = 0.0f;
      m[ 2] = 0.0f;
      m[ 3] = 0.0f;
      m[ 4] = 0.0f;
      m[ 5] = 0.0f;
      m[ 6] = 0.0f;
      m[ 7] = 0.0f;
      m[ 8] = 0.0f;
      m[ 9] = 0.0f;
      m[10] = 0.0f;
      m[11] = 0.0f;
      m[12] = 0.0f;
      m[13] = 0.0f;
      m[14] = 0.0f;
      m[15] = 0.0f;
      return *this;
    }

    bool isId() const
    {
      return
          m[ 0] == 1.0f &&
          m[ 1] == 0.0f &&
          m[ 2] == 0.0f &&
          m[ 3] == 0.0f &&
          m[ 4] == 0.0f &&
          m[ 5] == 1.0f &&
          m[ 6] == 0.0f &&
          m[ 7] == 0.0f &&
          m[ 8] == 0.0f &&
          m[ 9] == 0.0f &&
          m[10] == 1.0f &&
          m[11] == 0.0f &&
          m[12] == 0.0f &&
          m[13] == 0.0f &&
          m[14] == 0.0f &&
          m[15] == 1.0f;
    }

    Mat44 &setId()
    {
      m[ 0] = 1.0f;
      m[ 1] = 0.0f;
      m[ 2] = 0.0f;
      m[ 3] = 0.0f;
      m[ 4] = 0.0f;
      m[ 5] = 1.0f;
      m[ 6] = 0.0f;
      m[ 7] = 0.0f;
      m[ 8] = 0.0f;
      m[ 9] = 0.0f;
      m[10] = 1.0f;
      m[11] = 0.0f;
      m[12] = 0.0f;
      m[13] = 0.0f;
      m[14] = 0.0f;
      m[15] = 1.0f;
      return *this;
    }

    Mat44 &operator += ( const Mat44 &a )
    {
      m[ 0] += a.m[ 0];
      m[ 1] += a.m[ 1];
      m[ 2] += a.m[ 2];
      m[ 3] += a.m[ 3];
      m[ 4] += a.m[ 4];
      m[ 5] += a.m[ 5];
      m[ 6] += a.m[ 6];
      m[ 7] += a.m[ 7];
      m[ 8] += a.m[ 8];
      m[ 9] += a.m[ 9];
      m[10] += a.m[10];
      m[11] += a.m[11];
      m[12] += a.m[12];
      m[13] += a.m[13];
      m[14] += a.m[14];
      m[15] += a.m[15];
      return *this;
    }

    Mat44 &operator -= ( const Mat44 &a )
    {
      m[ 0] -= a.m[ 0];
      m[ 1] -= a.m[ 1];
      m[ 2] -= a.m[ 2];
      m[ 3] -= a.m[ 3];
      m[ 4] -= a.m[ 4];
      m[ 5] -= a.m[ 5];
      m[ 6] -= a.m[ 6];
      m[ 7] -= a.m[ 7];
      m[ 8] -= a.m[ 8];
      m[ 9] -= a.m[ 9];
      m[10] -= a.m[10];
      m[11] -= a.m[11];
      m[12] -= a.m[12];
      m[13] -= a.m[13];
      m[14] -= a.m[14];
      m[15] -= a.m[15];
      return *this;
    }

    Mat44 &operator *= ( float k )
    {
      m[ 0] *= k;
      m[ 1] *= k;
      m[ 2] *= k;
      m[ 3] *= k;
      m[ 4] *= k;
      m[ 5] *= k;
      m[ 6] *= k;
      m[ 7] *= k;
      m[ 8] *= k;
      m[ 9] *= k;
      m[10] *= k;
      m[11] *= k;
      m[12] *= k;
      m[13] *= k;
      m[14] *= k;
      m[15] *= k;
      return *this;
    }

    Mat44 &operator /= ( float k )
    {
      assert( k != 0.0f );

      k = 1.0f / k;
      m[ 0] *= k;
      m[ 1] *= k;
      m[ 2] *= k;
      m[ 3] *= k;
      m[ 4] *= k;
      m[ 5] *= k;
      m[ 6] *= k;
      m[ 7] *= k;
      m[ 8] *= k;
      m[ 9] *= k;
      m[10] *= k;
      m[11] *= k;
      m[12] *= k;
      m[13] *= k;
      m[14] *= k;
      m[15] *= k;
      return *this;
    }

    Mat44 operator + ( const Mat44 &a ) const
    {
      return Mat44( m[ 0] + a.m[ 0], m[ 1] + a.m[ 1], m[ 2] + a.m[ 2], m[ 3] + a.m[ 3],
                    m[ 4] + a.m[ 4], m[ 5] + a.m[ 5], m[ 6] + a.m[ 6], m[ 7] + a.m[ 7],
                    m[ 8] + a.m[ 8], m[ 9] + a.m[ 9], m[10] + a.m[10], m[11] + a.m[11],
                    m[12] + a.m[12], m[13] + a.m[13], m[14] + a.m[14], m[15] + a.m[15] );
    }

    Mat44 operator - ( const Mat44 &a ) const
    {
      return Mat44( m[ 0] - a.m[ 0], m[ 1] - a.m[ 1], m[ 2] - a.m[ 2], m[ 3] - a.m[ 3],
                    m[ 4] - a.m[ 4], m[ 5] - a.m[ 5], m[ 6] - a.m[ 6], m[ 7] - a.m[ 7],
                    m[ 8] - a.m[ 8], m[ 9] - a.m[ 9], m[10] - a.m[10], m[11] - a.m[11],
                    m[12] - a.m[12], m[13] - a.m[13], m[14] - a.m[14], m[15] - a.m[15] );
    }

    Mat44 operator * ( float k ) const
    {
      return Mat44( m[ 0] * k, m[ 1] * k, m[ 2] * k, m[ 3] * k,
                    m[ 4] * k, m[ 5] * k, m[ 6] * k, m[ 7] * k,
                    m[ 8] * k, m[ 9] * k, m[10] * k, m[11] * k,
                    m[12] * k, m[13] * k, m[14] * k, m[15] * k );
    }

    Mat44 operator / ( float k ) const
    {
      assert( k != 0.0f );

      k = 1.0f / k;
      return Mat44( m[ 0] * k, m[ 1] * k, m[ 2] * k, m[ 3] * k,
                    m[ 4] * k, m[ 5] * k, m[ 6] * k, m[ 7] * k,
                    m[ 8] * k, m[ 9] * k, m[10] * k, m[11] * k,
                    m[12] * k, m[13] * k, m[14] * k, m[15] * k );
    }

    Mat44 operator * ( const Mat44 &a ) const
    {
      return Mat44( m[ 0] * a.m[ 0] + m[ 4] * a.m[ 1] + m[ 8] * a.m[ 2] + m[12] * a.m[ 3],
                    m[ 1] * a.m[ 0] + m[ 5] * a.m[ 1] + m[ 9] * a.m[ 2] + m[13] * a.m[ 3],
                    m[ 2] * a.m[ 0] + m[ 6] * a.m[ 1] + m[10] * a.m[ 2] + m[14] * a.m[ 3],
                    m[ 3] * a.m[ 0] + m[ 7] * a.m[ 1] + m[11] * a.m[ 2] + m[15] * a.m[ 3],

                    m[ 0] * a.m[ 4] + m[ 4] * a.m[ 5] + m[ 8] * a.m[ 6] + m[12] * a.m[ 7],
                    m[ 1] * a.m[ 4] + m[ 5] * a.m[ 5] + m[ 9] * a.m[ 6] + m[13] * a.m[ 7],
                    m[ 2] * a.m[ 4] + m[ 6] * a.m[ 5] + m[10] * a.m[ 6] + m[14] * a.m[ 7],
                    m[ 3] * a.m[ 4] + m[ 7] * a.m[ 5] + m[11] * a.m[ 6] + m[15] * a.m[ 7],

                    m[ 0] * a.m[ 8] + m[ 4] * a.m[ 9] + m[ 8] * a.m[10] + m[12] * a.m[11],
                    m[ 1] * a.m[ 8] + m[ 5] * a.m[ 9] + m[ 9] * a.m[10] + m[13] * a.m[11],
                    m[ 2] * a.m[ 8] + m[ 6] * a.m[ 9] + m[10] * a.m[10] + m[14] * a.m[11],
                    m[ 3] * a.m[ 8] + m[ 7] * a.m[ 9] + m[11] * a.m[10] + m[15] * a.m[11],

                    m[ 0] * a.m[12] + m[ 4] * a.m[13] + m[ 8] * a.m[14] + m[12] * a.m[15],
                    m[ 1] * a.m[12] + m[ 5] * a.m[13] + m[ 9] * a.m[14] + m[13] * a.m[15],
                    m[ 2] * a.m[12] + m[ 6] * a.m[13] + m[10] * a.m[14] + m[14] * a.m[15],
                    m[ 3] * a.m[12] + m[ 7] * a.m[13] + m[11] * a.m[14] + m[15] * a.m[15] );
    }

    Vec3 operator * ( const Vec3 &v ) const
    {
      return Vec3( v.x * m[ 0] + v.y * m[ 4] + v.z * m[ 8] + m[12],
                   v.x * m[ 1] + v.y * m[ 5] + v.z * m[ 9] + m[13],
                   v.x * m[ 2] + v.y * m[ 6] + v.z * m[10] + m[14] );
    }

    Vec3 invMultiply( const Vec3 &v ) const
    {
      return Vec3( v.x * m[ 0] + v.y * m[ 1] + v.z * m[ 2] + m[ 3],
                   v.x * m[ 4] + v.y * m[ 5] + v.z * m[ 6] + m[ 7],
                   v.x * m[ 8] + v.y * m[ 9] + v.z * m[10] + m[11] );
    }

    friend Mat44 operator * ( float k, const Mat44 &a )
    {
      return Mat44( a.m[ 0] * k, a.m[ 1] * k, a.m[ 2] * k, a.m[ 3] * k,
                    a.m[ 4] * k, a.m[ 5] * k, a.m[ 6] * k, a.m[ 7] * k,
                    a.m[ 8] * k, a.m[ 9] * k, a.m[10] * k, a.m[11] * k,
                    a.m[12] * k, a.m[13] * k, a.m[14] * k, a.m[15] * k );
    }

    // transformation matrices
    static Mat44 transl( float dx, float dy, float dz )
    {
      return Mat44( 1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                      dx,   dy,   dz, 1.0f );
    }

    static Mat44 transl( const Vec3 &v )
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

  inline Mat33::Mat33( const Mat44 &m ) :
      xx( m.xx ), xy( m.xy ), xz( m.xz ),
      yx( m.yx ), yy( m.yy ), yz( m.yz ),
      zx( m.zx ), zy( m.zy ), zz( m.zz )
  {}

  // declared in Quat.h
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

  // declared in Quat.h
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
