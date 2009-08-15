/*
 *  Terrain.h
 *
 *  Matrix structure for terrain
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  namespace client
  {
    class Terrain;
  };

  struct TerraQuad
  {
    // integer size of terrain quad
    static const int   SIZEI = 16;
    // float size of terrain quad
    static const float SIZE;
    // dimension of terrain quad (size / 2)
    static const float DIM;

    Vec3  normal[2];
    float distance[2];
  };

  class Terrain
  {
    friend class client::Terrain;

    public:

      static const int MAX = 256;

    private:

      // World::DIM == Terrain::DIM == Terrain::MAX * TerraQuad::DIM
      static const float DIM;
      // height step for 1 unit in integer format, read from image
      static const float HEIGHT_STEP;
      static const float HEIGHT_BIAS;

      float heightMap[MAX + 1][MAX + 1];
      bool  isLoaded;

      void  buildTerraFrame();

    public:

      TerraQuad terra[MAX][MAX];

      int   ix;
      int   iy;
      int   ii;

      int   minX;
      int   minY;
      int   maxX;
      int   maxY;

      void  level( float height );
      void  load( const char *heightMapFile );

      void  isEmpty() const;

      // indices of TerraQuad and index of the triangle inside the TerraQuad
      void  getIndices( float x, float y );
      void  getInters( float minX, float minY, float maxX, float maxY, float epsilon = 0.0f );
      float height( float x, float y );

  };

  inline void Terrain::getIndices( float x, float y )
  {
    ix = (int) ( x + Terrain::DIM ) / TerraQuad::SIZEI;
    iy = (int) ( y + Terrain::DIM ) / TerraQuad::SIZEI;

    ix = bound( ix, 0, Terrain::MAX - 1 );
    iy = bound( iy, 0, Terrain::MAX - 1 );
  }

  inline void Terrain::getInters( float minPosX, float minPosY, float maxPosX, float maxPosY,
                                  float epsilon )
  {
    minX = max( (int) ( minPosX + Terrain::DIM - epsilon ) / TerraQuad::SIZEI, 0 );
    minY = max( (int) ( minPosY + Terrain::DIM - epsilon ) / TerraQuad::SIZEI, 0 );
    maxX = min( (int) ( maxPosX + Terrain::DIM + epsilon ) / TerraQuad::SIZEI, Terrain::MAX - 1 );
    maxY = min( (int) ( maxPosY + Terrain::DIM + epsilon ) / TerraQuad::SIZEI, Terrain::MAX - 1 );
  }

  inline float Terrain::height( float x, float y )
  {
    getIndices( x, y );

    float intraX = Math::mod( x + Terrain::DIM, TerraQuad::SIZE );
    float intraY = Math::mod( y + Terrain::DIM, TerraQuad::SIZE );

    ii = intraX > intraY;

    return
        ( terra[ix][iy].distance[ii] - terra[ix][iy].normal[ii].x * x -
        terra[ix][iy].normal[ii].y * y ) / terra[ix][iy].normal[ii].z;
  }

}
