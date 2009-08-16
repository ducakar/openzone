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

      struct TexCoord
      {
        float u;
        float v;
      };

      uint     detailTexId;
      uint     mapTexId;

      float    radius;

      Vec3     *normals;
      TexCoord *detailTexCoords;
      TexCoord *mapTexCoords;

    public:

      void init();
      void free();

      void setRadius( float radius );
      void draw() const;
  };

  inline void Terrain::setRadius( float radius_ )
  {
    radius = radius_;
  }

}
}
