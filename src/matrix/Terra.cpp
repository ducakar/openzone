/*
 *  Terra.cpp
 *
 *  Matrix structure for terrain
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Terra.hpp"

#include "matrix/Translator.hpp"

#include <SDL_image.h>

namespace oz
{

  const float Terra::Quad::SIZE     = float( Terra::Quad::SIZEI );
  const float Terra::Quad::INV_SIZE = 1.0f / float( Terra::Quad::SIZEI );
  const float Terra::Quad::DIM      = Terra::Quad::SIZE / 2.0f;

  const float Terra::DIM            = Terra::Quad::DIM * Terra::QUADS;

  void Terra::buildTerraFrame()
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
        const Point3& a = quads[x    ][y    ].vertex;
        const Point3& b = quads[x + 1][y    ].vertex;
        const Point3& c = quads[x + 1][y + 1].vertex;
        const Point3& d = quads[x    ][y + 1].vertex;

        quads[x][y].triNormal[0] = ~( ( c - b ) ^ ( a - b ) );
        quads[x][y].triNormal[1] = ~( ( a - d ) ^ ( c - d ) );
      }
    }
  }

  void Terra::prebuild( const char* name_ )
  {
    String name       = name_;
    String configFile = "terra/" + name + ".rc";
    String imageFile  = "terra/" + name + ".png";
    String destFile   = "terra/" + name + ".ozTerra";

    Config terraConfig;
    terraConfig.load( configFile );

    float heightStep = terraConfig.get( "step", 0.5f );
    float heightBias = terraConfig.get( "bias", 0.0f );

    // just to mark them as used for OZ_VERBOSE_CONFIG
    terraConfig.get( "detailTexture", "" );
    terraConfig.get( "mapTexture", "" );
    terraConfig.get( "waterTexture", "" );

    log.print( "Loading terrain heightmap image '%s' ...", name.cstr() );

    SDL_Surface* image = IMG_Load( imageFile );

    if( image == null ) {
      log.printEnd( " No such file" );
      throw Exception( "Failed to load terrain" );
    }
    if( image->w != VERTS || image->h != VERTS || image->format->BytesPerPixel != 1 ) {
      log.printEnd( " Invalid format: %d x %d %d bpp, should be %d x %d 8 bpp", image->w, image->h,
                   image->format->BytesPerPixel * 8, VERTS, VERTS );
      SDL_FreeSurface( image );
      throw Exception( "Failed to load terrain" );
    }

    int scanLineLength = image->pitch;
    const ubyte* line = reinterpret_cast<const ubyte*>( image->pixels );
    for( int y = VERTS - 1; y >= 0; --y ) {
      for( int x = 0; x < VERTS; ++x ) {
        quads[x][y].vertex.z = float( line[x] ) * heightStep + heightBias;
      }
      line += scanLineLength;
    }

    buildTerraFrame();

    SDL_FreeSurface( image );
    log.printEnd( " OK" );
    log.print( "Dumping terrain to '%s' ...", destFile.cstr() );

    int size = 0;

    size += 1 * int( sizeof( int ) );
    size += VERTS * VERTS * int( sizeof( Quad ) );

    Buffer buffer( size );
    OutputStream os = buffer.outputStream();

    os.writeInt( VERTS );

    for( int x = 0; x < VERTS; ++x ) {
      for( int y = 0; y < VERTS; ++y ) {
        os.writePoint3( quads[x][y].vertex );

        if( x == QUADS || y == QUADS ) {
          os.writeVec3( Vec3::ZERO );
          os.writeVec3( Vec3::ZERO );
        }
        else {
          os.writeVec3( quads[x][y].triNormal[0] );
          os.writeVec3( quads[x][y].triNormal[1] );
        }
      }
    }

    hard_assert( !os.isAvailable() );
    buffer.write( destFile );

    log.printEnd( " OK" );
  }

  void Terra::load( int id_ )
  {
    id = id_;

    const String& name = translator.terras[id].name;
    const String& path = translator.terras[id].path;

    log.print( "Loading terrain '%s' ...", name.cstr() );

    Buffer buffer( path );

    if( buffer.isEmpty() ) {
      log.printEnd( " Cannot read file" );
      throw Exception( "Failed to load terrain" );
    }

    InputStream is = buffer.inputStream();

    int max = is.readInt();
    if( max != VERTS ) {
      log.printEnd( " Invalid dimension %d, should be %d", max, VERTS );
      throw Exception( "Failed to load terrain" );
    }

    for( int x = 0; x < VERTS; ++x ) {
      for( int y = 0; y < VERTS; ++y ) {
        quads[x][y].vertex       = is.readPoint3();
        quads[x][y].triNormal[0] = is.readVec3();
        quads[x][y].triNormal[1] = is.readVec3();
      }
    }

    hard_assert( !is.isAvailable() );

    log.printEnd( " OK" );
  }

  void Terra::init()
  {
    for( int x = 0; x < VERTS; ++x ) {
      for( int y = 0; y < VERTS; ++y ) {
        quads[x][y].vertex.x = float( x * Quad::SIZEI ) - DIM;
        quads[x][y].vertex.y = float( y * Quad::SIZEI ) - DIM;
      }
    }
  }

}
