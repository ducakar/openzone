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
 * @file builder/Terra.hh
 */

#pragma once

#include <builder/common.hh>
#include <client/Terra.hh>

namespace oz::builder
{

class Terra
{
private:

  // Some "shortcuts".
  using Quad = oz::Terra::Quad;

  static constexpr int DIM        = oz::Terra::DIM;
  static constexpr int VERTS      = oz::Terra::VERTS;
  static constexpr int QUADS      = oz::Terra::QUADS;
  static constexpr int TILES      = client::Terra::TILES;
  static constexpr int TILE_QUADS = client::Terra::TILE_QUADS;

private:

  Quad   quads[VERTS][VERTS];

  String name;

  int    liquid;
  Vec4   liquidColour;

  String detailTex;
  String liquidTex;
  String map;

private:

  void load();
  void saveMatrix();
  void saveClient();

public:

  void build(const char* name);

};

extern Terra terra;

}
