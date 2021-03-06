/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2019 Davorin Učakar
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

#include <client/Model.hh>

namespace oz::client
{

class Terra
{
public:

  static constexpr int   TILE_QUADS    = 32;
  static constexpr int   TILES         = oz::Terra::QUADS / TILE_QUADS;

private:

  static constexpr int   TILE_SIZE     = TILE_QUADS * oz::Terra::Quad::SIZE;
  static constexpr int   TILE_INDICES  = TILE_QUADS * (TILE_QUADS + 1) * 2 + (TILE_QUADS - 1) * 2;
  static constexpr int   TILE_VERTICES = (TILE_QUADS + 1) * (TILE_QUADS + 1);

  static constexpr float WAVE_BIAS_INC = 1.5f;

  uint                   vbos[TILES][TILES] = {};
  uint                   ibo                = 0;

  int                    detailTexId;
  int                    landShaderId;
  int                    liquidShaderId;

  Texture                detailTex;
  Texture                liquidTex;
  GLuint                 mapTex;

  float                  waveBias;

  Span                   span;
  SBitset<TILES * TILES> liquidTiles;

public:

  int  id = -1;

  int  liquidTexId;
  Vec4 liquidFogColour;

  void draw();
  void drawLiquid();

  void load();
  void unload();

};

extern Terra terra;

}
