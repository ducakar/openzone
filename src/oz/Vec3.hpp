/*
 *  Vec3.hpp
 *
 *  3D vector
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "Math.hpp"

namespace oz
{

  class Vec3
  {
    public:

      static const Vec3 ZERO;

      float x;
      float y;
      float z;

      explicit Vec3()
      {}

      explicit Vec3( float x_, float y_, float z_ ) : x( x_ ), y( y_ ), z( z_ )
      {}

      explicit Vec3( const float* v ) : x( v[0] ), y( v[1] ), z( v[2] )
      {}

      bool operator == ( const Vec3& v ) const
      {
        return x == v.x && y == v.y && z == v.z;
      }

      bool operator != ( const Vec3& v ) const
      {
        return x != v.x || y != v.y || z != v.z;
      }

      operator const float* () const
      {
        return &x;
      }

      operator float* ()
      {
        return &x;
      }

      const float& operator [] ( int i ) const
      {
        assert( 0 <= i && i < 3 );

        return ( &x )[i];
      }

      float& operator [] ( int i )
      {
        assert( 0 <= i && i < 3 );

        return ( &x )[i];
      }

      bool equals( const Vec3& a, float epsilon ) const
      {
        return
            Math::abs( x - a.x ) <= epsilon &&
            Math::abs( y - a.y ) <= epsilon &&
            Math::abs( z - a.z ) <= epsilon;
      }

      Vec3 abs() const
      {
        return Vec3( Math::abs( x ), Math::abs( y ), Math::abs( z ) );
      }

      float operator ! () const
      {
        return Math::sqrt( x*x + y*y + z*z );
      }

      float fastL() const
      {
         return Math::fastSqrt( x*x + y*y + z*z );
      }

      float sqL() const
      {
        return x*x + y*y + z*z;
      }

      bool isUnit() const
      {
        return x*x + y*y + z*z == 1.0f;
      }

      Vec3 operator ~ () const
      {
        assert( x*x + y*y + z*z > 0.0f );

        float k = 1.0f / Math::sqrt( x*x + y*y + z*z );
        return Vec3( x * k, y * k, z * k );
      }

      Vec3 fastUnit() const
      {
        assert( x*x + y*y + z*z > 0.0f );

        float k = Math::fastInvSqrt( x*x + y*y + z*z );
        return Vec3( x * k, y * k, z * k );
      }

      bool isColinear( const Vec3& v, float epsilon ) const
      {
        float p1 = v.x * y * z;
        float p2 = v.y * x * z;
        float p3 = v.z * x * y;

        return Math::abs( p1 - p2 ) <= epsilon && Math::abs( p1 - p3 ) <= epsilon;
      }

      Vec3 operator + () const
      {
        return *this;
      }

      Vec3 operator - () const
      {
        return Vec3( -x, -y, -z );
      }

      Vec3 operator + ( const Vec3& a ) const
      {
        return Vec3( x + a.x, y + a.y, z + a.z );
      }

      Vec3 operator - ( const Vec3& a ) const
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

      Vec3& operator += ( const Vec3& a )
      {
        x += a.x;
        y += a.y;
        z += a.z;
        return *this;
      }

      Vec3& operator -= ( const Vec3& a )
      {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        return *this;
      }

      Vec3& operator *= ( float k )
      {
        x *= k;
        y *= k;
        z *= k;
        return *this;
      }

      Vec3& operator /= ( float k )
      {
        assert( k != 0.0f );

        k = 1.0f / k;
        x *= k;
        y *= k;
        z *= k;
        return *this;
      }

      // dot product
      float operator * ( const Vec3& a ) const
      {
        return x*a.x + y*a.y + z*a.z;
      }

      // cross product
      Vec3 operator ^ ( const Vec3& a ) const
      {
        return Vec3( y*a.z - z*a.y, z*a.x - x*a.z, x*a.y - y*a.x );
      }

      // vector which lies in plane defined by given vectors and is perpendicular to first one
      Vec3 operator % ( const Vec3& a ) const
      {
        // this is actually -( u x v ) x u, where u is* this vector
        // This equals to:
        // ( u . u )v - ( u . v )u = |u|^2 * ( v - ( u . v )/( |u|^2 ) * u )
        // ( the length doesn't matter )

        // |u|^2, assume it's not 0
        float k = x*x + y*y + z*z;

        assert( k != 0.0f );

        k = ( x * a.x + y * a.y + z * a.z ) / k;
        return a - Vec3( x * k, y * k, z * k );
      }

      friend Vec3 operator * ( float k, const Vec3& a )
      {
        return Vec3( a.x * k, a.y * k, a.z * k );
      }

      // mixed product
      static float mix( const Vec3& a, const Vec3& b, const Vec3& c )
      {
        // 3x3 determinant
        return
            a.x * ( b.y * c.z - b.z * c.y ) -
            a.y * ( b.x * c.z - b.z * c.x ) +
            a.z * ( b.x * c.y - b.y * c.x );
      }

  };

}
