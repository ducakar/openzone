/*
 *  Point3.hpp
 *
 *  3D point
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "Vec3.hpp"

namespace oz
{

#ifdef OZ_SIMD
  class Point3 : public Simd
#else
  class Point3
#endif
  {
    public:

      static const Point3 ORIGIN;

#ifndef OZ_SIMD
      float x;
      float y;
      float z;
#endif

      OZ_ALWAYS_INLINE
      Point3()
      {}

#ifdef OZ_SIMD
  protected:

      OZ_ALWAYS_INLINE
      explicit Point3( uint4 u4 ) : Simd( u4 )
      {}

      OZ_ALWAYS_INLINE
      explicit Point3( float4 f4 ) : Simd( f4 )
      {}

  public:
#endif

#ifdef OZ_SIMD
      OZ_ALWAYS_INLINE
      explicit Point3( float x, float y, float z ) : Simd( float4( x, y, z, 1.0f ) )
      {}
#else
      OZ_ALWAYS_INLINE
      explicit Point3( float x_, float y_, float z_ ) : x( x_ ), y( y_ ), z( z_ )
      {}
#endif

#ifdef OZ_SIMD
      OZ_ALWAYS_INLINE
      explicit Point3( const float* v ) : Simd( float4( v[0], v[1], v[2], 1.0 ) )
      {}
#else
      OZ_ALWAYS_INLINE
      explicit Point3( const float* v ) : x( v[0] ), y( v[1] ), z( v[2] )
      {}
#endif

      OZ_ALWAYS_INLINE
      bool operator == ( const Point3& p ) const
      {
        return x == p.x && y == p.y && z == p.z;
      }

      OZ_ALWAYS_INLINE
      bool operator != ( const Point3& p ) const
      {
        return x != p.x || y != p.y || z != p.z;
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
      Point3 operator + ( const Vec3& v ) const
      {
#ifdef OZ_SIMD
        return Point3( f4 + v.f4 );
#else
        return Point3( x + v.x, y + v.y, z + v.z );
#endif
      }

      OZ_ALWAYS_INLINE
      Point3 operator - ( const Vec3& v ) const
      {
#ifdef OZ_SIMD
        return Point3( f4 - v.f4 );
#else
        return Point3( x - v.x, y - v.y, z - v.z );
#endif
      }

      OZ_ALWAYS_INLINE
      Vec3 operator - ( const Point3& p ) const
      {
#ifdef OZ_SIMD
        return Vec3( f4 - p.f4 );
#else
        return Vec3( x - p.x, y - p.y, z - p.z );
#endif
      }

      OZ_ALWAYS_INLINE
      Point3& operator += ( const Vec3& v )
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
      Point3& operator -= ( const Vec3& v )
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
      float operator * ( const Vec3& v ) const
      {
        return x*v.x + y*v.y + z*v.z;
      }

  };

}
