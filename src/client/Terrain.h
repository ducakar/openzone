/*
 *  Terrain.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/bv.h"

namespace oz
{
namespace client
{

  class Terrain
  {
    private:

      static const float DETAILTEX_SCALE;

      uint detailTexId;
      uint mapTexId;

      Vec3  *normals;
      float ( *texCoords )[2];
      Vec3  *vertices;

    public:

      void init();
      void free();

      void draw() const;
  };

}
}
