/*
 *  Terra.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

#include "matrix/common.hpp"

namespace oz
{
namespace client
{

  class Terra
  {
    private:

      static const float DETAIL_SCALE;
      static const float WATER_SCALE;

      struct TexCoord
      {
        float u;
        float v;
      };

      struct VertexData
      {
        Vec3     position;
        Vec3     normal;
        TexCoord detailTexCoord;
        TexCoord mapTexCoord;
      };

      uint      arrayBuffer;
      uint      indexBuffer;

      uint      detailTexId;
      uint      mapTexId;
      uint      waterTexId;

      float     radius;

      bool      inWater;

    public:

      void load();
      void unload();

      void setRadius( float radius );
      void draw() const;
      void drawWater() const;
  };

  inline void Terra::setRadius( float radius_ )
  {
    radius = radius_;
  }

  extern Terra terra;

}
}
