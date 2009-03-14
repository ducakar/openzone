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

  struct Mat44
  {
    // WARNING: first index is column, second is line
    float _00;
    float _01;
    float _02;
    float _03;
    float _10;
    float _11;
    float _12;
    float _13;
    float _20;
    float _21;
    float _22;
    float _23;
    float _30;
    float _31;
    float _32;
    float _33;

    Mat44()
    {}

    Mat44( float m00, float m01, float m02, float m03,
           float m10, float m11, float m12, float m13,
           float m20, float m21, float m22, float m23,
           float m30, float m31, float m32, float m33 ) :
        _00( m00 ), _01( m01 ), _02( m02 ), _03( m03 ),
        _10( m10 ), _11( m11 ), _12( m12 ), _13( m13 ),
        _20( m20 ), _21( m21 ), _22( m22 ), _23( m23 ),
        _30( m30 ), _31( m31 ), _32( m32 ), _33( m33 )
    {}

    explicit Mat44( const float *v )
    {
      *this = *(Mat44*) v;
    }

    explicit Mat44( const Mat33 &m ) :
        _00( m._00 ), _01( m._01 ), _02( m._02 ), _03( 0.0f ),
        _10( m._10 ), _11( m._11 ), _12( m._12 ), _13( 0.0f ),
        _20( m._20 ), _21( m._21 ), _22( m._22 ), _23( 0.0f ),
        _30(  0.0f ), _31(  0.0f ), _32(  0.0f ), _33( 1.0f )
    {}

    Mat44( const Vec3 &a, const Vec3 &b, const Vec3 &c, const Vec3 &d ) :
        _00( a.x ), _01( a.y ), _02( a.z ), _03( 0.0f ),
        _10( b.x ), _11( b.y ), _12( b.z ), _13( 0.0f ),
        _20( c.x ), _21( c.y ), _22( c.z ), _23( 0.0f ),
        _30( d.x ), _31( d.y ), _32( d.z ), _33( 1.0f )
    {}

    Mat44( const Quat &a, const Quat &b, const Quat &c, const Quat &d ) :
        _00( a.x ), _01( a.y ), _02( a.z ), _03( a.w ),
        _10( b.x ), _11( b.y ), _12( b.z ), _13( b.w ),
        _20( c.x ), _21( c.y ), _22( c.z ), _23( c.w ),
        _30( d.x ), _31( d.y ), _32( d.z ), _33( d.w )
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

    operator float* () const
    {
      return (float*) this;
    }

    operator const float* () const
    {
      return (float*) this;
    }

    float &operator [] ( int i )
    {
      return ( (float*) this )[i];
    }

    const float &operator [] ( int i ) const
    {
      return ( (const float*) this )[i];
    }

    bool operator == ( const Mat44 &a ) const
    {
      return
          _00 == a._00 &&
          _01 == a._01 &&
          _02 == a._02 &&
          _03 == a._03 &&
          _10 == a._10 &&
          _11 == a._11 &&
          _12 == a._12 &&
          _13 == a._13 &&
          _20 == a._20 &&
          _21 == a._21 &&
          _22 == a._22 &&
          _23 == a._23 &&
          _30 == a._30 &&
          _31 == a._31 &&
          _32 == a._32 &&
          _33 == a._33;
    }

    bool operator != ( const Mat44 &a ) const
    {
      return
          _00 != a._00 ||
          _01 != a._01 ||
          _02 != a._02 ||
          _03 != a._03 ||
          _10 != a._10 ||
          _11 != a._11 ||
          _12 != a._12 ||
          _13 != a._13 ||
          _20 != a._20 ||
          _21 != a._21 ||
          _22 != a._22 ||
          _23 != a._23 ||
          _30 != a._30 ||
          _31 != a._31 ||
          _32 != a._32 ||
          _33 != a._33;
    }

    Mat44 operator + () const
    {
      return *this;
    }

    Mat44 operator - () const
    {
      return Mat44( -_00, -_01, -_02, -_03,
                    -_10, -_11, -_12, -_13,
                    -_20, -_21, -_22, -_23,
                    -_30, -_31, -_32, -_33 );
    }

    float det() const
    {
      float klop = _22 * _33 - _32 * _23;
      float jlnp = _12 * _33 - _32 * _13;
      float jkno = _12 * _23 - _22 * _13;
      float ilmp = _02 * _33 - _32 * _03;
      float ikmo = _02 * _23 - _22 * _03;
      float ijmn = _02 * _13 - _12 * _03;
      return
          _00 * _11 * klop -
          _00 * _21 * jlnp +
          _00 * _31 * jkno -
          _10 * _01 * klop +
          _10 * _21 * ilmp -
          _10 * _31 * ikmo +
          _20 * _01 * jlnp -
          _20 * _11 * ilmp +
          _20 * _31 * ijmn -
          _30 * _01 * jkno +
          _30 * _11 * ikmo -
          _30 * _21 * ijmn;
    }

    Mat44 operator ~ () const
    {
      return Mat44( _00, _10, _20, _30,
                    _01, _11, _21, _31,
                    _02, _12, _22, _32,
                    _03, _13, _23, _33 );
    }

    Mat44 &trans()
    {
      swap( _01, _10 );
      swap( _02, _20 );
      swap( _03, _30 );
      swap( _12, _21 );
      swap( _13, _31 );
      swap( _23, _32 );
      return *this;
    }

    bool isZero() const
    {
      return
          0.f == _00 &&
          _00 == _01 &&
          _01 == _02 &&
          _02 == _03 &&
          _03 == _10 &&
          _10 == _11 &&
          _11 == _12 &&
          _12 == _13 &&
          _13 == _20 &&
          _20 == _21 &&
          _21 == _22 &&
          _22 == _23 &&
          _23 == _30 &&
          _30 == _31 &&
          _31 == _32 &&
          _32 == _33;
    }

    Mat44 &setZero()
    {
      _33 = _32 = _31 = _30 = _23 = _22 = _21 = _20 = _13 = _12 = _11 = _10 =
          _03 = _02 = _01 = _00 = 0.0f;
      return *this;
    }

    bool isId() const
    {
      return
          0.f == _01 && _01 == _02 && _02 == _03 && _03 == _10 &&
          _10 == _12 && _12 == _13 && _13 == _20 && _20 == _21 &&
          _21 == _23 && _23 == _30 && _30 == _31 && _31 == _32 &&
          1.f == _00 && _00 == _11 && _11 == _22 && _22 == _33;
    }

    Mat44 &setId()
    {
      _32 = _31 = _30 = _23 = _21 = _20 = _13 = _12 = _10 = _03 = _02 = _01 = 0.0f;
      _33 = _22 = _11 = _00 = 1.0f;
      return *this;
    }

    Mat44 &operator += ( const Mat44 &a )
    {
      _00 += a._00;
      _01 += a._01;
      _02 += a._02;
      _03 += a._03;
      _10 += a._10;
      _11 += a._11;
      _12 += a._12;
      _13 += a._13;
      _20 += a._20;
      _21 += a._21;
      _22 += a._22;
      _23 += a._23;
      _30 += a._30;
      _31 += a._31;
      _32 += a._32;
      _33 += a._33;
      return *this;
    }

    Mat44 &operator -= ( const Mat44 &a )
    {
      _00 -= a._00;
      _01 -= a._01;
      _02 -= a._02;
      _03 -= a._03;
      _10 -= a._10;
      _11 -= a._11;
      _12 -= a._12;
      _13 -= a._13;
      _20 -= a._20;
      _21 -= a._21;
      _22 -= a._22;
      _23 -= a._23;
      _30 -= a._30;
      _31 -= a._31;
      _32 -= a._32;
      _33 -= a._33;
      return *this;
    }

    Mat44 &operator *= ( float k )
    {
      _00 *= k;
      _01 *= k;
      _02 *= k;
      _03 *= k;
      _10 *= k;
      _11 *= k;
      _12 *= k;
      _13 *= k;
      _20 *= k;
      _21 *= k;
      _22 *= k;
      _23 *= k;
      _30 *= k;
      _31 *= k;
      _32 *= k;
      _33 *= k;
      return *this;
    }

    Mat44 &operator /= ( float k )
    {
      k = 1.0f / k;
      _00 *= k;
      _01 *= k;
      _02 *= k;
      _03 *= k;
      _10 *= k;
      _11 *= k;
      _12 *= k;
      _13 *= k;
      _20 *= k;
      _21 *= k;
      _22 *= k;
      _23 *= k;
      _30 *= k;
      _31 *= k;
      _32 *= k;
      _33 *= k;
      return *this;
    }

    Mat44 operator + ( const Mat44 &a ) const
    {
      return Mat44( _00 + a._00, _01 + a._01, _02 + a._02, _03 + a._03,
                    _10 + a._10, _11 + a._11, _12 + a._12, _13 + a._13,
                    _20 + a._20, _21 + a._21, _22 + a._22, _23 + a._23,
                    _30 + a._30, _31 + a._31, _32 + a._32, _33 + a._33 );
    }

    Mat44 operator - ( const Mat44 &a ) const
    {
      return Mat44( _00 - a._00, _01 - a._01, _02 - a._02, _03 - a._03,
                    _10 - a._10, _11 - a._11, _12 - a._12, _13 - a._13,
                    _20 - a._20, _21 - a._21, _22 - a._22, _23 - a._23,
                    _30 - a._30, _31 - a._31, _32 - a._32, _33 - a._33 );
    }

    Mat44 operator * ( float k ) const
    {
      return Mat44( _00 * k, _01 * k, _02 * k, _03 * k,
                    _10 * k, _11 * k, _12 * k, _13 * k,
                    _20 * k, _21 * k, _22 * k, _23 * k,
                    _30 * k, _31 * k, _32 * k, _33 * k );
    }

    Mat44 operator / ( float k ) const
    {
      k = 1.0f / k;
      return Mat44( _00 * k, _01 * k, _02 * k, _03 * k,
                    _10 * k, _11 * k, _12 * k, _13 * k,
                    _20 * k, _21 * k, _22 * k, _23 * k,
                    _30 * k, _31 * k, _32 * k, _33 * k );
    }

    Mat44 operator * ( const Mat44 &a ) const
    {
      return Mat44( _00 * a._00 + _10 * a._01 + _20 * a._02 + _30 * a._03,
                    _01 * a._00 + _11 * a._01 + _21 * a._02 + _31 * a._03,
                    _02 * a._00 + _12 * a._01 + _22 * a._02 + _32 * a._03,
                    _03 * a._00 + _13 * a._01 + _23 * a._02 + _33 * a._03,

                    _00 * a._10 + _10 * a._11 + _20 * a._12 + _30 * a._13,
                    _01 * a._10 + _11 * a._11 + _21 * a._12 + _31 * a._13,
                    _02 * a._10 + _12 * a._11 + _22 * a._12 + _32 * a._13,
                    _03 * a._10 + _13 * a._11 + _23 * a._12 + _33 * a._13,

                    _00 * a._20 + _10 * a._21 + _20 * a._22 + _30 * a._23,
                    _01 * a._20 + _11 * a._21 + _21 * a._22 + _31 * a._23,
                    _02 * a._20 + _12 * a._21 + _22 * a._22 + _32 * a._23,
                    _03 * a._20 + _13 * a._21 + _23 * a._22 + _33 * a._23,

                    _00 * a._30 + _10 * a._31 + _20 * a._32 + _30 * a._33,
                    _01 * a._30 + _11 * a._31 + _21 * a._32 + _31 * a._33,
                    _02 * a._30 + _12 * a._31 + _22 * a._32 + _32 * a._33,
                    _03 * a._30 + _13 * a._31 + _23 * a._32 + _33 * a._33 );
    }

    Vec3 operator * ( const Vec3 &v ) const
    {
      return Vec3( v.x * _00 + v.y * _10 + v.z * _20 + _30,
                   v.x * _01 + v.y * _11 + v.z * _21 + _31,
                   v.x * _02 + v.y * _12 + v.z * _22 + _32 );
    }

    Vec3 invMultiply( const Vec3 &v ) const
    {
      return Vec3( v.x * _00 + v.y * _01 + v.z * _02 + _03,
                   v.x * _10 + v.y * _11 + v.z * _12 + _13,
                   v.x * _20 + v.y * _21 + v.z * _22 + _23 );
    }

    friend Mat44 operator * ( float k, const Mat44 &a )
    {
      return Mat44( a._00 * k, a._01 * k, a._02 * k, a._03 * k,
                    a._10 * k, a._11 * k, a._12 * k, a._13 * k,
                    a._20 * k, a._21 * k, a._22 * k, a._23 * k,
                    a._30 * k, a._31 * k, a._32 * k, a._33 * k );
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

  // declared in Quat.h
  inline Mat33::Mat33( const Mat44 &m ) :
      _00( m._00 ), _01( m._01 ), _02( m._02 ),
      _10( m._10 ), _11( m._11 ), _12( m._12 ),
      _20( m._20 ), _21( m._21 ), _22( m._22 )
  {}

}
