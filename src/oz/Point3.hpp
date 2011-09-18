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

  class Point3 : public Simd
  {
    public:

      static const Point3 ORIGIN;

      OZ_ALWAYS_INLINE
      Point3()
      {}

      OZ_ALWAYS_INLINE
      explicit Point3( float x_, float y_, float z_ ) : Simd( x_, y_, z_, 1.0f )
      {}

      OZ_ALWAYS_INLINE
      explicit Point3( const float* v ) : Simd( v[0], v[1], v[2], 1.0 )
      {}

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
        return f;
      }

      OZ_ALWAYS_INLINE
      operator float* ()
      {
        return f;
      }

      OZ_ALWAYS_INLINE
      const float& operator [] ( int i ) const
      {
        hard_assert( 0 <= i && i < 3 );

        return f[i];
      }

      OZ_ALWAYS_INLINE
      float& operator [] ( int i )
      {
        hard_assert( 0 <= i && i < 3 );

        return f[i];
      }

      OZ_ALWAYS_INLINE
      Point3 operator + ( const Vec3& v ) const
      {
        return Point3( x + v.x, y + v.y, z + v.z );
      }

      OZ_ALWAYS_INLINE
      Point3 operator - ( const Vec3& v ) const
      {
        return Point3( x - v.x, y - v.y, z - v.z );
      }

      OZ_ALWAYS_INLINE
      Vec3 operator - ( const Point3& p ) const
      {
        return Vec3( x - p.x, y - p.y, z - p.z );
      }

      OZ_ALWAYS_INLINE
      Point3& operator += ( const Vec3& v )
      {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Point3& operator -= ( const Vec3& v )
      {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
      }

      OZ_ALWAYS_INLINE
      float operator * ( const Vec3& v ) const
      {
        return x*v.x + y*v.y + z*v.z;
      }

  };

}
