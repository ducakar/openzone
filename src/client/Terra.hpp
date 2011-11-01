/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file client/Terra.hpp
 */

#pragma once

#include "matrix/Terra.hpp"

#include "client/Mesh.hpp"

namespace oz
{
namespace client
{

class Terra
{
  public:

    static const int   TILE_QUADS = 16;
    static const int   TILES      = matrix::Terra::QUADS / TILE_QUADS;

  private:

    static const int   TILE_INDICES;
    static const int   TILE_VERTICES;

    static const float TILE_SIZE;
    static const float TILE_INV_SIZE;

    static const float WAVE_BIAS_INC;

    uint  vaos[TILES][TILES];
    uint  vbos[TILES][TILES];
    uint  ibo;

    uint  waterTexId;
    uint  detailTexId;
    uint  mapTexId;

    int   landShaderId;
    int   waterShaderId;

    float waveBias;

    Span span;
    SBitset<TILES * TILES> waterTiles;

  public:

    int id;

    Terra();

    void draw();
    void drawWater();

    void load();
    void unload();

};

extern Terra terra;

}
}
