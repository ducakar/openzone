/*
 *  Terrain.hpp
 *
 *  Matrix structure for terrain
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  struct TerraQuad
  {
    static const int SIZEI = 16;
    static const float SIZE;
    static const float DIM;
    static const float STEP;

    Vec3  normal[2];
    float distance[2];
  };

  class Terrain
  {
    protected:

      void      buildTerraFrame();

    public:

      static const int MAX = 128;
      // World::DIM == Terrain::DIM == Terrain::MAX * TerraQuad::DIM
      static const float DIM;
      static const float HEIGHT_BIAS;

      int ix;
      int iy;
      int ii;

      float     heightMap[MAX + 1][MAX + 1];
      TerraQuad terra[MAX][MAX];

      void  load( ubyte *data );
      void  loadRAW( const char *heightMapFile );
      void  loadIMG( const char *heightMapFile );

      void  getIndices( float x, float y );

      float height( float x, float y );
  };

}
