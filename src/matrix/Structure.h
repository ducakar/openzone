/*
 *  Structure.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "bv.h"
#include "io.h"

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
    int      index;
    int      bsp;
    Rotation rot;
    float    damage;

    explicit Structure() {}

    explicit Structure( const Vec3 &p_, int bsp_, Rotation rot_ ) :
        p( p_ ), bsp( bsp_ ), rot( rot_ )
    {}

    void readFull( InputStream *istream );
    void writeFull( OutputStream *ostream );
  };

}
