/*
 *  Shape.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "matrix/common.hpp"
#include "matrix/Particle.hpp"
#include "client/Context.hpp"

namespace oz
{
namespace client
{

  class Shape
  {
    private:

      static const int MAX_PART_LISTS = 64;

      uint partListBase;

    public:

      uint genRandomTetrahedicParticle( uint list, float size );
      uint genRandomCubicParticle( uint list, float size );
      uint genBox( uint list, const AABB& bb, uint texture );

      void drawBox( const AABB& bb );
      void draw( const Particle* part );

      void load();
      void unload();

  };

  extern Shape shape;

}
}
