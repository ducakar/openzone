/*
 *  Plane.hpp
 *
 *  Plane
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "Point3.hpp"

namespace oz
{

#ifdef OZ_SIMD
  class Plane : public Simd
#else
  class Plane
#endif
  {
    public:

#ifndef OZ_SIMD
      float nx;
      float ny;
      float nz;
      float d;
#endif

      OZ_ALWAYS_INLINE
      Plane()
      {}

#ifdef OZ_SIMD
    protected:

      OZ_ALWAYS_INLINE
      explicit Plane( uint4 u4 ) : Simd( u4 )
      {}

      OZ_ALWAYS_INLINE
      explicit Plane( float4 f4 ) : Simd( f4 )
      {}

  public:
#endif

#ifdef OZ_SIMD
      OZ_ALWAYS_INLINE
      explicit Plane( const Vec3& n, float d ) : Simd( float4( n.x, n.y, n.z, d ) )
      {}
#else
      OZ_ALWAYS_INLINE
      explicit Plane( const Vec3& n, float d_ ) : nx( n.x ), ny( n.y ), nz( n.z ), d( d_ )
      {}
#endif

#ifdef OZ_SIMD
      OZ_ALWAYS_INLINE
      explicit Plane( const float* p ) : Simd( float4( p[0], p[1], p[2],  p[3] ) )
      {}
#else
      OZ_ALWAYS_INLINE
      explicit Plane( const float* p ) : nx( p[0] ), ny( p[1] ), nz( p[2] ), d( p[3] )
      {}
#endif

#ifdef OZ_SIMD
      OZ_ALWAYS_INLINE
      explicit Plane( float nx, float ny, float nz, float d ) : Simd( float4( nx, ny, nz, d ) )
      {}
#else
      OZ_ALWAYS_INLINE
      explicit Plane( float nx_, float ny_, float nz_, float d_ ) :
          nx( nx_ ), ny( ny_ ), nz( nz_ ), d( d_ )
      {}
#endif

      OZ_ALWAYS_INLINE
      bool operator == ( const Plane& p ) const
      {
        return nx == p.nx && ny == p.ny && nz == p.nz && d == p.d;
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
      Vec3 n() const
      {
#ifdef OZ_SIMD
        return Vec3( u4 & uint4( 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff ) );
#else
        return Vec3( nx, ny, nz );
#endif
      }

      OZ_ALWAYS_INLINE
      Plane abs() const
      {
#ifdef OZ_SIMD
        return Plane( u4 & uint4( 0x7fffffff, 0x7fffffff, 0x7fffffff, 0xffffffff ) );
#else
        return Plane( Math::abs( nx ), Math::abs( ny ), Math::abs( nz ), d );
#endif
      }

      OZ_ALWAYS_INLINE
      friend float operator * ( const Vec3& v, const Plane& plane )
      {
        return v.x*plane.nx + v.y*plane.ny + v.z*plane.nz;
      }

      OZ_ALWAYS_INLINE
      friend float operator * ( const Point3& p, const Plane& plane )
      {
        return p.x*plane.nx + p.y*plane.ny + p.z*plane.nz - plane.d;
      }

  };

}
