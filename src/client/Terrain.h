/*
 *  Terrain.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "matrix/matrix.h"

namespace oz
{
namespace client
{

  class Terrain
  {
    private:

      static const float DETAIL_SCALE;
      static const float WATER_SCALE;

      struct TexCoord
      {
        float u;
        float v;
      };

      uint      detailTexId;
      uint      mapTexId;
      uint      waterTexId;

      float     radius;

      bool      inWater;

      Vec3*     normals;
      TexCoord* detailTexCoords;
      TexCoord* mapTexCoords;

    public:

      void load();
      void unload();

      void setRadius( float radius );
      void draw() const;
      void drawWater() const;
  };

  inline void Terrain::setRadius( float radius_ )
  {
    radius = radius_;
  }

}
}
