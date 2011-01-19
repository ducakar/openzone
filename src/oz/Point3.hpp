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

  class Point3
  {
    public:

      static const Point3 ORIGIN;

      float x;
      float y;
      float z;

    public:

      explicit Point3()
      {}

      explicit Point3( float x_, float y_, float z_ ) : x( x_ ), y( y_ ), z( z_ )
      {}

      explicit Point3( const float* v ) : x( v[0] ), y( v[1] ), z( v[2] )
      {}

      bool operator == ( const Point3& p ) const
      {
        return x == p.x && y == p.y && z == p.z;
      }

      bool operator != ( const Point3& p ) const
      {
        return x != p.x || y != p.y || z != p.z;
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

      bool equals( const Point3& p, float epsilon ) const
      {
        return
            Math::abs( x - p.x ) <= epsilon &&
            Math::abs( y - p.y ) <= epsilon &&
            Math::abs( z - p.z ) <= epsilon;
      }

      Point3 abs() const
      {
        return Point3( Math::abs( x ), Math::abs( y ), Math::abs( z ) );
      }

      bool isColinear( const Point3& p, float epsilon ) const
      {
        float p1 = p.x * y * z;
        float p2 = p.y * x * z;
        float p3 = p.z * x * y;

        return Math::abs( p1 - p2 ) <= epsilon && Math::abs( p1 - p3 ) <= epsilon;
      }

      Point3 operator + ( const Vec3& v ) const
      {
        return Point3( x + v.x, y + v.y, z + v.z );
      }

      Point3 operator - ( const Vec3& v ) const
      {
        return Point3( x - v.x, y - v.y, z - v.z );
      }

      Vec3 operator - ( const Point3& p ) const
      {
        return Vec3( x - p.x, y - p.y, z - p.z );
      }

      Point3& operator += ( const Vec3& v )
      {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
      }

      Point3& operator -= ( const Vec3& v )
      {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
      }

      // dot product
      float operator * ( const Vec3& v ) const
      {
        return x*v.x + y*v.y + z*v.z;
      }

  };

}
