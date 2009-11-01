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

      int  minX;
      int  minY;
      int  maxX;
      int  maxY;

      int  ix;
      int  iy;

      bool isLoaded;

      void init();
      void load( float height );
      void load( const char *heightMapFile );

      void isEmpty() const;

      void  getInters( float minX, float minY, float maxX, float maxY, float epsilon = 0.0f );
      // indices of TerraQuad and index of the triangle inside the TerraQuad
      void  getIndices( float x, float y );
      float height( float x, float y );

  };

  inline void Terrain::getInters( float minPosX, float minPosY, float maxPosX, float maxPosY,
                                  float epsilon )
  {
    minX = max( static_cast<int>( minPosX - epsilon + DIM ) / Quad::SIZEI, 0 );
    minY = max( static_cast<int>( minPosY - epsilon + DIM ) / Quad::SIZEI, 0 );
    maxX = min( static_cast<int>( maxPosX + epsilon + DIM ) / Quad::SIZEI, QUADS - 1 );
    maxY = min( static_cast<int>( maxPosY + epsilon + DIM ) / Quad::SIZEI, QUADS - 1 );
  }

  inline void Terrain::getIndices( float x, float y )
  {
    ix = static_cast<int>( x + DIM ) / Quad::SIZEI;
    iy = static_cast<int>( y + DIM ) / Quad::SIZEI;

    ix = bound( ix, 0, QUADS - 1 );
    iy = bound( iy, 0, QUADS - 1 );
  }

  inline float Terrain::height( float x, float y )
  {
    getIndices( x, y );

    float intraX = Math::mod( x + DIM, Quad::SIZE );
    float intraY = Math::mod( y + DIM, Quad::SIZE );
    int   ii = intraX <= intraY;

    return
        ( quads[ix][iy].tri[ii].distance - quads[ix][iy].tri[ii].normal.x * x -
        quads[ix][iy].tri[ii].normal.y * y ) / quads[ix][iy].tri[ii].normal.z;
  }

}
