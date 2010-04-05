/*
 *  Terrain.h
 *
 *  Matrix structure for terrain
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "matrix/common.h"

namespace oz
{

  namespace client
  {
    class Terrain;
  }

  class Terrain
  {
    friend class client::Terrain;

    public:

      static const int QUADS = 128;
      static const int MAX   = QUADS + 1;

    private:

      // World::DIM == Terrain::DIM == Terrain::MAX * TerraQuad::DIM
      static const float DIM;

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
        static const float INV_SIZE;
        // dimension of a terrain quad (size / 2)
        static const float DIM;

        Triangle tri[2];
      };

      Vec3 vertices[MAX][MAX];
      Quad quads[QUADS][QUADS];

      void init();
      void load( float height );
      void load( const char* heightMapFile, float heightStep, float heightBias );

      void isEmpty() const;

      void  getInters( Span& span, float minX, float minY, float maxX, float maxY,
                       float epsilon = 0.0f ) const;
      // indices of TerraQuad and index of the triangle inside the TerraQuad
      Pair<int> getIndices( float x, float y ) const;
      float height( float x, float y ) const;

  };

  inline void Terrain::getInters( Span& span, float minPosX, float minPosY,
                                  float maxPosX, float maxPosY, float epsilon ) const
  {
    span.minX = max( int( ( minPosX - epsilon + DIM ) * Quad::INV_SIZE ), 0 );
    span.minY = max( int( ( minPosY - epsilon + DIM ) * Quad::INV_SIZE ), 0 );
    span.maxX = min( int( ( maxPosX + epsilon + DIM ) * Quad::INV_SIZE ), QUADS - 1 );
    span.maxY = min( int( ( maxPosY + epsilon + DIM ) * Quad::INV_SIZE ), QUADS - 1 );
  }

  inline Pair<int> Terrain::getIndices( float x, float y ) const
  {
    int ix = int( ( x + DIM ) * Quad::INV_SIZE );
    int iy = int( ( y + DIM ) * Quad::INV_SIZE );

    return Pair<int>( bound( ix, 0, QUADS - 1 ), bound( iy, 0, QUADS - 1 ) );
  }

  inline float Terrain::height( float x, float y ) const
  {
    Pair<int> i = getIndices( x, y );

    float intraX = Math::mod( x + DIM, Quad::SIZE );
    float intraY = Math::mod( y + DIM, Quad::SIZE );
    int   ii = intraX <= intraY;

    return
        ( quads[i.x][i.y].tri[ii].distance - quads[i.x][i.y].tri[ii].normal.x * x -
            quads[i.x][i.y].tri[ii].normal.y * y ) / quads[i.x][i.y].tri[ii].normal.z;
  }

}
