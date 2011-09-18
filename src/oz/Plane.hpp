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

  class Plane : public Simd
  {
    public:

      OZ_ALWAYS_INLINE
      Plane()
      {}

      OZ_ALWAYS_INLINE
      explicit Plane( const Vec3& n, float d ) : Simd( n.x, n.y, n.z, d )
      {}

      OZ_ALWAYS_INLINE
      explicit Plane( const float* p ) : Simd( p[0], p[1], p[2],  p[3] )
      {}

      OZ_ALWAYS_INLINE
      explicit Plane( float nx, float ny, float nz, float d ) : Simd( nx, ny, nz, d )
      {}

      OZ_ALWAYS_INLINE
      bool operator == ( const Plane& p ) const
      {
        return x == p.x && y == p.y && z == p.z && w == p.w;
      }

      OZ_ALWAYS_INLINE
      bool operator != ( const Plane& p ) const
      {
        return x != p.x || y != p.y || z != p.z || w != p.w;
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
        hard_assert( 0 <= i && i < 4 );

        return f[i];
      }

      OZ_ALWAYS_INLINE
      float& operator [] ( int i )
      {
        hard_assert( 0 <= i && i < 4 );

        return f[i];
      }

      OZ_ALWAYS_INLINE
      Vec3 n() const
      {
        return Vec3( x, y, z, 0.0f );
      }

      OZ_ALWAYS_INLINE
      float d() const
      {
        return w;
      }

      OZ_ALWAYS_INLINE
      Plane abs() const
      {
        return Plane( Math::abs( x ), Math::abs( y ), Math::abs( z ), w );
      }

      OZ_ALWAYS_INLINE
      friend float operator * ( const Vec3& v, const Plane& plane )
      {
        return v.x*plane.x + v.y*plane.y + v.z*plane.z;
      }

      OZ_ALWAYS_INLINE
      friend float operator * ( const Point3& p, const Plane& plane )
      {
        return p.x*plane.x + p.y*plane.y + p.z*plane.z - plane.w;
      }

  };

}
