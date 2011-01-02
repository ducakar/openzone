/*
 *  Point4.hpp
 *
 *  3D point in 4D space optimised for SIMD
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "Vec4.hpp"

namespace oz
{

  class __attribute__(( aligned( 16 ) )) Point4
  {
    public:

      static const Point4 ORIGIN;

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

    protected:

      static float dot( float4 a, float4 b )
      {
        Vec4 v( a * b );
        return v.x + v.y + v.z;
      }

    public:

      explicit Point4()
      {}

      explicit Point4( float4 f4_ ) : f4( f4_ )
      {}

      explicit Point4( float x, float y, float z ) : f4( float4( x, y, z, 1.0f ) )
      {}

      explicit Point4( const float* v ) : f4( float4( v[0], v[1], v[2], 1.0f ) )
      {}

      bool operator == ( const Point4& p ) const
      {
        return x == p.x && y == p.y && z == p.z;
      }

      bool operator != ( const Point4& p ) const
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

      bool equals( const Point4& p, float epsilon ) const
      {
        return
            Math::abs( x - p.x ) <= epsilon &&
            Math::abs( y - p.y ) <= epsilon &&
            Math::abs( z - p.z ) <= epsilon;
      }

      Point4 abs() const
      {
        return Point4( Math::abs( x ), Math::abs( y ), Math::abs( z ) );
      }

      bool isColinear( const Point4& p, float epsilon ) const
      {
        float p1 = p.x * y * z;
        float p2 = p.y * x * z;
        float p3 = p.z * x * y;

        return Math::abs( p1 - p2 ) <= epsilon && Math::abs( p1 - p3 ) <= epsilon;
      }

      Point4 operator + ( const Vec4& v ) const
      {
        return Point4( f4 + v.f4 );
      }

      Point4 operator - ( const Vec4& v ) const
      {
        return Point4( f4 - v.f4 );
      }

      Vec4 operator - ( const Point4& p ) const
      {
        return Vec4( f4 - p.f4 );
      }

      Point4& operator += ( const Vec4& v )
      {
        f4 += v.f4;
        return *this;
      }

      Point4& operator -= ( const Vec4& v )
      {
        f4 += v.f4;
        return *this;
      }

      // dot product
      float operator * ( const Vec4& v ) const
      {
        return x*v.x + y*v.y + z*v.z;
      }

  };

}
