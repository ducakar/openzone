/*
 *  Terra.cpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "build/Terra.hpp"

#include "matrix/Library.hpp"

#include "client/Context.hpp"
#include "client/Terra.hpp"

#include "client/OpenGL.hpp"

#include <SDL_image.h>

using namespace oz::client;

namespace oz
{
namespace build
{

void Terra::load()
{
  String configFile = "terra/" + name + ".rc";
  String imageFile  = "terra/" + name + ".png";

  Config terraConfig;
  terraConfig.load( configFile );

  float heightStep = terraConfig.get( "step", 0.5f );
  float heightBias = terraConfig.get( "bias", 0.0f );

  waterTexture  = terraConfig.get( "waterTexture", "" );
  detailTexture = terraConfig.get( "detailTexture", "" );
  mapTexture    = terraConfig.get( "mapTexture", "" );

  log.print( "Loading terrain heightmap '%s' ...", name.cstr() );

  SDL_Surface* image = IMG_Load( imageFile );
  if( image == null ) {
    throw Exception( "Terrain heightmap missing" );
  }
  if( image->w != oz::Terra::VERTS || image->h != oz::Terra::VERTS ||
      image->format->BytesPerPixel != 1 )
  {
    SDL_FreeSurface( image );
    throw Exception( "Invalid terrain heightmap format, should be " + String( oz::Terra::VERTS ) +
                     " x " + String( oz::Terra::VERTS ) + " 8 bpp" );
  }

  const ubyte* line = reinterpret_cast<const ubyte*>( image->pixels );
  for( int y = oz::Terra::VERTS - 1; y >= 0; --y ) {
    for( int x = 0; x < oz::Terra::VERTS; ++x ) {
      quads[x][y].vertex.x     = float( x * oz::Terra::Quad::SIZEI ) - oz::Terra::DIM;
      quads[x][y].vertex.y     = float( y * oz::Terra::Quad::SIZEI ) - oz::Terra::DIM;
      quads[x][y].vertex.z     = float( line[x] ) * heightStep + heightBias;
      quads[x][y].triNormal[0] = Vec3::ZERO;
      quads[x][y].triNormal[1] = Vec3::ZERO;
    }
    line += image->pitch;
  }

  for( int x = 0; x < oz::Terra::QUADS; ++x ) {
    for( int y = 0; y < oz::Terra::QUADS; ++y ) {
      if( x != oz::Terra::QUADS && y != oz::Terra::QUADS ) {
        //
        // 0. triangle -- upper left
        // 1. triangle -- lower right
        //
        //    |  ...  |         D        C
        //    +---+---+-         o----->o
        //    |1 /|1 /|          |      ^
        //    | / | / |          |      |
        //    |/ 0|/ 0|          |      |
        //    +---+---+- ...     v      |
        //    |1 /|1 /|          o<-----o
        //    | / | / |         A        B
        //    |/ 0|/ 0|
        //  (0,0)
        //
        const Point3& a = quads[x    ][y    ].vertex;
        const Point3& b = quads[x + 1][y    ].vertex;
        const Point3& c = quads[x + 1][y + 1].vertex;
        const Point3& d = quads[x    ][y + 1].vertex;

        quads[x][y].triNormal[0] = ~( ( c - b ) ^ ( a - b ) );
        quads[x][y].triNormal[1] = ~( ( a - d ) ^ ( c - d ) );
      }
    }
  }

  SDL_FreeSurface( image );

  log.printEnd( " OK" );
}

void Terra::saveMatrix()
{
  String destFile = "terra/" + name + ".ozTerra";

  log.print( "Dumping terrain structure to '%s' ...", destFile.cstr() );

  int size = 0;
  size += int( sizeof( int ) );
  size += oz::Terra::VERTS * oz::Terra::VERTS * 9 * int( sizeof( float ) );

  Buffer buffer( size );
  OutputStream os = buffer.outputStream();

  os.writeInt( oz::Terra::VERTS );

  for( int x = 0; x < oz::Terra::VERTS; ++x ) {
    for( int y = 0; y < oz::Terra::VERTS; ++y ) {
      os.writePoint3( quads[x][y].vertex );
      os.writeVec3( quads[x][y].triNormal[0] );
      os.writeVec3( quads[x][y].triNormal[1] );
    }
  }

  hard_assert( os.available() == 0 );
  buffer.write( destFile, os.length() );

  log.printEnd( " OK" );
}

void Terra::saveClient()
{
  String terraDir = "terra/";
  String destFile = terraDir + name + ".ozcTerra";

  log.println( "Compiling client terrain data to '%s' {", destFile.cstr() );
  log.indent();

  uint waterTexId  = context.loadRawTexture( terraDir + waterTexture );
  uint detailTexId = context.loadRawTexture( terraDir + detailTexture );
  uint mapTexId    = context.loadRawTexture( terraDir + mapTexture );

  Buffer buffer( 20 * 1024 * 1024 );
  OutputStream os = buffer.outputStream();

  context.writeTexture( waterTexId, &os );
  context.writeTexture( detailTexId, &os );
  context.writeTexture( mapTexId, &os );

  glDeleteTextures( 1, &waterTexId );
  glDeleteTextures( 1, &detailTexId );
  glDeleteTextures( 1, &mapTexId );

  // generate index buffer
  int index = 0;
  for( int x = 0; x < client::Terra::TILE_QUADS; ++x ) {
    if( x != 0 ) {
      os.writeShort( short( index + client::Terra::TILE_QUADS + 1 ) );
    }
    for( int y = 0; y <= client::Terra::TILE_QUADS; ++y ) {
      os.writeShort( short( index + client::Terra::TILE_QUADS + 1 ) );
      os.writeShort( short( index ) );
      ++index;
    }
    if( x != client::Terra::TILE_QUADS - 1 ) {
      os.writeShort( short( index - 1 ) );
    }
  }

  // generate vertex buffers
  Point3 pos;
  Vec3   normal;
  Vertex vertex;

  Bitset waterTiles( client::Terra::TILES * client::Terra::TILES );
  waterTiles.clearAll();

  for( int i = 0; i < client::Terra::TILES; ++i ) {
    for( int j = 0; j < client::Terra::TILES; ++j ) {
      // tile
      for( int k = 0; k <= client::Terra::TILE_QUADS; ++k ) {
        for( int l = 0; l <= client::Terra::TILE_QUADS; ++l ) {
          int x = i * client::Terra::TILE_QUADS + k;
          int y = j * client::Terra::TILE_QUADS + l;

          pos    = quads[x][y].vertex;
          normal = Vec3::ZERO;

          if( x < oz::Terra::QUADS && y < oz::Terra::QUADS ) {
            normal += quads[x][y].triNormal[0];
            normal += quads[x][y].triNormal[1];
          }
          if( x > 0 && y < oz::Terra::QUADS ) {
            normal += quads[x - 1][y].triNormal[0];
          }
          if( x > 0 && y > 0 ) {
            normal += quads[x - 1][y - 1].triNormal[0];
            normal += quads[x - 1][y - 1].triNormal[1];
          }
          if( x < oz::Terra::QUADS && y > 0 ) {
            normal += quads[x][y - 1].triNormal[1];
          }

          if( pos.z < 0.0f ) {
            waterTiles.set( i * client::Terra::TILES + j );
          }

          vertex.pos[0] = pos.x;
          vertex.pos[1] = pos.y;
          vertex.pos[2] = pos.z;

          vertex.texCoord[0] = float( x ) / float( oz::Terra::VERTS );
          vertex.texCoord[1] = float( y ) / float( oz::Terra::VERTS );

          vertex.normal[0] = normal.x;
          vertex.normal[1] = normal.y;
          vertex.normal[2] = normal.z;

          vertex.write( &os );
        }
      }
    }
  }

  for( int i = 0; i < waterTiles.length(); ++i ) {
    os.writeChar( waterTiles.get( i ) );
  }

  buffer.write( destFile, os.length() );

  log.unindent();
  log.println( "}" );
}

Terra::Terra( const char* name_ ) : name( name_ )
{}

void Terra::prebuild( const char* name )
{
  Terra* terra = new Terra( name );
  terra->load();
  terra->saveMatrix();
  terra->saveClient();
  delete terra;
}

}
}
