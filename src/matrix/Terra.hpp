/*
 *  Terra.hpp
 *
 *  Matrix structure for terrain
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/common.hpp"

namespace oz
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
      void write( OutputStream* ostream ) const;

#ifdef OZ_TOOLS

      void buildTerraFrame();
      void prebuild( const char* name );

#endif

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

    return Pair<int>( clamp( ix, 0, QUADS - 1 ), clamp( iy, 0, QUADS - 1 ) );
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
