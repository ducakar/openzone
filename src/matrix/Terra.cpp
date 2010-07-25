/*
 *  Terra.cpp
 *
 *  Matrix structure for terrain
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Terra.hpp"

#include <SDL_image.h>

namespace oz
{

  const float Terra::Quad::SIZE     = float( Terra::Quad::SIZEI );
  const float Terra::Quad::INV_SIZE = 1.0f / float( Terra::Quad::SIZEI );
  const float Terra::Quad::DIM      = Terra::Quad::SIZE / 2.0f;

  const float Terra::DIM            = Terra::Quad::DIM * Terra::QUADS;

#ifndef OZ_PREBUILT

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

#endif

  void Terra::init()
  {
    for( int x = 0; x < MAX; ++x ) {
      for( int y = 0; y < MAX; ++y ) {
        vertices[x][y].x = float( x * Quad::SIZEI ) - DIM;
        vertices[x][y].y = float( y * Quad::SIZEI ) - DIM;
      }
    }
  }

#ifndef OZ_PREBUILT
  void Terra::load( float height )
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
#endif

  void Terra::load( const char* name_ )
  {
#ifdef OZ_PREBUILT

    String terraFile = "terra/" + String( name_ ) + ".ozTerra";

    log.print( "Loading terrain '%s' ...", name_ );

    Buffer buffer;
    buffer.load( terraFile );

    if( buffer.isEmpty() ) {
      log.printEnd( " Cannot read file" );
      throw Exception( "Failed to load terrain" );
    }

    InputStream is = buffer.inputStream();

    int max = is.readInt();
    if( max != MAX ) {
      log.printEnd( " Invalid dimension %d, should be %d", max, MAX );
      throw Exception( "Failed to load terrain" );
    }

    is.readString( detailTexture );
    is.readString( mapTexture );
    is.readString( waterTexture );

    for( int x = 0; x < MAX; ++x ) {
      for( int y = 0; y < MAX; ++y ) {
        vertices[x][y] = is.readVec3();
      }
    }

    for( int x = 0; x < QUADS; ++x ) {
      for( int y = 0; y < QUADS; ++y ) {
        quads[x][y].tri[0].normal   = is.readVec3();
        quads[x][y].tri[0].distance = is.readFloat();
        quads[x][y].tri[1].normal   = is.readVec3();
        quads[x][y].tri[1].distance = is.readFloat();
      }
    }

    log.printEnd( " OK" );

#else

    String name = name_;
    String configFile = "terra/" + name + ".rc";
    String imageFile = "terra/" + name + ".png";

    Config terraConfig;
    terraConfig.load( configFile );

    float heightStep = terraConfig.get( "step", 0.5f );
    float heightBias = terraConfig.get( "bias", 0.0f );

    String terraString = "terra/";
    detailTexture = terraString + terraConfig.get( "detailTexture", "" );
    mapTexture = terraString + terraConfig.get( "mapTexture", "" );
    waterTexture = terraString + terraConfig.get( "waterTexture", "" );

    log.print( "Loading terrain heightmap image '%s' ...", name.cstr() );

    SDL_Surface* image = IMG_Load( imageFile );

    if( image == null ) {
      log.printEnd( " No such file" );
      throw Exception( "Failed to load terrain" );
    }
    if( image->w != MAX || image->h != MAX || image->format->BytesPerPixel != 1 ) {
      log.printEnd( " Invalid format: %d x %d %d bpp, should be %d x %d 8 bpp", image->w, image->h,
                   image->format->BytesPerPixel * 8, MAX, MAX );
      SDL_FreeSurface( image );
      throw Exception( "Failed to load terrain" );
    }

    int scanLineLength = image->pitch;
    const ubyte* line = reinterpret_cast<const ubyte*>( image->pixels );
    for( int y = MAX - 1; y >= 0; --y ) {
      for( int x = 0; x < MAX; ++x ) {
        vertices[x][y].z = float( line[x] ) * heightStep + heightBias;
      }
      line += scanLineLength;
    }

    buildTerraFrame();

    SDL_FreeSurface( image );
    log.printEnd( " OK" );

#ifdef OZ_BUILD
    save( "terra/" + name + ".ozTerra" );
#endif

#endif
  }

#ifndef OZ_PREBUILT

  void Terra::save( const char* fileName )
  {
    log.print( "Dumping terrain to '%s' ...", fileName );

    int size = 0;

    size += 1 * int( sizeof( int ) );
    size += detailTexture.length() + 1;
    size += mapTexture.length() + 1;
    size += waterTexture.length() + 1;
    size += MAX * MAX * int( sizeof( Vec3 ) );
    size += QUADS * QUADS * int( sizeof( Quad ) );

    Buffer buffer( size );
    OutputStream os = buffer.outputStream();

    os.writeInt( MAX );
    os.writeString( detailTexture );
    os.writeString( mapTexture );
    os.writeString( waterTexture );

    for( int x = 0; x < MAX; ++x ) {
      for( int y = 0; y < MAX; ++y ) {
        os.writeVec3( vertices[x][y] );
      }
    }

    for( int x = 0; x < QUADS; ++x ) {
      for( int y = 0; y < QUADS; ++y ) {
        os.writeVec3( quads[x][y].tri[0].normal );
        os.writeFloat( quads[x][y].tri[0].distance );
        os.writeVec3( quads[x][y].tri[1].normal );
        os.writeFloat( quads[x][y].tri[1].distance );
      }
    }

    buffer.write( fileName );

    log.printEnd( " OK" );
  }

#endif

}
