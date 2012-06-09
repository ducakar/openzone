/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file client/Terra.hh
 */

#pragma once

#include "client/Mesh.hh"

namespace oz
{
namespace client
{

class Terra
{
  public:

    static const int   TILE_QUADS    = 32;
    static const int   TILES         = matrix::Terra::QUADS / TILE_QUADS;

  private:

    static const int   TILE_SIZE     = TILE_QUADS * matrix::Terra::Quad::SIZE;
    static const int   TILE_INDICES  = TILE_QUADS * ( TILE_QUADS + 1 ) * 2 + ( TILE_QUADS - 1 ) * 2;
    static const int   TILE_VERTICES = ( TILE_QUADS + 1 ) * ( TILE_QUADS + 1 );

    static const float WAVE_BIAS_INC;

    uint  vaos[TILES][TILES];
    uint  vbos[TILES][TILES];
    uint  ibo;

    int   landShaderId;
    int   waterShaderId;

    float waveBias;

    Span  span;
    SBitset<TILES * TILES> waterTiles;

  public:

    int  id;

    uint waterTexId;
    uint detailTexId;
    uint mapTexId;

    Vec4 liquidFogColour;

    Terra();

    void draw();
    void drawWater();

    void load();
    void unload();

};

extern Terra terra;

}
}
