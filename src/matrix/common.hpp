/*
 *  common.hpp
 *
 *  Common matrix includes and definitions
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

namespace oz
{
  // ~1 mm, should be enough for 8 km x 8 km map (I guess)
  const float EPSILON = 0.0009765625f;

  struct Span
  {
    int minX;
    int minY;
    int maxX;
    int maxY;

    Span()
    {}

    Span( int minX_, int minY_, int maxX_, int maxY_ ) :
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

}

#include "matrix/Sphere.hpp"
#include "matrix/Bounds.hpp"
#include "matrix/AABB.hpp"

#include "matrix/io.hpp"
