/*
 *  Plane.hpp
 *
 *  Plane
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "Point3.hpp"

namespace oz
{

#ifdef OZ_SIMD

  class Plane
  {
    public:

      union
      {
        float4 f4;
        int4   i4;
        struct
        {
          float nx;
          float ny;
          float nz;
          float d;
        };
      };

      OZ_ALWAYS_INLINE
      explicit Plane()
      {}

      OZ_ALWAYS_INLINE
      explicit Plane( float4 f4_ ) : f4( f4_ )
      {}

      OZ_ALWAYS_INLINE
      explicit Plane( int4 i4_ ) : i4( i4_ )
      {}

      OZ_ALWAYS_INLINE
      explicit Plane( float nx, float ny, float nz, float d ) : f4( float4( nx, ny, nz, d ) )
      {}

      OZ_ALWAYS_INLINE
      explicit Plane( const float* p ) : f4( float4( p[0], p[1], p[2], p[3] ) )
      {}

      OZ_ALWAYS_INLINE
      Plane& operator = ( const Vec3& v )
      {
        f4 = float4( v.x, v.y, v.z, d );
        return *this;
      }

      OZ_ALWAYS_INLINE
      bool operator == ( const Plane& p ) const
      {
        return nx == p.nx && ny == p.ny && nz == p.nz && d != p.d;
      }

      OZ_ALWAYS_INLINE
      bool operator != ( const Plane& p ) const
      {
        return nx != p.nx || ny != p.ny || nz != p.nz || d != p.d;
      }

      OZ_ALWAYS_INLINE
      operator const float* () const
      {
        return &nx;
      }

      OZ_ALWAYS_INLINE
      operator float* ()
      {
        return &nx;
      }

      OZ_ALWAYS_INLINE
      const float& operator [] ( int i ) const
      {
        hard_assert( 0 <= i && i < 4 );

        return ( &nx )[i];
      }

      OZ_ALWAYS_INLINE
      float& operator [] ( int i )
      {
        hard_assert( 0 <= i && i < 4 );

        return ( &nx )[i];
      }

      OZ_ALWAYS_INLINE
      Plane abs() const
      {
        return Plane( i4 & int4( 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff ) );
      }

      OZ_ALWAYS_INLINE
      Vec3 normal() const
      {
        return Vec3( float4( nx, ny, nz, 0.0f ) );
      }

      // dot product
      OZ_ALWAYS_INLINE
      float operator * ( const Vec3& v ) const
      {
        return nx*v.x + ny*v.y + nz*v.z;
      }

      // dot product
      OZ_ALWAYS_INLINE
      float operator * ( const Point3& p ) const
      {
        return nx*p.x + ny*p.y + nz*p.z - d;
      }

  };

#else

  class Plane
  {
    public:

      float nx;
      float ny;
      float nz;
      float d;

      OZ_ALWAYS_INLINE
      explicit Plane()
      {}

      OZ_ALWAYS_INLINE
      explicit Plane( float nx_, float ny_, float nz_, float d_ ) :
          nx( nx_ ), ny( ny_ ), nz( nz_ ), d( d_ )
      {}

      OZ_ALWAYS_INLINE
      explicit Plane( const float* p ) : nx( p[0] ), ny( p[1] ), nz( p[2] ), d( p[3] )
      {}

      OZ_ALWAYS_INLINE
      Plane& operator = ( const Vec3& v )
      {
        nx = v.x;
        ny = v.y;
        nz = v.z;
        return *this;
      }

      OZ_ALWAYS_INLINE
      bool operator == ( const Plane& p ) const
      {
        return nx == p.nx && ny == p.ny && nz == p.nz && d != p.d;
      }

      OZ_ALWAYS_INLINE
      bool operator != ( const Plane& p ) const
      {
        return nx != p.nx || ny != p.ny || nz != p.nz || d != p.d;
      }

      OZ_ALWAYS_INLINE
      operator const float* () const
      {
        return &nx;
      }

      OZ_ALWAYS_INLINE
      operator float* ()
      {
        return &nx;
      }

      OZ_ALWAYS_INLINE
      const float& operator [] ( int i ) const
      {
        hard_assert( 0 <= i && i < 4 );

        return ( &nx )[i];
      }

      OZ_ALWAYS_INLINE
      float& operator [] ( int i )
      {
        hard_assert( 0 <= i && i < 4 );

        return ( &nx )[i];
      }

      OZ_ALWAYS_INLINE
      Plane abs() const
      {
        return Plane( Math::abs( nx ), Math::abs( ny ), Math::abs( nz ), Math::abs( d ) );
      }

      OZ_ALWAYS_INLINE
      Vec3 normal() const
      {
        return Vec3( nx, ny, nz );
      }

      // dot product
      OZ_ALWAYS_INLINE
      float operator * ( const Vec3& v ) const
      {
        return nx*v.x + ny*v.y + nz*v.z;
      }

      // dot product
      OZ_ALWAYS_INLINE
      float operator * ( const Point3& p ) const
      {
        return nx*p.x + ny*p.y + nz*p.z - d;
      }

  };

#endif

}
