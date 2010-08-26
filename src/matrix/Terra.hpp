/*
 *  Terra.hpp
 *
 *  Matrix structure for terrain
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "matrix/common.hpp"

namespace oz
{

  namespace client
  {
    class Terra;
  }

  class Terra
  {
    friend class client::Terra;

    public:

      static const int QUADS = 256;
      static const int MAX   = QUADS + 1;

    private:

      // Orbis::DIM == Terrain::DIM == Terrain::MAX * TerraQuad::DIM
      static const float DIM;

#ifndef OZ_USE_PREBUILT
      void buildTerraFrame();
#endif

    public:

      struct Triangle
      {
        Vec3  normal;
        float distance;
      };

      struct Quad
      {
        // integer size of a terrain quad
        static const int   SIZEI = 8;
        // float size of a terrain quad
        static const float SIZE;
        static const float INV_SIZE;
        // dimension of a terrain quad (size / 2)
        static const float DIM;

        Triangle tri[2];
      };

      Vec3 vertices[MAX][MAX];
      Quad quads[QUADS][QUADS];

      String detailTexture;
      String mapTexture;
      String waterTexture;

      void init();
#ifndef OZ_USE_PREBUILT
      void load( float height );
#endif
      void load( const char* name );
#ifndef OZ_USE_PREBUILT
      void save( const char* name );
#endif

      void isEmpty() const;

      Span getInters( float minX, float minY, float maxX, float maxY, float epsilon = 0.0f ) const;
      // indices of TerraQuad and index of the triangle inside the TerraQuad
      Pair<int> getIndices( float x, float y ) const;
      float height( float x, float y ) const;

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

    return Pair<int>( bound( ix, 0, QUADS - 1 ), bound( iy, 0, QUADS - 1 ) );
  }

  inline float Terra::height( float x, float y ) const
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
