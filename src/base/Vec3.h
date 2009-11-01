/*
 *  Vec3.h
 *
 *  3D vector library
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  struct Quat;

  struct Vec3
  {
    float x;
    float y;
    float z;

    explicit Vec3()
    {}

    explicit Vec3( float x_, float y_, float z_ ) : x( x_ ), y( y_ ), z( z_ )
    {}

    explicit Vec3( float *v )
    {
      *this = *reinterpret_cast<Vec3*>( v );
    }

    // implemented in Quat.h
    explicit Vec3( const Quat &q );

    static Vec3 zero()
    {
      return Vec3( 0.0f, 0.0f, 0.0f );
    }

    operator float* ()
    {
      return reinterpret_cast<float*>( this );
    }

    operator const float* () const
    {
      return reinterpret_cast<const float*>( this );
    }

    float &operator [] ( int i )
    {
      return reinterpret_cast<float*>( this )[i];
    }

    const float &operator [] ( int i ) const
    {
      return reinterpret_cast<const float*>( this )[i];
    }

    bool isEqual( const Vec3 &a, float epsilon ) const
    {
      return
          Math::abs( x - a.x ) <= epsilon &&
          Math::abs( y - a.y ) <= epsilon &&
          Math::abs( z - a.z ) <= epsilon;
    }

    Vec3 operator + () const
    {
      return *this;
    }

    Vec3 operator - () const
    {
      return Vec3( -x, -y, -z );
    }

    // length
    float operator ! () const
    {
      return Math::sqrt( x*x + y*y + z*z );
    }

    // square length
    float sqL() const
    {
      return x*x + y*y + z*z;
    }

    bool isZero() const
    {
      return x == 0.0f && y == 0.0f && z == 0.0f;
    }

    Vec3 &setZero()
    {
      x = 0.0f;
      y = 0.0f;
      z = 0.0f;
      return *this;
    }

    bool isUnit() const
    {
      return x*x + y*y + z*z == 1.0f;
    }

    Vec3 operator ~ () const
    {
      assert( x*x + y*y + z*z > 0.0f );

      float r = 1.0f / Math::sqrt( x*x + y*y + z*z );
      return Vec3( x * r, y * r, z * r );
    }

    Vec3 &norm()
    {
      assert( x*x + y*y + z*z > 0.0f );

      float r = 1.0f / Math::sqrt( x*x + y*y + z*z );
      x *= r;
      y *= r;
      z *= r;
      return *this;
    }

    bool isColinear( const Vec3 &v ) const
    {
      float p1 = v.x * y * z;
      float p2 = v.y * x * z;
      float p3 = v.z * x * y;

      return p1 == p2 && p1 == p3;
    }

    bool isColinear( const Vec3 &v, float epsilon ) const
    {
      float p1 = v.x * y * z;
      float p2 = v.y * x * z;
      float p3 = v.z * x * y;

      return Math::abs( p1 - p2 ) <= epsilon && Math::abs( p1 - p3 ) <= epsilon;
    }

    Vec3 &operator += ( const Vec3 &a )
    {
      x += a.x;
      y += a.y;
      z += a.z;
      return *this;
    }

    Vec3 &operator -= ( const Vec3 &a )
    {
      x -= a.x;
      y -= a.y;
      z -= a.z;
      return *this;
    }

    Vec3 &operator *= ( float k )
    {
      x *= k;
      y *= k;
      z *= k;
      return *this;
    }

    Vec3 &operator /= ( float k )
    {
      assert( k != 0.0f );

      k = 1.0f / k;
      x *= k;
      y *= k;
      z *= k;
      return *this;
    }

    Vec3 operator + ( const Vec3 &a ) const
    {
      return Vec3( x + a.x, y + a.y, z + a.z );
    }

    Vec3 operator - ( const Vec3 &a ) const
    {
      return Vec3( x - a.x, y - a.y, z - a.z );
    }

    Vec3 operator * ( float k ) const
    {
      return Vec3( x * k, y * k, z * k );
    }

    Vec3 operator / ( float k ) const
    {
      assert( k != 0.0f );

      k = 1.0f / k;
      return Vec3( x * k, y * k, z * k );
    }

    // dot product
    float operator * ( const Vec3 &a ) const
    {
      return x*a.x + y*a.y + z*a.z;
    }

    // cross product
    Vec3 operator ^ ( const Vec3 &a ) const
    {
      return Vec3( y*a.z - z*a.y, z*a.x - x*a.z, x*a.y - y*a.x );
    }

    // vector which lies in plane defined by given vectors and is perpendicular to first one
    Vec3 operator % ( const Vec3 &a ) const
    {
      // this is actually -( u x v ) x u, where u is *this vector
      // This equals to:
      // ( u . u )v - ( u . v )u = |u|^2 * ( v - ( u . v )/( |u|^2 ) * u )
      // ( the length doesn't matter )

      // |u|^2, assume it's not 0
      float k = x*x + y*y + z*z;

      assert( k != 0.0f );

      k = ( x * a.x + y * a.y + z * a.z ) / k;
      return a - k * ( *this );
    }

    friend Vec3 operator * ( float k, const Vec3 &a )
    {
      return Vec3( a.x * k, a.y * k, a.z * k );
    }

    // mixed product
    static float mix( const Vec3 &a, const Vec3 &b, const Vec3 &c )
    {
      // 3x3 determinant
      return
          a.x * ( b.y * c.z - b.z * c.y ) -
          a.y * ( b.x * c.z - b.z * c.x ) +
          a.z * ( b.x * c.y - b.y * c.x );
    }

  };

}
