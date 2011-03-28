/*
 *  Sphere.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

namespace oz
{

  class Sphere
  {
    public:

      Point3 p;
      float  r;

    protected:

      OZ_ALWAYS_INLINE
      explicit Sphere( const Point3& p_ ) : p( p_ )
      {}

    public:

      OZ_ALWAYS_INLINE
      explicit Sphere() = default;

      OZ_ALWAYS_INLINE
      explicit Sphere( const Point3& p_, float r_ ) : p( p_ ), r( r_ )
      {}

      OZ_ALWAYS_INLINE
      Sphere operator + ( const Vec3& v ) const
      {
        return Sphere( p + v, r );
      }

      OZ_ALWAYS_INLINE
      Sphere operator - ( const Vec3& v ) const
      {
        return Sphere( p - v, r );
      }

      OZ_ALWAYS_INLINE
      Sphere& operator += ( const Vec3& v )
      {
        p += v;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Sphere& operator -= ( const Vec3& v )
      {
        p -= v;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Sphere operator * ( float k ) const
      {
        return Sphere( p, r * k );
      }

      OZ_ALWAYS_INLINE
      Sphere operator / ( float k ) const
      {
        return Sphere( p, r / k );
      }

      OZ_ALWAYS_INLINE
      Sphere& operator *= ( float k )
      {
        r *= k;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Sphere& operator /= ( float k )
      {
        r /= k;
        return *this;
      }

  };

}
