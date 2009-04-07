/*
 *  Structure.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "bv.h"

namespace oz
{

  struct Structure : Bounds
  {
    enum Rotation
    {
      R0   = 0,
      R90  = 1,
      R180 = 2,
      R270 = 3
    };

    Vec3     p;
    Rotation rot;
    int      index;
    int      bsp;
    float    damage;

    explicit Structure() {}

    explicit Structure( const Vec3 &p_, Rotation rot_, int bsp_ ) :
        p( p_ ), rot( rot_ ), bsp( bsp_ )
    {}
  };

}
