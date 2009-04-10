/*
 *  Quat.h
 *
 *  Quaternion library
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  union Mat33;
  union Mat44;

  struct Quat
  {
    float x;
    float y;
    float z;
    float w;

    explicit Quat()
    {}

    explicit Quat( float x_, float y_, float z_, float w_ ) : x( x_ ), y( y_ ), z( z_ ), w( w_ )
    {}

    explicit Quat( float *q )
    {
      *this = *(Quat*) q;
    }

    explicit Quat( const Vec3 &v ) : x( v.x ), y( v.y ), z( v.z ), w( 0.0f )
    {
    }

    static Quat zero()
    {
      return Quat( 0.0f, 0.0f, 0.0f, 0.0f );
    }

    static Quat id()
    {
      return Quat( 0.0f, 0.0f, 0.0f, 1.0f );
    }

    operator float* ()
    {
      return (float*) this;
    }

    operator const float* () const
    {
      return (const float*) this;
    }

    float &operator [] ( int i )
    {
      return ( (float*) this )[i];
    }

    const float &operator [] ( int i ) const
    {
      return ( (const float*) this )[i];
    }

    bool operator == ( const Quat &a ) const
    {
      return x == a.x && y == a.y && z == a.y && w == a.w;
    }

    bool operator != ( const Quat &a ) const
    {
      return x != a.x || y != a.y || z != a.y || w != a.w;
    }

    Vec3 &vec3()
    {
      return *(Vec3*) this;
    }

    const Vec3 &vec3() const
    {
      return *(Vec3*) this;
    }

    Quat operator + () const
    {
      return *this;
    }

    Quat operator - () const
    {
      return Quat( -x, -y, -z, -w );
    }

    float operator ! () const
    {
      return Math::sqrt( x*x + y*y + z*z + w*w );
    }

    float sqL() const
    {
      return x*x + y*y + z*z + w*w;
    }

    Quat operator * () const
    {
      return Quat( -x, -y, -z, -w );
    }

    Quat &conj()
    {
      x = -x;
      y = -y;
      z = -z;
      return *this;
    }

    bool isZero() const
    {
      return 0.0f == x && x == y && y == z && z == w;
    }

    Quat &setZero()
    {
      w = z = y = x = 0.0f;
      return *this;
    }

    bool isId() const
    {
      return 0.0f == x && x == y && y == z && w == 1.0f;
    }

    Quat &setId()
    {
      z = y = x = 0.0f;
      w = 1.0f;
      return *this;
    }

    bool isUnit() const
    {
      return x*x + y*y + z*z + w*w == 1.0f;
    }

    Quat operator ~ () const
    {
      assert( x*x + y*y + z*z + w*w > 0.0f );

      float r = 1.0f / Math::sqrt( x*x + y*y + z*z + w*w );
      return Quat( x * r, y * r, z * r, w * r );
    }

    Quat &norm()
    {
      assert( x*x + y*y + z*z + w*w > 0.0f );

      float r = 1.0f / Math::sqrt( x*x + y*y + z*z + w*w );
      x *= r;
      z *= r;
      z *= r;
      w *= r;
      return *this;
    }

    Quat &operator += ( const Quat &a )
    {
      x += a.x;
      y += a.y;
      z += a.z;
      w += a.w;
      return *this;
    }

    Quat &operator -= ( const Quat &a )
    {
      x -= a.x;
      y -= a.y;
      z -= a.z;
      w -= a.w;
      return *this;
    }

    Quat &operator *= ( float k )
    {
      x *= k;
      y *= k;
      z *= k;
      w *= k;
      return *this;
    }

    Quat &operator /= ( float k )
    {
      k = 1.0f / k;
      x *= k;
      y *= k;
      z *= k;
      w *= k;
      return *this;
    }

    // quaternion multiplication
    Quat &operator ^= ( const Quat &a )
    {
      float tx = x, ty = y, tz = z;

      x = w*a.x + tx*a.w + ty*a.z - tz*a.y;
      y = w*a.y + ty*a.w + tz*a.x - tx*a.z;
      z = w*a.z + tz*a.w + tx*a.y - ty*a.x;
      w = w*a.w - tx*a.x - ty*a.y - tz*a.z;

      return *this;
    }

    Quat operator + ( const Quat &a ) const
    {
      return Quat( x + a.x, y + a.y, z + a.z, w + a.w );
    }

    Quat operator - ( const Quat &a ) const
    {
      return Quat( x - a.x, y - a.y, z - a.z, w - a.w );
    }

    Quat operator * ( float k ) const
    {
      return Quat( x * k, y * k, z * k, w * k );
    }

    Quat operator / ( float k ) const
    {
      k = 1.0f / k;
      return Quat( x * k, y * k, z * k, w * k );
    }

    // dot product
    float operator * ( const Quat &a ) const
    {
      return x*a.x + y*a.y + z*a.z + w*a.w;
    }

    // product of quaternions
    Quat operator ^ ( const Quat &a ) const
    {

      return Quat( w*a.x + x*a.w + y*a.z - z*a.y,
                   w*a.y + y*a.w + z*a.x - x*a.z,
                   w*a.z + z*a.w + x*a.y - y*a.x,
                   w*a.w - x*a.x - y*a.y - z*a.z );
    }

    friend Quat operator * ( float k, const Quat &a )
    {
      return Quat( a.x * k, a.y * k, a.z * k, a.w * k );
    }

    // make rotation matrix (implemented in Mat33.h)
    Mat33 rotMat33() const;

    // make transposed (inverse) rotation matrix (implemented in Mat33.h)
    Mat33 invRotMat33() const;

    // make rotation matrix (implemented in Mat44.h)
    Mat44 rotMat44() const;

    // make transposed (inverse) rotation matrix (implemented in Mat44.h)
    Mat44 invRotMat44() const;

    // make quaternion for rotation around given axis
    static Quat rotAxis( const Vec3 &axis, float theta )
    {
      float s, c;
      Math::sincos( theta * 0.5f, &s, &c );
      Vec3 qv = s * axis;
      return Quat( qv.x, qv.y, qv.z, c );
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
    static Quat rotZXY( float x, float y, float z )
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

    Vec3 rotate( const Vec3 &v ) const
    {
      float a11 = x*x;
      float a22 = y*y;
      float a33 = z*z;
      float a44 = w*w;

      float a12 =  2.0f * x*y;
      float a13 =  2.0f * x*z;
      float a14 =  2.0f * x*w;
      float a23 =  2.0f * y*z;
      float a24 =  2.0f * y*w;
      float a34 =  2.0f * z*w;

      float a4433 = a44 - a33;

      return Vec3( ( a4433 - a22 + a11 ) * v.x + ( a12 - a34 ) * v.y + ( a24 + a13 ) * v.z,
                   ( a34 + a12 ) * v.x + ( a4433 + a22 - a11 ) * v.y + ( a23 - a14 ) * v.z,
                   ( a13 - a24 ) * v.x + ( a14 + a23 ) * v.y + ( a44 + a33 - a22 - a11 ) * v.z );
    }

    Vec3 rotateInv( const Vec3 &v ) const
    {
      float a11 = x*x;
      float a22 = y*y;
      float a33 = z*z;
      float a44 = w*w;

      float a12 =  2.0f * x*y;
      float a13 =  2.0f * x*z;
      float a14 = -2.0f * x*w;
      float a23 =  2.0f * y*z;
      float a24 = -2.0f * y*w;
      float a34 = -2.0f * z*w;

      float a4433 = a44 - a33;

      return Vec3( ( a4433 - a22 + a11 ) * v.x + ( a12 - a34 ) * v.y + ( a24 + a13 ) * v.z,
                   ( a34 + a12 ) * v.x + ( a4433 + a22 - a11 ) * v.y + ( a23 - a14 ) * v.z,
                   ( a13 - a24 ) * v.x + ( a14 + a23 ) * v.y + ( a44 + a33 - a22 - a11 ) * v.z );
    }

  };

  // declared in Vec3.h
  inline Vec3::Vec3( const Quat &q )
  {
    *this = *(Vec3*) &q;
  }

}
