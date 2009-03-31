/*
 *  Terrain.cpp
 *
 *  Matrix structure for terrain
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Terrain.h"

namespace oz
{

  const float TerraQuad::SIZE = (float) TerraQuad::SIZEI;
  const float TerraQuad::DIM = TerraQuad::SIZE / 2.0f;
  const float TerraQuad::STEP = 0.5f;

  const float Terrain::DIM = TerraQuad::DIM * Terrain::MAX;
  const float Terrain::HEIGHT_BIAS = 0.0f;

  void Terrain::buildTerraFrame() {
    for( int x = 0; x < MAX; x++ ) {
      for( int y = 0; y < MAX; y++ ) {
        /*
          0. triangle -- leftmost
          1. triangle -- rightmost
          beginning:
            |  ...  |         D        C
            +---+---+-         o----->o
            |0 /|0 /|          |      ^
            | / | / |          |      |
            |/ 1|/ 1|          |      |
            +---+---+- ...     v      |
            |0 /|0 /|          o<-----o
            | / | / |         A        B
            |/ 1|/ 1|
          ( 0,0 )
        */
        Vec3 a(   x      * TerraQuad::SIZE - Terrain::DIM,
                  y      * TerraQuad::SIZE - Terrain::DIM,
                heightMap[x    ][y    ] );
        Vec3 b( ( x + 1 ) * TerraQuad::SIZE - Terrain::DIM,
                  y      * TerraQuad::SIZE - Terrain::DIM,
                heightMap[x + 1][y    ] );
        Vec3 c( ( x + 1 ) * TerraQuad::SIZE - Terrain::DIM,
                ( y + 1 ) * TerraQuad::SIZE - Terrain::DIM,
                heightMap[x + 1][y + 1] );
        Vec3 d(   x      * TerraQuad::SIZE - Terrain::DIM,
                ( y + 1 ) * TerraQuad::SIZE - Terrain::DIM,
                heightMap[x    ][y + 1] );

        terra[x][y].normal[0] = ( ( a - d ) ^ ( c - d ) ).norm();
        terra[x][y].distance[0] = terra[x][y].normal[0] * a;

        terra[x][y].normal[1] = ( ( c - b ) ^ ( a - b ) ).norm();
        terra[x][y].distance[1] = terra[x][y].normal[1] * a;
      }
    }
  }

  void Terrain::load( ubyte *data ) {
    for( int x = 0; x < Terrain::MAX; x++ ) {
      for( int y = 0; y < Terrain::MAX; y++ ) {
        heightMap[x][y] = (float) data[y * ( Terrain::MAX + 0 ) + x] * TerraQuad::STEP + Terrain::HEIGHT_BIAS;
      }
      heightMap[x][Terrain::MAX] = heightMap[x][Terrain::MAX - 1];
    }
    for( int y = 0; y <= Terrain::MAX; y++ ) {
      heightMap[Terrain::MAX][y] = heightMap[Terrain::MAX - 1][y];
    }

    buildTerraFrame();
  }

  void Terrain::loadRAW( const char *heightMapFile ) {
    logFile.print( "Loading raw terrain heightmap '%s' ...", heightMapFile );

    ubyte data[ ( Terrain::MAX + 1 ) * ( Terrain::MAX + 1 ) ];
    FILE *f = fopen( heightMapFile, "rb" );

    if( f == null ) {
      logFile.printEnd( " No such file" );
      return;
    }

    fread( data, Terrain::MAX + 1, Terrain::MAX + 1, f );
    load( data );
    fclose( f );

    logFile.printEnd( " OK" );
  }

  void Terrain::loadIMG( const char *heightMapFile ) {
    logFile.print( "Loading terrain heightmap image '%s' ...", heightMapFile );

    SDL_Surface *image = IMG_Load( heightMapFile );

    if( image == null ) {
      logFile.printEnd( " No such file" );
      return;
    }

    load( (ubyte*) image->pixels );

    SDL_FreeSurface( image );

    logFile.printEnd( " OK" );
  }

  void Terrain::getIndices( float x, float y ) {
    ix = ( int )( x + Terrain::DIM ) / TerraQuad::SIZEI;
    iy = ( int )( y + Terrain::DIM ) / TerraQuad::SIZEI;

    ix = bound( ix, 0, Terrain::MAX - 1 );
    iy = bound( iy, 0, Terrain::MAX - 1 );
  }

  float Terrain::height( float x, float y ) {
    getIndices( x, y );

    float intraX = Math::mod( x + Terrain::DIM, TerraQuad::SIZE );
    float intraY = Math::mod( y + Terrain::DIM, TerraQuad::SIZE );

    ii = intraX > intraY;

    return
      ( terra[ix][iy].distance[ii] - terra[ix][iy].normal[ii].x * x -
        terra[ix][iy].normal[ii].y * y ) / terra[ix][iy].normal[ii].z;
  }

}
