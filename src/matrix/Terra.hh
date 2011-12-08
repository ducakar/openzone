/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
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
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file matrix/Terra.hh
 *
 * Matrix structure for terrain.
 */

#pragma once

#include "matrix/common.hh"

namespace oz
{
namespace matrix
{

class Terra
{
  public:

    static const int QUADS = 256;
    static const int VERTS = QUADS + 1;

    // Orbis::DIM == Terrain::DIM == Terrain::MAX * TerraQuad::DIM
    static const float DIM;

    struct Quad
    {
      // integer size of a terrain quad
      static const int   SIZEI = 16;
      // float size of a terrain quad
      static const float SIZE;
      static const float INV_SIZE;
      // dimension of a terrain quad (size / 2)
      static const float DIM;

      Point3 vertex;
      Vec3   triNormal[2];
    };

    Quad quads[VERTS][VERTS];
    int  id;

    Terra();

    Span getInters( float minX, float minY, float maxX, float maxY, float epsilon = 0.0f ) const;
    // indices of TerraQuad and index of the triangle inside the TerraQuad
    Pair<int> getIndices( float x, float y ) const;
    float height( float x, float y ) const;

    void load( int id );
    void init();

    void read( InputStream* istream );
    void write( BufferStream* ostream ) const;

};

inline Span Terra::getInters( float minPosX, float minPosY,
                              float maxPosX, float maxPosY, float epsilon ) const
{
  return Span( max( int( ( minPosX - epsilon + DIM ) * Quad::INV_SIZE ), 0 ),
               max( int( ( minPosY - epsilon + DIM ) * Quad::INV_SIZE ), 0 ),
               min( int( ( maxPosX + epsilon + DIM ) * Quad::INV_SIZE ), QUADS - 1 ),
               min( int( ( maxPosY + epsilon + DIM ) * Quad::INV_SIZE ), QUADS - 1 ) );
}

inline Pair<int> Terra::getIndices( float x, float y ) const
{
  int ix = int( ( x + DIM ) * Quad::INV_SIZE );
  int iy = int( ( y + DIM ) * Quad::INV_SIZE );

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
