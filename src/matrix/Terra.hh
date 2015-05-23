/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file matrix/Terra.hh
 *
 * Matrix structure for terrain.
 */

#pragma once

#include <matrix/common.hh>

namespace oz
{

class Terra
{
public:

  //
  // 0. triangle -- upper left
  // 1. triangle -- lower right
  //
  //    |  ...  |         D        C
  //    +---+---+-         o----->o
  //    |1 /|1 /|          |      ^
  //    | / | / |          |      |
  //    |/ 0|/ 0|          |      |
  //    +---+---+- ...     v      |
  //    |1 /|1 /|          o<-----o
  //    | / | / |         A        B
  //    |/ 0|/ 0|
  //  (0,0)
  //
  struct Quad
  {
    static const int SIZE = 8;        ///< Integer size of a terrain quad.
    static const int DIM  = SIZE / 2; ///< Dimension of a terrain quad (size / 2).

    Point vertex;                     ///< Upper-left vertex.
    Vec3  normals[2];                 ///< [0] upper-left and [1] lower-right triangle normal.
  };

  // Orbis::DIM == Terrain::DIM == Terrain::MAX * TerraQuad::DIM
  static const int QUADS = 2 * MAX_WORLD_COORD / Quad::SIZE;
  static const int VERTS = QUADS + 1;
  static const int DIM   = QUADS * Quad::DIM;

  Quad quads[VERTS][VERTS]; ///< Vertices and triangle normals.
  int  liquid;              ///< Either `matrix::Medium::GLOBAL_WATER_BIT` or
                            ///< `matrix::Medium::GLOBAL_LAVA_BIT`.
  int  id;

  Span getInters(float minX, float minY, float maxX, float maxY, float epsilon = 0.0f) const
  {
    return {
      max(int((minX - epsilon + DIM) / Quad::SIZE), 0),
      max(int((minY - epsilon + DIM) / Quad::SIZE), 0),
      min(int((maxX + epsilon + DIM) / Quad::SIZE), QUADS - 1),
      min(int((maxY + epsilon + DIM) / Quad::SIZE), QUADS - 1)
    };
  }

  Pos2 getIndices(float x, float y) const
  {
    int ix = int((x + DIM) / Quad::SIZE);
    int iy = int((y + DIM) / Quad::SIZE);

    return { clamp(ix, 0, QUADS - 1), clamp(iy, 0, QUADS - 1) };
  }

  float getHeight(float x, float y) const
  {
    Pos2        pos  = getIndices(x, y);
    const Quad& quad = quads[pos.x][pos.y];

    float localX = x - quad.vertex.x;
    float localY = y - quad.vertex.y;
    float height = quad.vertex.z;
    int   ii     = localX <= localY;
    Vec3  normal = quad.normals[ii];

    return height - (normal.x * localX + normal.y * localY) / normal.z;
  }

  void reset();
  void load(int id);
  void init();

  void read(const Json& json);
  void read(Stream* is);

  Json write() const;
  void write(Stream* os) const;

};

}
