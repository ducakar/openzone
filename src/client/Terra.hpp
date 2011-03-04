/*
 *  Terra.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Terra.hpp"

#include "client/Mesh.hpp"

namespace oz
{
namespace client
{

  class Terra
  {
    private:

      static const int   TILE_QUADS   = 16;
      static const int   TILES        = oz::Terra::QUADS / TILE_QUADS;

      static const int   TILE_INDICES;
      static const int   TILE_VERTICES;

      static const float TILE_SIZE;
      static const float TILE_INV_SIZE;

      static const float DETAIL_SCALE;
      static const float WATER_SCALE;

      uint vaos[TILES][TILES];
      uint vbos[TILES][TILES];
      uint ibo;

      uint waterTexId;
      uint detailTexId;
      uint mapTexId;

      Span span;
      SBitset<TILES * TILES> waterTiles;

    public:

      void prebuild( const char* name );
      void load();
      void unload();

      void draw();
      void drawWater();
  };

  extern Terra terra;

}
}
