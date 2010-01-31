/*
 *  Terrain.cpp
 *
 *  Matrix structure for terrain
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "precompiled.h"

#include "Terrain.h"

#include <SDL_image.h>

namespace oz
{

  const float Terrain::Quad::SIZE     = float( Terrain::Quad::SIZEI );
  const float Terrain::Quad::INV_SIZE = 1.0f / float( Terrain::Quad::SIZEI );
  const float Terrain::Quad::DIM      = Terrain::Quad::SIZE / 2.0f;

  const float Terrain::DIM         = Terrain::Quad::DIM * Terrain::QUADS;
  const float Terrain::HEIGHT_STEP = 0.5f;
  const float Terrain::HEIGHT_BIAS = -40.25f;

  void Terrain::buildTerraFrame()
  {
    for( int x = 0; x < QUADS; ++x ) {
      for( int y = 0; y < QUADS; ++y ) {
        /*
          0. triangle -- upper left
          1. triangle -- lower right

            |  ...  |         D        C
            +---+---+-         --o--->o
            |1 /|1 /|          |      ^
            | / | / |          |      |
            |/ 0|/ 0|          |      |
            +---+---+- ...     v      |
            |1 /|1 /|          o<-----o
            | / | / |         A        B
            |/ 0|/ 0|
          (0,0)
        */
        const Vec3& a = vertices[x    ][y    ];
        const Vec3& b = vertices[x + 1][y    ];
        const Vec3& c = vertices[x + 1][y + 1];
        const Vec3& d = vertices[x    ][y + 1];

        quads[x][y].tri[0].normal   = ( ( c - b ) ^ ( a - b ) ).norm();
        quads[x][y].tri[0].distance = quads[x][y].tri[0].normal * a;

        quads[x][y].tri[1].normal   = ( ( a - d ) ^ ( c - d ) ).norm();
        quads[x][y].tri[1].distance = quads[x][y].tri[1].normal * a;
      }
    }
  }

  void Terrain::init()
  {
    for( int x = 0; x < MAX; ++x ) {
      for( int y = 0; y < MAX; ++y ) {
        vertices[x][y].x = float( x * Quad::SIZEI ) - DIM;
        vertices[x][y].y = float( y * Quad::SIZEI ) - DIM;
      }
    }
  }

  void Terrain::load( float height )
  {
    for( int x = 0; x < MAX; ++x ) {
      for( int y = 0; y < MAX; ++y ) {
        vertices[x][y].x = float( x * Quad::SIZEI ) - DIM;
        vertices[x][y].y = float( y * Quad::SIZEI ) - DIM;
        vertices[x][y].z = height;
      }
    }

    buildTerraFrame();
  }

  void Terrain::load( const char* verticesFile )
  {
    log.print( "Loading terrain heightmap image '%s' ...", verticesFile );

    SDL_Surface* image = IMG_Load( verticesFile );

    if( image == null ) {
      log.printEnd( " No such file" );
      throw Exception( "Failed to load terrain" );
    }
    if( image->w != MAX || image->h != MAX || image->format->BytesPerPixel != 1 ) {
      log.println( "Invalid size: %d x %d, should be %d x %d", image->w, image->h, MAX, MAX );
      SDL_FreeSurface( image );
      throw Exception( "Failed to load terrain" );
    }

    int scanLineLength = image->pitch;
    const ubyte* line = reinterpret_cast<const ubyte*>( image->pixels );
    for( int y = MAX - 1; y >= 0; --y ) {
      for( int x = 0; x < MAX; ++x ) {
        vertices[x][y].z = float( line[x] ) * HEIGHT_STEP + HEIGHT_BIAS;
      }
      line += scanLineLength;
    }

    buildTerraFrame();

    SDL_FreeSurface( image );
    log.printEnd( " OK" );
  }

}
