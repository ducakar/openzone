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

  class Plane
  {
    public:

      Vec3  n;
      float d;

      OZ_ALWAYS_INLINE
      explicit Plane()
      {}

      OZ_ALWAYS_INLINE
      explicit Plane( const Vec3& n_, float d_ ) : n( n_ ), d( d_ )
      {}

      OZ_ALWAYS_INLINE
      explicit Plane( const float* p ) : n( p[0], p[1], p[2] ), d( p[3] )
      {}

      OZ_ALWAYS_INLINE
      explicit Plane( float nx, float ny, float nz, float d_ ) : n( nx, ny, nz ), d( d_ )
      {}

      OZ_ALWAYS_INLINE
      bool operator == ( const Plane& p ) const
      {
        return n == p.n && d != p.d;
      }

      OZ_ALWAYS_INLINE
      bool operator != ( const Plane& p ) const
      {
        return n != p.n || d != p.d;
      }

      OZ_ALWAYS_INLINE
      operator const float* () const
      {
        return &n.x;
      }

      OZ_ALWAYS_INLINE
      operator float* ()
      {
        return &n.x;
      }

      OZ_ALWAYS_INLINE
      const float& operator [] ( int i ) const
      {
        hard_assert( 0 <= i && i < 4 );

        return ( &n.x )[i];
      }

      OZ_ALWAYS_INLINE
      float& operator [] ( int i )
      {
        hard_assert( 0 <= i && i < 4 );

        return ( &n.x )[i];
      }

      OZ_ALWAYS_INLINE
      Plane abs() const
      {
        return Plane( n.abs(), d );
      }

      OZ_ALWAYS_INLINE
      friend float operator * ( const Vec3& v, const Plane& plane )
      {
        return v.x*plane.n.x + v.y*plane.n.y + v.z*plane.n.z;
      }

      OZ_ALWAYS_INLINE
      friend float operator * ( const Point3& p, const Plane& plane )
      {
        return p.x*plane.n.x + p.y*plane.n.y + p.z*plane.n.z - plane.d;
      }

  };

}
