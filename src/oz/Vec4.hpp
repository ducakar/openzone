/*
 *  Vec4.hpp
 *
 *  3D vector with 4 components (for SIMD optimization)
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

namespace oz
{

  class Vec4 : public Vec3
  {
    public:

      float w;

      explicit Vec4()
      {}

      explicit Vec4( float x_, float y_, float z_, float w_ ) : Vec3( x_, y_, z_ ), w( w_ )
      {}

      explicit Vec4( float x_, float y_, float z_ ) : Vec3( x_, y_, z_ ), w( 0.0f )
      {}

      explicit Vec4( const float* v ) : Vec3( v[0], v[1], v[2] ), w( 0.0f )
      {}

      explicit Vec4( const Vec3& v ) : Vec3( v.x, v.y, v.z ), w( 0.0f )
      {}

      Vec4& operator = ( const Vec3& v )
      {
        x = v.x;
        y = v.y;
        z = v.z;
        w = 0.0f;
        return *this;
      }

      static Vec4 zero()
      {
        return Vec4( 0.0f, 0.0f, 0.0f, 0.0f );
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
        assert( 0 <= i && i < 4 );

        return ( &x )[i];
      }

      float& operator [] ( int i )
      {
        assert( 0 <= i && i < 4 );

        return ( &x )[i];
      }

      bool equals( const Vec4& a, float epsilon ) const
      {
        return
            Math::abs( x - a.x ) <= epsilon &&
            Math::abs( y - a.y ) <= epsilon &&
            Math::abs( z - a.z ) <= epsilon;
      }

      bool isZero() const
      {
        return x == 0.0f && y == 0.0f && z == 0.0f;
      }

      const Vec4& setZero()
      {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
        w = 0.0f;
        return *this;
      }

      float operator ! () const
      {
        return Math::sqrt( x*x + y*y + z*z );
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

        float r = 1.0f / Math::sqrt( x*x + y*y + z*z );
        return Vec4( x * r, y * r, z * r, w * r );
      }

      Vec4 fastUnit() const
      {
        assert( x*x + y*y + z*z > 0.0f );

        float k = Math::fastInvSqrt( x*x + y*y + z*z );
        return Vec4( x * k, y * k, z * k, w * k );
      }

      Vec4& norm()
      {
        assert( x*x + y*y + z*z > 0.0f );

        float k = 1.0f / Math::sqrt( x*x + y*y + z*z );
        x *= k;
        y *= k;
        z *= k;
        w *= k;
        return *this;
      }

      Vec4& fastNorm()
      {
        assert( x*x + y*y + z*z > 0.0f );

        float k = Math::fastInvSqrt( x*x + y*y + z*z );
        x *= k;
        y *= k;
        z *= k;
        w *= k;
        return *this;
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
        return Vec4( -x, -y, -z, -w );
      }

      Vec4 operator + ( const Vec4& a ) const
      {
        return Vec4( x + a.x, y + a.y, z + a.z, w + a.w );
      }

      Vec4 operator - ( const Vec4& a ) const
      {
        return Vec4( x - a.x, y - a.y, z - a.z, w + a.w );
      }

      Vec4 operator * ( float k ) const
      {
        return Vec4( x * k, y * k, z * k, w * k );
      }

      Vec4 operator / ( float k ) const
      {
        assert( k != 0.0f );

        k = 1.0f / k;
        return Vec4( x * k, y * k, z * k, w * k );
      }

      Vec4& operator += ( const Vec4& a )
      {
        x += a.x;
        y += a.y;
        z += a.z;
        w += a.w;
        return *this;
      }

      Vec4& operator -= ( const Vec4& a )
      {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        w -= a.w;
        return *this;
      }

      Vec4& operator *= ( float k )
      {
        x *= k;
        y *= k;
        z *= k;
        w *= k;
        return *this;
      }

      Vec4& operator /= ( float k )
      {
        assert( k != 0.0f );

        k = 1.0f / k;
        x *= k;
        y *= k;
        z *= k;
        w *= k;
        return *this;
      }

      // dot product
      float operator * ( const Vec4& a ) const
      {
        return x*a.x + y*a.y + z*a.z;
      }

      // cross product
      Vec4 operator ^ ( const Vec4& a ) const
      {
        return Vec4( y*a.z - z*a.y, z*a.x - x*a.z, x*a.y - y*a.x );
      }

      // vector which lies in plane defined by given vectors and is perpendicular to first one
      Vec4 operator % ( const Vec4& a ) const
      {
        // this is actually -( u x v ) x u, where u is* this vector
        // This equals to:
        // ( u . u )v - ( u . v )u = |u|^2 * ( v - ( u . v )/( |u|^2 ) * u )
        // ( the length doesn't matter )

        // |u|^2, assume it's not 0
        float k = x*x + y*y + z*z;

        assert( k != 0.0f );

        k = ( x * a.x + y * a.y + z * a.z ) / k;
        return a - Vec4( x * k, y * k, z * k, w * k );
      }

      friend Vec4 operator * ( float k, const Vec4& a )
      {
        return Vec4( a.x * k, a.y * k, a.z * k, a.w * k );
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
