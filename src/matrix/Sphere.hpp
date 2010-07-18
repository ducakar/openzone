/*
 *  Sphere.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

namespace oz
{

  class Sphere
  {
    public:

      Vec3  p;
      float r;

      explicit Sphere()
      {}

      explicit Sphere( const Vec3& p_, float r_ ) : p( p_ ), r( r_ )
      {}

      Sphere operator + ( const Vec3& v ) const
      {
        return Sphere( p + v, r );
      }

      Sphere operator - ( const Vec3& v ) const
      {
        return Sphere( p - v, r );
      }

      Sphere& operator += ( const Vec3& v )
      {
        p += v;
        return *this;
      }

      Sphere& operator -= ( const Vec3& v )
      {
        p -= v;
        return *this;
      }

  };

}
