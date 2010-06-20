/*
 *  Sphere.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

namespace oz
{

  class Sphere
  {
    public:

      Vec4  p;
      float r;

      explicit Sphere()
      {}

      explicit Sphere( const Vec4& p_, float r_ ) : p( p_ ), r( r_ )
      {}

      Sphere operator + ( const Vec4& v ) const
      {
        return Sphere( p + v, r );
      }

      Sphere operator - ( const Vec4& v ) const
      {
        return Sphere( p - v, r );
      }

      Sphere& operator += ( const Vec4& v )
      {
        p += v;
        return *this;
      }

      Sphere& operator -= ( const Vec4& v )
      {
        p -= v;
        return *this;
      }

  };

}
