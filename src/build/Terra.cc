/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file build/Terra.cc
 */

#include "stable.hh"

#include "build/Terra.hh"

#include "matrix/Library.hh"

#include "client/Terra.hh"
#include "client/OpenGL.hh"

#include "build/Context.hh"

#include <limits>
#include <FreeImage.h>

namespace oz
{
namespace build
{

void Terra::load()
{
  File configFile( "terra/" + name + ".rc" );
  String imageFile  = "terra/" + name + ".png";

  Config terraConfig;
  terraConfig.load( configFile );

  float minHeight = terraConfig.get( "minHeight", float( std::numeric_limits<short>::min() ) );
  float maxHeight = terraConfig.get( "maxHeight", float( std::numeric_limits<short>::max() ) );

  waterTexture  = terraConfig.get( "waterTexture", "" );
  detailTexture = terraConfig.get( "detailTexture", "" );
  mapTexture    = terraConfig.get( "mapTexture", "" );

  terraConfig.clear( true );

  log.print( "Loading terrain heightmap '%s' ...", name.cstr() );

  FIBITMAP* image = FreeImage_Load( FIF_PNG, imageFile.cstr() );
  if( image == null ) {
    throw Exception( "Failed to load heightmap '%s'", imageFile.cstr() );
  }

  int width  = int( FreeImage_GetWidth( image ) );
  int height = int( FreeImage_GetHeight( image ) );
  int bpp    = int( FreeImage_GetBPP( image ) );
  int type   = int( FreeImage_GetImageType( image ) );

  if( width != matrix::Terra::VERTS || height != matrix::Terra::VERTS || bpp != 48 ||
      type != FIT_RGB16 )
  {
    throw Exception( "Invalid terrain heightmap format %d x %d %d bpp, "
                     "should be %d x %d 48 bpp RGB (red channel as greyscale)",
                     width, height, bpp, matrix::Terra::VERTS, matrix::Terra::VERTS );
  }

  log.print( "Calculating triangles ..." );

  for( int y = matrix::Terra::VERTS - 1; y >= 0; --y ) {
    const ushort* pixel = reinterpret_cast<const ushort*>( FreeImage_GetScanLine( image, y ) );

    for( int x = 0; x < matrix::Terra::VERTS; ++x ) {
      float value = float( *pixel ) / float( std::numeric_limits<unsigned short>::max() );

      quads[x][y].vertex.x     = float( x * matrix::Terra::Quad::SIZEI ) - matrix::Terra::DIM;
      quads[x][y].vertex.y     = float( y * matrix::Terra::Quad::SIZEI ) - matrix::Terra::DIM;
      quads[x][y].vertex.z     = Math::mix( minHeight, maxHeight, value );
      quads[x][y].triNormal[0] = Vec3::ZERO;
      quads[x][y].triNormal[1] = Vec3::ZERO;

      pixel += 3;
    }
  }

  FreeImage_Unload( image );

  for( int x = 0; x < matrix::Terra::QUADS; ++x ) {
    for( int y = 0; y < matrix::Terra::QUADS; ++y ) {
      if( x != matrix::Terra::QUADS && y != matrix::Terra::QUADS ) {
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

  log.printEnd( " OK" );
}

void Terra::saveMatrix()
{
  String destFile = "terra/" + name + ".ozTerra";

  log.print( "Dumping terrain structure to '%s' ...", destFile.cstr() );

  BufferStream os;

  os.writeInt( matrix::Terra::VERTS );

  for( int x = 0; x < matrix::Terra::VERTS; ++x ) {
    for( int y = 0; y < matrix::Terra::VERTS; ++y ) {
      os.writePoint3( quads[x][y].vertex );
      os.writeVec3( quads[x][y].triNormal[0] );
      os.writeVec3( quads[x][y].triNormal[1] );
    }
  }

  File( destFile ).write( &os );

  log.printEnd( " OK" );
}

void Terra::saveClient()
{
  String terraDir = "terra/";
  String destFile = terraDir + name + ".ozcTerra";

  log.println( "Compiling terrain model to '%s' {", destFile.cstr() );
  log.indent();

  uint waterTexId  = Context::loadRawTexture( terraDir + waterTexture );
  uint detailTexId = Context::loadRawTexture( terraDir + detailTexture );
  uint mapTexId    = Context::loadRawTexture( terraDir + mapTexture );

  BufferStream os;

  Context::writeTexture( waterTexId, &os );
  Context::writeTexture( detailTexId, &os );
  Context::writeTexture( mapTexId, &os );

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

          if( x < matrix::Terra::QUADS && y < matrix::Terra::QUADS ) {
            normal += quads[x][y].triNormal[0];
            normal += quads[x][y].triNormal[1];
          }
          if( x > 0 && y < matrix::Terra::QUADS ) {
            normal += quads[x - 1][y].triNormal[0];
          }
          if( x > 0 && y > 0 ) {
            normal += quads[x - 1][y - 1].triNormal[0];
            normal += quads[x - 1][y - 1].triNormal[1];
          }
          if( x < matrix::Terra::QUADS && y > 0 ) {
            normal += quads[x][y - 1].triNormal[1];
          }

          if( pos.z < 0.0f ) {
            waterTiles.set( i * client::Terra::TILES + j );
          }

          vertex.pos[0] = pos.x;
          vertex.pos[1] = pos.y;
          vertex.pos[2] = pos.z;

          vertex.texCoord[0] = float( x ) / float( matrix::Terra::VERTS );
          vertex.texCoord[1] = float( y ) / float( matrix::Terra::VERTS );

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

  File( destFile ).write( &os );

  log.unindent();
  log.println( "}" );
}

Terra::Terra( const char* name_ ) : name( name_ )
{}

void Terra::build( const char* name )
{
  Terra* terra = new Terra( name );
  terra->load();
  terra->saveMatrix();
  terra->saveClient();
  delete terra;
}

}
}
