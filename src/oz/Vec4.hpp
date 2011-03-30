/*
 *  Vec4.hpp
 *
 *  Generic four-component vector
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "Point3.hpp"

namespace oz
{

  class Vec4 : public Vec3
  {
    public:

      static const Vec4 ZERO;
      static const Vec4 ONE;
      static const Vec4 ID;

      float w;

      Vec4() = default;

      OZ_ALWAYS_INLINE
      explicit Vec4( float x, float y, float z, float w_ ) : Vec3( x, y, z ), w( w_ )
      {}

      OZ_ALWAYS_INLINE
      explicit Vec4( const float* v ) : Vec3( v ), w( v[3] )
      {}

      OZ_ALWAYS_INLINE
      explicit Vec4( const Vec3& v, float w_ = 0.0f ) : Vec3( v ), w( w_ )
      {}

      OZ_ALWAYS_INLINE
      explicit Vec4( const Point3& p ) : Vec3( p.x, p.y, p.z ), w( 1.0f )
      {}

      OZ_ALWAYS_INLINE
      Vec4& operator = ( const Vec3& v )
      {
        x = v.x;
        y = v.y;
        z = v.z;
        w = 0.0f;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Vec4& operator = ( const Point3& v )
      {
        x = v.x;
        y = v.y;
        z = v.z;
        w = 1.0f;
        return *this;
      }

      OZ_ALWAYS_INLINE
      bool operator == ( const Vec4& v ) const
      {
        return x == v.x && y == v.y && z == v.z && w == v.w;
      }

      OZ_ALWAYS_INLINE
      bool operator != ( const Vec4& v ) const
      {
        return x != v.x || y != v.y || z != v.z || w != v.w;
      }

      OZ_ALWAYS_INLINE
      const float& operator [] ( int i ) const
      {
        hard_assert( 0 <= i && i < 4 );

        return ( &x )[i];
      }

      OZ_ALWAYS_INLINE
      float& operator [] ( int i )
      {
        hard_assert( 0 <= i && i < 4 );

        return ( &x )[i];
      }

      OZ_ALWAYS_INLINE
      Vec4 abs() const
      {
        return Vec4( Math::abs( x ), Math::abs( y ), Math::abs( z ), Math::abs( w ) );
      }

      OZ_ALWAYS_INLINE
      float operator ! () const
      {
        return Math::sqrt( x*x + y*y + z*z + w*w );
      }

      OZ_ALWAYS_INLINE
      float fastL() const
      {
         return Math::fastSqrt( x*x + y*y + z*z + w*w );
      }

      OZ_ALWAYS_INLINE
      float sqL() const
      {
        return x*x + y*y + z*z + w*w;
      }

      OZ_ALWAYS_INLINE
      Vec4 operator ~ () const
      {
        hard_assert( x*x + y*y + z*z + w*w > 0.0f );

        float k = 1.0f / Math::sqrt( x*x + y*y + z*z + w*w );
        return Vec4( x * k, y * k, z * k, w * k );
      }

      OZ_ALWAYS_INLINE
      Vec4 fastUnit() const
      {
        hard_assert( x*x + y*y + z*z + w*w > 0.0f );

        float k = Math::fastInvSqrt( x*x + y*y + z*z + w*w );
        return Vec4( x * k, y * k, z * k, w * k );
      }

      OZ_ALWAYS_INLINE
      Vec4 operator + () const
      {
        return *this;
      }

      OZ_ALWAYS_INLINE
      Vec4 operator - () const
      {
        return Vec4( -x, -y, -z, -w );
      }

      OZ_ALWAYS_INLINE
      Vec4 operator + ( const Vec4& v ) const
      {
        return Vec4( x + v.x, y + v.y, z + v.z, w + v.w );
      }

      OZ_ALWAYS_INLINE
      Vec4 operator - ( const Vec4& v ) const
      {
        return Vec4( x - v.x, y - v.y, z - v.z, w - v.w );
      }

      OZ_ALWAYS_INLINE
      Vec4 operator * ( float k ) const
      {
        return Vec4( x * k, y * k, z * k, w * k );
      }

      OZ_ALWAYS_INLINE
      friend Vec4 operator * ( float k, const Vec4& v )
      {
        return Vec4( k * v.x, k * v.y, k * v.z, k * v.w );
      }

      OZ_ALWAYS_INLINE
      Vec4 operator / ( float k ) const
      {
        hard_assert( k != 0.0f );

        k = 1.0f / k;
        return Vec4( x * k, y * k, z * k, w * k );
      }

      OZ_ALWAYS_INLINE
      Vec4& operator += ( const Vec4& v )
      {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Vec4& operator -= ( const Vec4& v )
      {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Vec4& operator *= ( float k )
      {
        x *= k;
        y *= k;
        z *= k;
        w *= k;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Vec4& operator /= ( float k )
      {
        hard_assert( k != 0.0f );

        k = 1.0f / k;
        x *= k;
        y *= k;
        z *= k;
        w *= k;
        return *this;
      }

      OZ_ALWAYS_INLINE
      float operator * ( const Vec4& v ) const
      {
        return x*v.x + y*v.y + z*v.z + w*v.w;
      }

  };

}
