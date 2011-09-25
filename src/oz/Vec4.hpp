/*
 *  Vec4.hpp
 *
 *  Generic four-component vector
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
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

#ifndef OZ_SIMD
      float w;
#endif

      OZ_ALWAYS_INLINE
      Vec4()
      {}

#ifdef OZ_SIMD
  protected:

      OZ_ALWAYS_INLINE
      explicit Vec4( uint4 u4 ) : Vec3( u4 )
      {}

      OZ_ALWAYS_INLINE
      explicit Vec4( float4 f4 ) : Vec3( f4 )
      {}

  public:
#endif

#ifdef OZ_SIMD
      OZ_ALWAYS_INLINE
      explicit Vec4( float x, float y, float z, float w ) : Vec3( float4( x, y, z, w ) )
      {}
#else
      OZ_ALWAYS_INLINE
      explicit Vec4( float x, float y, float z, float w_ ) : Vec3( x, y, z ), w( w_ )
      {}
#endif

#ifdef OZ_SIMD
      OZ_ALWAYS_INLINE
      explicit Vec4( const float* v ) : Vec3( float4( v[0], v[1], v[2], v[3] ) )
      {}
#else
      OZ_ALWAYS_INLINE
      explicit Vec4( const float* v ) : Vec3( v[0], v[1], v[2] ), w( v[3] )
      {}
#endif

#ifdef OZ_SIMD
      OZ_ALWAYS_INLINE
      explicit Vec4( const Vec3& v, float w = 0.0f ) : Vec3( float4( v.x, v.y, v.z, w ) )
      {}
#else
      OZ_ALWAYS_INLINE
      explicit Vec4( const Vec3& v, float w_ = 0.0f ) : Vec3( v.x, v.y, v.z ), w( w_ )
      {}
#endif

#ifdef OZ_SIMD
      OZ_ALWAYS_INLINE
      explicit Vec4( const Point3& p ) : Vec3( float4( p.x, p.y, p.z, 1.0f ) )
      {}
#else
      OZ_ALWAYS_INLINE
      explicit Vec4( const Point3& p ) : Vec3( p.x, p.y, p.z ), w( 1.0f )
      {}
#endif

      OZ_ALWAYS_INLINE
      Vec4& operator = ( const Vec3& v )
      {
#ifdef OZ_SIMD
        f4 = v.f4;
#else
        x = v.x;
        y = v.y;
        z = v.z;
        w = 0.0f;
#endif
        return *this;
      }

      OZ_ALWAYS_INLINE
      Vec4& operator = ( const Point3& v )
      {
#ifdef OZ_SIMD
        f4 = v.f4;
#else
        x = v.x;
        y = v.y;
        z = v.z;
        w = 1.0f;
#endif
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
#ifdef OZ_SIMD
        return Vec4( u4 & uint4( 0x7fffffff, 0x7fffffff, 0x7f, 0x7fffffff ) );
#else
        return Vec4( Math::abs( x ), Math::abs( y ), Math::abs( z ), Math::abs( w ) );
#endif
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

#ifdef OZ_SIMD
        float k = Math::sqrt( x*x + y*y + z*z + w*w );
        return Vec4( f4 / float4( k, k, k, k ) );
#else
        float k = 1.0f / Math::sqrt( x*x + y*y + z*z + w*w );
        return Vec4( x * k, y * k, z * k, w * k );
#endif
      }

      OZ_ALWAYS_INLINE
      Vec4 fastUnit() const
      {
        hard_assert( x*x + y*y + z*z + w*w > 0.0f );

#ifdef OZ_SIMD
        float k = Math::fastInvSqrt( x*x + y*y + z*z + w*w );
        return Vec4( f4 * float4( k, k, k, k ) );
#else
        float k = Math::fastInvSqrt( x*x + y*y + z*z + w*w );
        return Vec4( x * k, y * k, z * k, w * k );
#endif
      }

      OZ_ALWAYS_INLINE
      Vec4 operator + () const
      {
        return *this;
      }

      OZ_ALWAYS_INLINE
      Vec4 operator - () const
      {
#ifdef OZ_SIMD
        return Vec4( -f4 );
#else
        return Vec4( -x, -y, -z, -w );
#endif
      }

      OZ_ALWAYS_INLINE
      Vec4 operator + ( const Vec4& v ) const
      {
#ifdef OZ_SIMD
        return Vec4( f4 + v.f4 );
#else
        return Vec4( x + v.x, y + v.y, z + v.z, w + v.w );
#endif
      }

      OZ_ALWAYS_INLINE
      Vec4 operator - ( const Vec4& v ) const
      {
#ifdef OZ_SIMD
        return Vec4( f4 - v.f4 );
#else
        return Vec4( x - v.x, y - v.y, z - v.z, w - v.w );
#endif
      }

      OZ_ALWAYS_INLINE
      Vec4 operator * ( float k ) const
      {
#ifdef OZ_SIMD
        return Vec4( f4 * float4( k, k, k, k ) );
#else
        return Vec4( x * k, y * k, z * k, w * k );
#endif
      }

      OZ_ALWAYS_INLINE
      friend Vec4 operator * ( float k, const Vec4& v )
      {
#ifdef OZ_SIMD
        return Vec4( float4( k, k, k, k ) * v.f4 );
#else
        return Vec4( k * v.x, k * v.y, k * v.z, k * v.w );
#endif
      }

      OZ_ALWAYS_INLINE
      Vec4 operator / ( float k ) const
      {
        hard_assert( k != 0.0f );

#ifdef OZ_SIMD
        return Vec4( f4 / float4( k, k, k, k ) );
#else
        k = 1.0f / k;
        return Vec4( x * k, y * k, z * k, w * k );
#endif
      }

      OZ_ALWAYS_INLINE
      Vec4& operator += ( const Vec4& v )
      {
#ifdef OZ_SIMD
        f4 += v.f4;
#else
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
#endif
        return *this;
      }

      OZ_ALWAYS_INLINE
      Vec4& operator -= ( const Vec4& v )
      {
#ifdef OZ_SIMD
        f4 -= v.f4;
#else
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
#endif
        return *this;
      }

      OZ_ALWAYS_INLINE
      Vec4& operator *= ( float k )
      {
#ifdef OZ_SIMD
        f4 *= float4( k, k, k, k );
#else
        x *= k;
        y *= k;
        z *= k;
        w *= k;
#endif
        return *this;
      }

      OZ_ALWAYS_INLINE
      Vec4& operator /= ( float k )
      {
        hard_assert( k != 0.0f );

#ifdef OZ_SIMD
        f4 /= float4( k, k, k, k );
#else
        k = 1.0f / k;
        x *= k;
        y *= k;
        z *= k;
        w *= k;
#endif
        return *this;
      }

      OZ_ALWAYS_INLINE
      float operator * ( const Vec4& v ) const
      {
        return x*v.x + y*v.y + z*v.z + w*v.w;
      }

  };

}
