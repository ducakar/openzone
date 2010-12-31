/*
 *  Vec4.hpp
 *
 *  3D vector inside 4D space, optimised for SIMD
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "Math.hpp"

namespace oz
{

  class __attribute__(( aligned( 16 ) )) Vec4
  {
    public:

      static const Vec4 ZERO;

      union
      {
        float4 f4;

        struct
        {
          float x;
          float y;
          float z;
          float w;
        };
      };

      explicit Vec4()
      {}

      explicit Vec4( float4 f4_ ) : f4( f4_ )
      {}

      explicit Vec4( float x, float y, float z, float w ) : f4( (float4) { x, y, z, w } )
      {}

      explicit Vec4( float x, float y, float z ) : f4( (float4) { x, y, z, 0.0f } )
      {}

      explicit Vec4( const float* v ) : f4( (float4) { v[0], v[1], v[2], 0.0f } )
      {}

      bool operator == ( const Vec4& v ) const
      {
        return x == v.x && y == v.y && z == v.z;
      }

      bool operator != ( const Vec4& v ) const
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

      bool equals( const Vec4& v, float epsilon ) const
      {
        return
            Math::abs( x - v.x ) <= epsilon &&
            Math::abs( y - v.y ) <= epsilon &&
            Math::abs( z - v.z ) <= epsilon;
      }

      Vec4 abs() const
      {
        return Vec4( Math::abs( x ), Math::abs( y ), Math::abs( z ) );
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

      Vec4 operator ~ () const
      {
        assert( x*x + y*y + z*z > 0.0f );

        float k = 1.0f / Math::sqrt( x*x + y*y + z*z );
        return Vec4( f4 * (float4) { k, k, k, k } );
      }

      Vec4 fastUnit() const
      {
        assert( x*x + y*y + z*z > 0.0f );

        float k = Math::fastInvSqrt( x*x + y*y + z*z );
        return Vec4( f4 * (float4) { k, k, k, k } );
      }

      bool isColinear( const Vec4& v, float epsilon ) const
      {
        float p1 = v.x * y * z;
        float p2 = v.y * x * z;
        float p3 = v.z * x * y;

        return Math::abs( p1 - p2 ) <= epsilon && Math::abs( p1 - p3 ) <= epsilon;
      }

      Vec4 operator + () const
      {
        return *this;
      }

      Vec4 operator - () const
      {
        return Vec4( -f4 );
      }

      Vec4 operator + ( const Vec4& v ) const
      {
        return Vec4( f4 + v.f4 );
      }

      Vec4 operator - ( const Vec4& v ) const
      {
        return Vec4( f4 - v.f4 );
      }

      Vec4 operator * ( float k ) const
      {
        return Vec4( f4 * (float4) { k, k, k, k } );
      }

      friend Vec4 operator * ( float k, const Vec4& v )
      {
        return Vec4( (float4) { k, k, k, k } * v.f4 );
      }

      Vec4 operator / ( float k ) const
      {
        assert( k != 0.0f );

        k = 1.0f / k;
        return Vec4( f4 * (float4) { k, k, k, k } );
      }

      Vec4& operator += ( const Vec4& v )
      {
        f4 += v.f4;
        return *this;
      }

      Vec4& operator -= ( const Vec4& v )
      {
        f4 -= v.f4;
        return *this;
      }

      Vec4& operator *= ( float k )
      {
        f4 *= (float4) { k, k, k, k };
        return *this;
      }

      Vec4& operator /= ( float k )
      {
        assert( k != 0.0f );

        k = 1.0f / k;
        f4 *= (float4) { k, k, k, k };
        return *this;
      }

      // dot product
      float operator * ( const Vec4& v ) const
      {
        return x*v.x + y*v.y + z*v.z;
      }

      // cross product
      Vec4 operator ^ ( const Vec4& v ) const
      {
        return Vec4( y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x );
      }

      // vector which lies in plane defined by given vectors and is perpendicular to first one
      Vec4 operator % ( const Vec4& v ) const
      {
        // this is actually -( u x v ) x u, where u is* this vector
        // This equals to:
        // ( u . u )v - ( u . v )u = |u|^2 * ( v - ( u . v )/( |u|^2 ) * u )
        // ( the length doesn't matter )

        // |u|^2, assume it's not 0
        float k = x*x + y*y + z*z;
        assert( k != 0.0f );

        k = ( x*v.x + y*v.y + z*v.z ) / k;
        return Vec4( v.f4 - f4 * (float4) { k, k, k, k } );
      }

      // mixed product
      static float mix( const Vec4& a, const Vec4& b, const Vec4& c )
      {
        // 3x3 determinant
        return
            a.x * ( b.y * c.z - b.z * c.y ) -
            a.y * ( b.x * c.z - b.z * c.x ) +
            a.z * ( b.x * c.y - b.y * c.x );
      }

  };

}
