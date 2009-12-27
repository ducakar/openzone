/*
 *  Terrain.h
 *
 *  Matrix structure for terrain
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "matrix.h"

namespace oz
{

  namespace client
  {
    class Terrain;
  };

  class Terrain
  {
    friend class client::Terrain;

    public:

      static const int QUADS = 128;
      static const int MAX   = QUADS + 1;

    private:

      // World::DIM == Terrain::DIM == Terrain::MAX * TerraQuad::DIM
      static const float DIM;
      // height step for 1 unit in integer format, read from image
      static const float HEIGHT_STEP;
      static const float HEIGHT_BIAS;

      void buildTerraFrame();

    public:

      struct Triangle
      {
        Vec3  normal;
        float distance;
      };

      struct Quad
      {
        // integer size of a terrain quad
        static const int   SIZEI = 16;
        // float size of a terrain quad
        static const float SIZE;
        // dimension of a terrain quad (size / 2)
        static const float DIM;

        Triangle tri[2];
      };

      Vec3 vertices[MAX][MAX];
      Quad quads[QUADS][QUADS];

      void init();
      void load( float height );
      void load( const char *heightMapFile );

      void isEmpty() const;

      void  getInters( Area &area, float minX, float minY, float maxX, float maxY,
                       float epsilon = 0.0f ) const;
      // indices of TerraQuad and index of the triangle inside the TerraQuad
      void  getIndices( Area &area, float x, float y ) const;
      float height( float x, float y ) const;

  };

  inline void Terrain::getInters( Area &area, float minPosX, float minPosY,
                                  float maxPosX, float maxPosY, float epsilon ) const
  {
    area.minX = max( static_cast<int>( minPosX - epsilon + DIM ) / Quad::SIZEI, 0 );
    area.minY = max( static_cast<int>( minPosY - epsilon + DIM ) / Quad::SIZEI, 0 );
    area.maxX = min( static_cast<int>( maxPosX + epsilon + DIM ) / Quad::SIZEI, QUADS - 1 );
    area.maxY = min( static_cast<int>( maxPosY + epsilon + DIM ) / Quad::SIZEI, QUADS - 1 );
  }

  inline void Terrain::getIndices( Area &area, float x, float y ) const
  {
    area.minX = static_cast<int>( x + DIM ) / Quad::SIZEI;
    area.minY = static_cast<int>( y + DIM ) / Quad::SIZEI;

    area.minX = bound( area.minX, 0, QUADS - 1 );
    area.minY = bound( area.minY, 0, QUADS - 1 );
  }

  inline float Terrain::height( float x, float y ) const
  {
    Area area;
    getIndices( area, x, y );

    float intraX = Math::mod( x + DIM, Quad::SIZE );
    float intraY = Math::mod( y + DIM, Quad::SIZE );
    int   ii = intraX <= intraY;

    return
        ( quads[area.minX][area.minY].tri[ii].distance -
            quads[area.minX][area.minY].tri[ii].normal.x * x -
            quads[area.minX][area.minY].tri[ii].normal.y * y ) /
            quads[area.minX][area.minY].tri[ii].normal.z;
  }

}
