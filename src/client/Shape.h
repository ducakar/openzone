/*
 *  Shape.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/bv.h"
#include "matrix/Particle.h"
#include "Context.h"

namespace oz
{
namespace client
{

  class Shape
  {
    private:

      uint partLists[64];
      uint spark;

    public:

      uint genRandomTetrahedicParticle( uint list, float size );
      uint genRandomCubicParticle( uint list, float size );
      uint genBox( uint list, const AABB &bb, uint texture );

      void drawBox( const AABB &bb );
      void draw( Particle *part );

      void init();
      void free();

  };

  extern Shape shape;

}
}
