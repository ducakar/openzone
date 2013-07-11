/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file builder/Terra.cc
 */

#include <builder/Terra.hh>

#include <builder/Context.hh>

#include <FreeImage.h>

namespace oz
{
namespace builder
{

void Terra::load()
{
  File configFile( "@terra/" + name + ".json" );

  JSON config;
  if( !config.load( configFile ) ) {
    OZ_ERROR( "Failed to load terra configuration '%s'", configFile.path().cstr() );
  }

  static const EnumName LIQUID_MAP[] = {
    { Medium::WATER_BIT | Medium::SEA_BIT, "WATER" },
    { Medium::LAVA_BIT  | Medium::SEA_BIT, "LAVA"  }
  };
  EnumMap<int> liquidMap( LIQUID_MAP );

  liquid        = liquidMap[ config["liquid"].get( "" ) ];
  liquidColour  = config["liquidFogColour"].get( Vec4( 0.00f, 0.05f, 0.20f, 1.00f ) );
  liquidTexture = config["liquidTexture"].get( "" );
  detailTexture = config["detailTexture"].get( "" );
  mapTexture    = config["mapTexture"].get( "" );

  float minHeight = config["bottomHeight"].get( -200.0f );
  float maxHeight = config["topHeight"].get( +200.0f );

  File heightmapFile = String::str( "@terra/%s", config["heightmap"].get( "" ) );

  config.clear( true );

  FIBITMAP* image     = nullptr;
  float*    heightmap = nullptr;

  if( !heightmapFile.type() == File::REGULAR ) {
    Log::print( "Loading terrain heightmap image '%s' ...", name.cstr() );

    String realPath  = heightmapFile.realPath();

    image = FreeImage_Load( FIF_PNG, realPath );
    if( image == nullptr ) {
      OZ_ERROR( "Failed to load heightmap '%s'", realPath.cstr() );
    }

    int width  = int( FreeImage_GetWidth( image ) );
    int height = int( FreeImage_GetHeight( image ) );
    int bpp    = int( FreeImage_GetBPP( image ) );
    int type   = int( FreeImage_GetImageType( image ) );

    if( type != FIT_RGB16 || bpp != 48 || width != VERTS || height != VERTS ) {
      OZ_ERROR( "Invalid terrain heightmap format %d x %d %d bpp, should be %d x %d and 48 bpp RGB"
                " (red channel is used as height, green and blue are ignored)",
                width, height, bpp, VERTS, VERTS );
    }

    for( int y = 0; y < VERTS; ++y ) {
      const ushort* pixel = reinterpret_cast<const ushort*>( FreeImage_GetScanLine( image, y ) );

      for( int x = 0; x < VERTS; ++x ) {
        float value = float( *pixel ) / float( USHRT_MAX );

        quads[x][y].vertex.x     = float( x * Quad::SIZE - DIM );
        quads[x][y].vertex.y     = float( y * Quad::SIZE - DIM );
        quads[x][y].vertex.z     = Math::mix( minHeight, maxHeight, value );
        quads[x][y].triNormal[0] = Vec3::ZERO;
        quads[x][y].triNormal[1] = Vec3::ZERO;

        pixel += 3;
      }
    }

    Log::printEnd( " OK" );
  }
  else {
    Log::print( "Generating terrain heightmap ..." );

    TerraBuilder::setBounds( minHeight, maxHeight );
    TerraBuilder::setBounds( -200.0f, +200.0f );

    heightmap = TerraBuilder::generateHeightmap( VERTS, VERTS );

    for( int x = 0; x < VERTS; ++x ) {
      for( int y = 0; y < VERTS; ++y ) {
        quads[x][y].vertex.x     = float( x * Quad::SIZE - DIM );
        quads[x][y].vertex.y     = float( y * Quad::SIZE - DIM );
        quads[x][y].vertex.z     = heightmap[x * VERTS + y];
        quads[x][y].triNormal[0] = Vec3::ZERO;
        quads[x][y].triNormal[1] = Vec3::ZERO;
      }
    }

    delete[] heightmap;

    Log::printEnd( " OK" );
  }

  Log::print( "Calculating triangles ..." );

  for( int x = 0; x < QUADS; ++x ) {
    for( int y = 0; y < QUADS; ++y ) {
      if( x != QUADS && y != QUADS ) {
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
        const Point& a = quads[x    ][y    ].vertex;
        const Point& b = quads[x + 1][y    ].vertex;
        const Point& c = quads[x + 1][y + 1].vertex;
        const Point& d = quads[x    ][y + 1].vertex;

        quads[x][y].triNormal[0] = ~( ( c - b ) ^ ( a - b ) );
        quads[x][y].triNormal[1] = ~( ( a - d ) ^ ( c - d ) );
      }
    }
  }

  if( image != nullptr ) {
    FreeImage_Unload( image );
  }

  Log::printEnd( " OK" );
}

void Terra::saveMatrix()
{
  File destFile( "terra/" + name + ".ozmTerra" );

  Log::print( "Dumping terrain structure to '%s' ...", destFile.path().cstr() );

  OutputStream os( 0 );

  os.writeInt( VERTS );

  for( int x = 0; x < VERTS; ++x ) {
    for( int y = 0; y < VERTS; ++y ) {
      os.writeFloat( quads[x][y].vertex.z );
    }
  }

  os.writeInt( liquid );

  if( !destFile.write( os.begin(), os.tell() ) ) {
    OZ_ERROR( "Failed to write '%s'", destFile.path().cstr() );
  }

  Log::printEnd( " OK" );
}

void Terra::saveClient()
{
  File destFile( "terra/" + name + ".ozcTerra" );

  Log::println( "Compiling terrain model to '%s' {", destFile.path().cstr() );
  Log::indent();

  context.buildTexture( "@terra/" + liquidTexture.fileBaseName(), "terra/" + name + "-liquid" );
  context.buildTexture( "@terra/" + detailTexture.fileBaseName(), "terra/" + name + "-detail" );

  if( !mapTexture.isEmpty() ) {
    context.buildTexture( "@terra/" + mapTexture.fileBaseName(), "terra/" + name + "-map" );
  }
  else {
    TerraBuilder::addGradientPoint( Vec4( 0.00f, 0.00f, 0.10f, -1.00f * 100 ) );
    TerraBuilder::addGradientPoint( Vec4( 0.00f, 0.20f, 0.40f, -0.20f * 100 ) );
    TerraBuilder::addGradientPoint( Vec4( 0.20f, 0.60f, 0.60f, -0.10f * 100 ) );
    TerraBuilder::addGradientPoint( Vec4( 0.80f, 0.60f, 0.20f, +0.00f * 100 ) );
    TerraBuilder::addGradientPoint( Vec4( 0.10f, 0.40f, 0.15f, +0.20f * 100 ) );
    TerraBuilder::addGradientPoint( Vec4( 0.05f, 0.30f, 0.10f, +0.50f * 100 ) );
    TerraBuilder::addGradientPoint( Vec4( 0.50f, 0.50f, 0.50f, +0.80f * 100 ) );
    TerraBuilder::addGradientPoint( Vec4( 0.80f, 0.80f, 0.80f, +0.95f * 100 ) );

    char* image = TerraBuilder::generateImage( VERTS - 1, VERTS - 1 );
    ImageBuilder::createDDS( image, VERTS - 1, VERTS - 1, 24,
                            ImageBuilder::MIPMAPS_BIT, "terra/" + name + "-map.dds" );
  }

  OutputStream os( 0 );

  // generate index buffer
  int index = 0;
  for( int x = 0; x < TILE_QUADS; ++x ) {
    if( x != 0 ) {
      os.writeUShort( ushort( index + TILE_QUADS + 1 ) );
    }
    for( int y = 0; y <= TILE_QUADS; ++y ) {
      os.writeUShort( ushort( index + TILE_QUADS + 1 ) );
      os.writeUShort( ushort( index ) );
      ++index;
    }
    if( x != TILE_QUADS - 1 ) {
      os.writeUShort( ushort( index - 1 ) );
    }
  }

  // generate vertex buffers
  Bitset waterTiles( TILES * TILES );
  waterTiles.clearAll();

  for( int i = 0; i < TILES; ++i ) {
    for( int j = 0; j < TILES; ++j ) {
      // tile
      for( int k = 0; k <= TILE_QUADS; ++k ) {
        for( int l = 0; l <= TILE_QUADS; ++l ) {
          int x = i * TILE_QUADS + k;
          int y = j * TILE_QUADS + l;

          Vec3 normal = Vec3::ZERO;

          if( x < QUADS && y < QUADS ) {
            normal += quads[x][y].triNormal[0];
            normal += quads[x][y].triNormal[1];
          }
          if( x > 0 && y < QUADS ) {
            normal += quads[x - 1][y].triNormal[0];
          }
          if( x > 0 && y > 0 ) {
            normal += quads[x - 1][y - 1].triNormal[0];
            normal += quads[x - 1][y - 1].triNormal[1];
          }
          if( x < QUADS && y > 0 ) {
            normal += quads[x][y - 1].triNormal[1];
          }
          normal = ~normal;

          if( ( quads[x][y].vertex.z < 0.0f ) ||
              ( x + 1 < VERTS && quads[x + 1][y].vertex.z < 0.0f ) ||
              ( y + 1 < VERTS && quads[x][y + 1].vertex.z < 0.0f ) ||
              ( x + 1 < VERTS && y + 1 < VERTS &&
                quads[x + 1][y + 1].vertex.z < 0.0f ) )
          {
            waterTiles.set( i * TILES + j );
          }

          os.writeByte( byte( normal.x * 127.0f ) );
          os.writeByte( byte( normal.y * 127.0f ) );
          os.writeByte( byte( normal.z * 127.0f ) );
        }
      }
    }
  }

  for( int i = 0; i < waterTiles.length(); ++i ) {
    os.writeBool( waterTiles.get( i ) );
  }

  os.writeVec4( liquidColour );

  if( !destFile.write( os.begin(), os.tell() ) ) {
    OZ_ERROR( "Failed to write '%s'", destFile.path().cstr() );
  }

  Log::unindent();
  Log::println( "}" );
}

void Terra::build( const char* name_ )
{
  name = name_;

  load();
  saveMatrix();
  saveClient();

  name          = "";
  liquidTexture = "";
  detailTexture = "";
  mapTexture    = "";
}

Terra terra;

}
}
