/*
 *  Vec3.hpp
 *
 *  3D vector
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "Math.hpp"

namespace oz
{

#ifdef OZ_SIMD
  class Point3;
  class Plane;

  class Vec3 : public Simd
  {
    friend class Point3;
    friend class Plane;
#else
  class Vec3
  {
#endif

    public:

      static const Vec3 ZERO;

#ifndef OZ_SIMD
      float x;
      float y;
      float z;
#endif

      OZ_ALWAYS_INLINE
      Vec3()
      {}

#ifdef OZ_SIMD
  protected:

      OZ_ALWAYS_INLINE
      explicit Vec3( uint4 u4 ) : Simd( u4 )
      {}

      OZ_ALWAYS_INLINE
      explicit Vec3( float4 f4 ) : Simd( f4 )
      {}

  public:
#endif

#ifdef OZ_SIMD
      OZ_ALWAYS_INLINE
      explicit Vec3( float x, float y, float z ) : Simd( float4( x, y, z, 0.0f ) )
      {}
#else
      OZ_ALWAYS_INLINE
      explicit Vec3( float x_, float y_, float z_ ) : x( x_ ), y( y_ ), z( z_ )
      {}
#endif

#ifdef OZ_SIMD
      OZ_ALWAYS_INLINE
      explicit Vec3( const float* v ) : Simd( float4( v[0], v[1], v[2], 0.0f ) )
      {}
#else
      OZ_ALWAYS_INLINE
      explicit Vec3( const float* v ) : x( v[0] ), y( v[1] ), z( v[2] )
      {}
#endif

      OZ_ALWAYS_INLINE
      bool operator == ( const Vec3& v ) const
      {
        return x == v.x && y == v.y && z == v.z;
      }

      OZ_ALWAYS_INLINE
      bool operator != ( const Vec3& v ) const
      {
        return x != v.x || y != v.y || z != v.z;
      }

      OZ_ALWAYS_INLINE
      operator const float* () const
      {
        return &x;
      }

      OZ_ALWAYS_INLINE
      operator float* ()
      {
        return &x;
      }

      OZ_ALWAYS_INLINE
      const float& operator [] ( int i ) const
      {
        hard_assert( 0 <= i && i < 3 );

        return ( &x )[i];
      }

      OZ_ALWAYS_INLINE
      float& operator [] ( int i )
      {
        hard_assert( 0 <= i && i < 3 );

        return ( &x )[i];
      }

      OZ_ALWAYS_INLINE
      Vec3 abs() const
      {
#ifdef OZ_SIMD
        return Vec3( u4 & uint4( 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff ) );
#else
        return Vec3( Math::abs( x ), Math::abs( y ), Math::abs( z ) );
#endif
      }

      OZ_ALWAYS_INLINE
      float operator ! () const
      {
        return Math::sqrt( x*x + y*y + z*z );
      }

      OZ_ALWAYS_INLINE
      float fastL() const
      {
         return Math::fastSqrt( x*x + y*y + z*z );
      }

      OZ_ALWAYS_INLINE
      float sqL() const
      {
        return x*x + y*y + z*z;
      }

      OZ_ALWAYS_INLINE
      Vec3 operator ~ () const
      {
        hard_assert( x*x + y*y + z*z > 0.0f );

#ifdef OZ_SIMD
        float k = Math::sqrt( x*x + y*y + z*z );
        return Vec3( f4 / float4( k, k, k, k ) );
#else
        float k = 1.0f / Math::sqrt( x*x + y*y + z*z );
        return Vec3( x * k, y * k, z * k );
#endif
      }

      OZ_ALWAYS_INLINE
      Vec3 fastUnit() const
      {
        hard_assert( x*x + y*y + z*z > 0.0f );

#ifdef OZ_SIMD
        float k = Math::fastInvSqrt( x*x + y*y + z*z );
        return Vec3( f4 * float4( k, k, k, k ) );
#else
        float k = Math::fastInvSqrt( x*x + y*y + z*z );
        return Vec3( x * k, y * k, z * k );
#endif
      }

      OZ_ALWAYS_INLINE
      bool isColinear( const Vec3& v, float epsilon ) const
      {
        float p1 = v.x * y * z;
        float p2 = v.y * x * z;
        float p3 = v.z * x * y;

        return Math::abs( p1 - p2 ) <= epsilon && Math::abs( p1 - p3 ) <= epsilon;
      }

      OZ_ALWAYS_INLINE
      Vec3 operator + () const
      {
        return *this;
      }

      OZ_ALWAYS_INLINE
      Vec3 operator - () const
      {
#ifdef OZ_SIMD
        return Vec3( -f4 );
#else
        return Vec3( -x, -y, -z );
#endif
      }

      OZ_ALWAYS_INLINE
      Vec3 operator + ( const Vec3& v ) const
      {
#ifdef OZ_SIMD
        return Vec3( f4 + v.f4 );
#else
        return Vec3( x + v.x, y + v.y, z + v.z );
#endif
      }

      OZ_ALWAYS_INLINE
      Vec3 operator - ( const Vec3& v ) const
      {
#ifdef OZ_SIMD
        return Vec3( f4 - v.f4 );
#else
        return Vec3( x - v.x, y - v.y, z - v.z );
#endif
      }

      OZ_ALWAYS_INLINE
      Vec3 operator * ( float k ) const
      {
#ifdef OZ_SIMD
        return Vec3( f4 * float4( k, k, k, k ) );
#else
        return Vec3( x * k, y * k, z * k );
#endif
      }

      OZ_ALWAYS_INLINE
      friend Vec3 operator * ( float k, const Vec3& v )
      {
#ifdef OZ_SIMD
        return Vec3( float4( k, k, k, k ) * v.f4 );
#else
        return Vec3( k * v.x, k * v.y, k * v.z );
#endif
      }

      OZ_ALWAYS_INLINE
      Vec3 operator / ( float k ) const
      {
        hard_assert( k != 0.0f );

#ifdef OZ_SIMD
        return Vec3( f4 / float4( k, k, k, k ) );
#else
        k = 1.0f / k;
        return Vec3( x * k, y * k, z * k );
#endif
      }

      OZ_ALWAYS_INLINE
      Vec3& operator += ( const Vec3& v )
      {
#ifdef OZ_SIMD
        f4 += v.f4;
#else
        x += v.x;
        y += v.y;
        z += v.z;
#endif
        return *this;
      }

      OZ_ALWAYS_INLINE
      Vec3& operator -= ( const Vec3& v )
      {
#ifdef OZ_SIMD
        f4 -= v.f4;
#else
        x -= v.x;
        y -= v.y;
        z -= v.z;
#endif
        return *this;
      }

      OZ_ALWAYS_INLINE
      Vec3& operator *= ( float k )
      {
#ifdef OZ_SIMD
        f4 *= float4( k, k, k, k );
#else
        x *= k;
        y *= k;
        z *= k;
#endif
        return *this;
      }

      OZ_ALWAYS_INLINE
      Vec3& operator /= ( float k )
      {
        hard_assert( k != 0.0f );

#ifdef OZ_SIMD
        f4 /= float4( k, k, k, k );
#else
        k = 1.0f / k;
        x *= k;
        y *= k;
        z *= k;
#endif
        return *this;
      }

      // dot product
      OZ_ALWAYS_INLINE
      float operator * ( const Vec3& v ) const
      {
        return x*v.x + y*v.y + z*v.z;
      }

      // cross product
      OZ_ALWAYS_INLINE
      Vec3 operator ^ ( const Vec3& v ) const
      {
        return Vec3( y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x );
      }

  };

}
