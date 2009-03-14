/*
 *  Shape.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/bv.hpp"
#include "Context.hpp"

namespace oz
{
namespace Client
{

  struct Shape
  {
    uint spark;

    uint genRandomTetrahedicParticle( uint list, float size );
    uint genRandomCubicParticle( uint list, float size );
    uint genBox( uint list, const AABB &bb, uint texture );
    void drawBox( const AABB &bb );

    void init();
    void free();
  };

  extern Shape shape;

}
}
