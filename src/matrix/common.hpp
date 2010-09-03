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

}

#include "matrix/Sphere.hpp"
#include "matrix/Bounds.hpp"
#include "matrix/AABB.hpp"
