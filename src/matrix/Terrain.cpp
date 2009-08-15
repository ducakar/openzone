/*
 *  Terrain.cpp
 *
 *  Matrix structure for terrain
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Terrain.h"

#include <SDL_image.h>

namespace oz
{

  const float TerraQuad::SIZE = (float) TerraQuad::SIZEI;
  const float TerraQuad::DIM = TerraQuad::SIZE / 2.0f;

  const float Terrain::DIM = TerraQuad::DIM * Terrain::MAX;
  const float Terrain::HEIGHT_STEP = 0.5f;
  const float Terrain::HEIGHT_BIAS = 0.0f;

  void Terrain::buildTerraFrame()
  {
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
          (0,0)
        */
        Vec3 a(   x       * TerraQuad::SIZE - DIM,
                  y       * TerraQuad::SIZE - DIM,
                heightMap[x    ][y    ] );
        Vec3 b( ( x + 1 ) * TerraQuad::SIZE - DIM,
                  y       * TerraQuad::SIZE - DIM,
                heightMap[x + 1][y    ] );
        Vec3 c( ( x + 1 ) * TerraQuad::SIZE - DIM,
                ( y + 1 ) * TerraQuad::SIZE - DIM,
                heightMap[x + 1][y + 1] );
        Vec3 d(   x       * TerraQuad::SIZE - DIM,
                ( y + 1 ) * TerraQuad::SIZE - DIM,
                heightMap[x    ][y + 1] );

        terra[x][y].normal[0] = ( ( a - d ) ^ ( c - d ) ).norm();
        terra[x][y].distance[0] = terra[x][y].normal[0] * a;

        terra[x][y].normal[1] = ( ( c - b ) ^ ( a - b ) ).norm();
        terra[x][y].distance[1] = terra[x][y].normal[1] * a;
      }
    }
  }

  void Terrain::level( float height )
  {
    for( int y = 0; y <= MAX; y++ ) {
      for( int x = 0; x <= MAX; x++ ) {
        heightMap[x][y] = height;
      }
    }

    buildTerraFrame();
  }

  void Terrain::load( const char *heightMapFile )
  {
    log.print( "Loading terrain heightmap image '%s' ...", heightMapFile );

    SDL_Surface *image = IMG_Load( heightMapFile );

    if( image == null ) {
      log.printEnd( " No such file" );
      throw Exception( 0, "Failed to load terrain" );
    }
    if( image->w != MAX + 1 || image->h != MAX + 1 || image->format->BytesPerPixel != 1 ) {
      log.println( "Invalid size: %d x %d, should be %d x %d",
                   image->w, image->h, MAX + 1, MAX + 1 );
      SDL_FreeSurface( image );
      throw Exception( 0, "Failed to load terrain" );
    }

    int scanLineLength = image->pitch;
    const ubyte *p = (const ubyte*) image->pixels;
    for( int y = MAX; y >= 0; y-- ) {
      for( int x = 0; x <= MAX; x++ ) {
        heightMap[x][y] = (float) p[x] * HEIGHT_STEP + HEIGHT_BIAS;
      }
      p += scanLineLength;
    }

    buildTerraFrame();

    SDL_FreeSurface( image );
    log.printEnd( " OK" );
  }

}
