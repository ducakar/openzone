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

      static void fill( float x, float y, float width, float height );
      static void fill( int x, int y, int width, int height );
      static void rect( float x, float y, float width, float height );
      static void rect( int x, int y, int width, int height );
      static void tag( float minX, float minY, float maxX, float maxY );

      static void quad( float dimX, float dimY );
      static void box( const AABB& bb );
      static void wireBox( const AABB& bb );
      static void draw( const Particle* part );

      void load();
      void unload();

  };

  extern Shape shape;

}
}
