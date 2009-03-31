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

    void setBounds( const Bounds &bounds )
    {
      switch( rot ) {
        case Structure::R0: {
          mins = bounds.mins + p;
          maxs = bounds.maxs + p;
          break;
        }
        case Structure::R90: {
          mins = Vec3( -bounds.maxs.y + p.x, bounds.mins.x + p.y, bounds.mins.z + p.z );
          maxs = Vec3( -bounds.mins.y + p.x, bounds.maxs.x + p.y, bounds.maxs.z + p.z );
          break;
        }
        case Structure::R180: {
          mins = Vec3( -bounds.maxs.x + p.x, -bounds.mins.x + p.y, bounds.mins.z + p.z );
          maxs = Vec3( -bounds.maxs.y + p.x, -bounds.maxs.y + p.y, bounds.maxs.z + p.z );
          break;
        }
        default:
        case Structure::R270: {
          mins = Vec3( bounds.mins.y + p.x, -bounds.maxs.x + p.y, bounds.mins.z + p.z );
          maxs = Vec3( bounds.maxs.y + p.x, -bounds.mins.x + p.y, bounds.maxs.z + p.z );
          break;
        }
      }
    }

    Structure() {}
    Structure( const Vec3 &p_, Rotation rot_, int bsp_ ) :
        p( p_ ), rot( rot_ ), bsp( bsp_ )
    {}
  };

}
