/*
 *  Structure.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "bv.hpp"

namespace oz
{

  struct Structure : Bounds
  {
    Vec3 p;

    int  index;
    int  bsp;

    float damage;

    Structure();

    Structure( const Vec3 &p, int bsp );
  };

}
