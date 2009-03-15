/*
 *  Terrain.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/bv.hpp"

#define TERRA_DEPTH            2
#define TERRA_DETAILTEX_SCALE  4
#define TERRA_GROUNDTEX_SCALE  1

namespace oz
{
namespace Client
{

  struct Quadtree : Sphere
  {
    union
    {
      Quadtree *next[4];
      uint     list;
    };

    ~Quadtree();
  };

  class Terrain
  {
    private:

      uint tTerraDetail;
      uint tTerraMap;

      Quadtree *qTerra;

      uint makeQuad( int minX, int minY, int maxX, int maxY, float *minHeight, float *maxHeight );
      void buildQuadtree( Quadtree *qTree, int minX, int minY, int maxX, int maxY,
                          float *minHeight, float *maxHeight );
      void drawQuadtree( const Quadtree *qTree );

    public:

      ~Terrain();

      void init();
      void free();

      void draw();
  };

}
}
