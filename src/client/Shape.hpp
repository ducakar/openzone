/*
 *  Shape.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

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

      static const int MAX_PARTS = 64;

      uint vao;
      uint ibo;
      uint vbo;

    public:

      void bindVertexArray() const;

      static void drawSprite( const Point3& p, float dimX, float dimY );
      static void drawBox( const AABB& bb );
      static void drawWireBox( const AABB& bb );
      static void draw( const Particle* part );

      void load();
      void unload();

  };

  extern Shape shape;

}
}
