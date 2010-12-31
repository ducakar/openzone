/*
 *  Terra.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

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

      struct VertexData
      {
        Vec3     position;
        Vec3     normal;
        TexCoord detailTexCoord;
        TexCoord mapTexCoord;
      };

      uint arrayBuffer;
      uint indexBuffer;

      uint detailTexId;
      uint mapTexId;
      uint waterTexId;

      bool inWater;

      void genBufferData( DArray<VertexData>* arrayData, DArray<uint>* indexData );
      void saveBufferData( const char* file, DArray<VertexData>* arrayData,
                           DArray<uint>* indexData );
      void loadBufferData( const char* file, DArray<VertexData>* arrayData,
                           DArray<uint>* indexData );

    public:

      float radius;

      void prebuild();
      void load();
      void unload();

      void draw() const;
      void drawWater() const;
  };

  extern Terra terra;

}
}
