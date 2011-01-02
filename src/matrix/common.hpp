/*
 *  common.hpp
 *
 *  Common matrix includes and definitions
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

namespace oz
{
  // ensure epsilon is big enough for a 4 km x 4 km world
  const float EPSILON = 2048.0f * 4.0f * Math::EPSILON;

  struct Span
  {
    int minX;
    int minY;
    int maxX;
    int maxY;

    explicit Span()
    {}

    explicit Span( int minX_, int minY_, int maxX_, int maxY_ ) :
        minX( minX_ ), minY( minY_ ), maxX( maxX_ ), maxY( maxY_ )
    {}
  };

  struct Material
  {
    static const int VOID_BIT    = 0x00000001;
    static const int TERRAIN_BIT = 0x00000002;
    static const int WATER_BIT   = 0x00000004;
    static const int STRUCT_BIT  = 0x00000008;
    static const int SLICK_BIT   = 0x00000010;
    static const int LADDER_BIT  = 0x00000020;
    static const int OBJECT_BIT  = 0x00000040;
  };

  struct TexCoord
  {
    static const TexCoord ZERO;

    float u;
    float v;

    explicit TexCoord()
    {}

    explicit TexCoord( float u_, float v_ ) : u( u_ ), v( v_ )
    {}

    bool operator == ( const TexCoord& tc ) const
    {
      return u == tc.u && v == tc.v;
    }

    bool operator != ( const TexCoord& tc ) const
    {
      return u != tc.u || v != tc.v;
    }

    operator const float* () const
    {
      return &u;
    }

    operator float* ()
    {
      return &u;
    }

    const float& operator [] ( int i ) const
    {
      assert( 0 <= i && i < 2 );

      return ( &u )[i];
    }

    float& operator [] ( int i )
    {
      assert( 0 <= i && i < 2 );

      return ( &u )[i];
    }
  };

}

#include "matrix/Sphere.hpp"
#include "matrix/Bounds.hpp"
#include "matrix/AABB.hpp"
