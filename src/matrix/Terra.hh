/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
namespace matrix
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

      Point vertex;
      Vec3  triNormal[2];
    };

    // Orbis::DIM == Terrain::DIM == Terrain::MAX * TerraQuad::DIM
    static const int QUADS = 2 * MAX_WORLD_COORD / Quad::SIZE;
    static const int VERTS = QUADS + 1;
    static const int DIM   = QUADS * Quad::DIM;

    Quad quads[VERTS][VERTS]; ///< Vertices and triangle normals.
    int  liquid;              ///< Either `matrix::Medium::GLOBAL_WATER_BIT` or
                              ///< `matrix::Medium::GLOBAL_LAVA_BIT`
    int  id;

    Span getInters( float minX, float minY, float maxX, float maxY, float epsilon = 0.0f ) const;
    Pair<int> getIndices( float x, float y ) const;
    float height( float x, float y ) const;

    void reset();
    void load( int id );
    void init();

    void read( InputStream* istream );
    void write( OutputStream* ostream ) const;

    void read( const JSON& json );
    JSON write() const;

};

inline Span Terra::getInters( float minPosX, float minPosY,
                              float maxPosX, float maxPosY, float epsilon ) const
{
  return {
    max( int( ( minPosX - epsilon + DIM ) / Quad::SIZE ), 0 ),
    max( int( ( minPosY - epsilon + DIM ) / Quad::SIZE ), 0 ),
    min( int( ( maxPosX + epsilon + DIM ) / Quad::SIZE ), QUADS - 1 ),
    min( int( ( maxPosY + epsilon + DIM ) / Quad::SIZE ), QUADS - 1 )
  };
}

inline Pair<int> Terra::getIndices( float x, float y ) const
{
  int ix = int( ( x + DIM ) / Quad::SIZE );
  int iy = int( ( y + DIM ) / Quad::SIZE );

  return { clamp( ix, 0, QUADS - 1 ), clamp( iy, 0, QUADS - 1 ) };
}

inline float Terra::height( float x, float y ) const
{
  Pair<int>   i    = getIndices( x, y );
  const Quad& quad = quads[i.x][i.y];

  float localX = x - quad.vertex.x;
  float localY = y - quad.vertex.y;
  float height = quad.vertex.z;
  int   ii     = localX <= localY;
  Vec3  normal = quad.triNormal[ii];

  return height - ( normal.x * localX + normal.y * localY ) / normal.z;
}

}
}
