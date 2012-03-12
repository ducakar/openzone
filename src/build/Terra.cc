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

#include "client/Terra.hh"

#include "build/Context.hh"
#include "build/MeshData.hh"

#include <limits>
#include <FreeImage.h>

namespace oz
{
namespace build
{

void Terra::load()
{
  PhysFile configFile( "terra/" + name + ".rc" );
  PhysFile imageFile( "terra/" + name + ".png" );

  Config terraConfig;
  terraConfig.load( configFile );

  float minHeight = terraConfig.get( "minHeight", float( std::numeric_limits<short>::min() ) );
  float maxHeight = terraConfig.get( "maxHeight", float( std::numeric_limits<short>::max() ) );

  String sLiquid = terraConfig.get( "liquid", "WATER" );

  if( sLiquid.equals( "WATER" ) ) {
    liquid = Medium::WATER_BIT | Medium::SEA_BIT;
  }
  else if( sLiquid.equals( "LAVA" ) ) {
    liquid = Medium::LAVA_BIT | Medium::SEA_BIT;
  }
  else {
    throw Exception( "Liquid should be either WATER or LAVA" );
  }

  liquidColour.x = terraConfig.get( "liquidFogColour.r", 0.00f );
  liquidColour.y = terraConfig.get( "liquidFogColour.g", 0.05f );
  liquidColour.z = terraConfig.get( "liquidFogColour.b", 0.25f );
  liquidColour.w = 1.0f;

  liquidTexture  = terraConfig.get( "liquidTexture", "" );
  detailTexture  = terraConfig.get( "detailTexture", "" );
  mapTexture     = terraConfig.get( "mapTexture", "" );

  terraConfig.clear( true );

  log.print( "Loading terrain heightmap '%s' ...", name.cstr() );

  FIBITMAP* image = FreeImage_Load( FIF_PNG, imageFile.realPath().cstr() );
  if( image == null ) {
    throw Exception( "Failed to load heightmap '%s'", imageFile.realPath().cstr() );
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

      quads[x][y].vertex.x = float( x * matrix::Terra::Quad::SIZE - matrix::Terra::DIM );
      quads[x][y].vertex.y = float( y * matrix::Terra::Quad::SIZE - matrix::Terra::DIM );
      quads[x][y].vertex.z = Math::mix( minHeight, maxHeight, value );

      pixel += 3;
    }
  }

  FreeImage_Unload( image );

  log.printEnd( " OK" );
}

void Terra::saveMatrix()
{
  File destFile( "terra/" + name + ".ozTerra" );

  log.print( "Dumping terrain structure to '%s' ...", destFile.path().cstr() );

  BufferStream os;

  os.writeInt( matrix::Terra::VERTS );

  for( int x = 0; x < matrix::Terra::VERTS; ++x ) {
    for( int y = 0; y < matrix::Terra::VERTS; ++y ) {
      os.writeFloat( quads[x][y].vertex.z );
    }
  }

  os.writeInt( liquid );

  if( !destFile.write( &os ) ) {
    throw Exception( "Failed to write '%s'", destFile.path().cstr() );
  }

  log.printEnd( " OK" );
}

void Terra::saveClient()
{
  File destFile( "terra/" + name + ".ozcTerra" );
  File minimapFile( "terra/" + name + ".ozcTex" );

  log.println( "Compiling terrain model to '%s' {", destFile.path().cstr() );
  log.indent();

  uint liquidTexId = context.loadLayer( "terra/" + liquidTexture );
  uint detailTexId = context.loadLayer( "terra/" + detailTexture );
  uint mapTexId    = context.loadLayer( "terra/" + mapTexture );

  BufferStream os;

  context.writeLayer( liquidTexId, &os );
  context.writeLayer( detailTexId, &os );
  context.writeLayer( mapTexId, &os );

  glDeleteTextures( 1, &liquidTexId );
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
  Bitset waterTiles( client::Terra::TILES * client::Terra::TILES );
  waterTiles.clearAll();

  for( int i = 0; i < client::Terra::TILES; ++i ) {
    for( int j = 0; j < client::Terra::TILES; ++j ) {
      // tile
      for( int k = 0; k <= client::Terra::TILE_QUADS; ++k ) {
        for( int l = 0; l <= client::Terra::TILE_QUADS; ++l ) {
          int x = i * client::Terra::TILE_QUADS + k;
          int y = j * client::Terra::TILE_QUADS + l;

          Vec3 normal = Vec3::ZERO;

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

          if( quads[x][y].vertex.z < 0.0f ) {
            waterTiles.set( i * client::Terra::TILES + j );
          }

          os.writeVec3( normal );
        }
      }
    }
  }

  for( int i = 0; i < waterTiles.length(); ++i ) {
    os.writeChar( waterTiles.get( i ) );
  }

  os.writeVec4( liquidColour );

  if( !destFile.write( &os ) ) {
    throw Exception( "Failed to write '%s'", destFile.path().cstr() );
  }

  log.unindent();
  log.println( "}" );

  bool useS3TC = context.useS3TC;
  context.useS3TC = false;

  mapTexId = context.loadLayer( "terra/" + mapTexture );

  log.print( "Writing minimap texture '%s' ...", minimapFile.path().cstr() );

  os.reset();
  context.writeLayer( mapTexId, &os );
  glDeleteTextures( 1, &mapTexId );

  if( !minimapFile.write( &os ) ) {
    throw Exception( "Minimap texture '%s' writing failed", minimapFile.path().cstr() );
  }

  log.printEnd( " OK" );

  context.useS3TC = useS3TC;
}

Terra::Terra( const char* name_ ) :
  name( name_ )
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
