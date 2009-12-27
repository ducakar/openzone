/*
 *  matrix.h
 *
 *  Common matrix includes and definitions
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

namespace oz
{
  // ~1 mm, should be enough for 8 km x 8 km map (i guess)
  const float EPSILON   = 0.0009765625f;
  // EPSILON / 2
  const float EPSILON_2 = 0.00048828125f;

  struct Area
  {
    int minX;
    int maxX;
    int minY;
    int maxY;
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

#include "Bounds.h"
#include "AABB.h"
#include "Sphere.h"

#include "io.h"
