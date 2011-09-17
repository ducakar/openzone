/*
 *  Terra.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Terra.hpp"

#include "matrix/Orbis.hpp"
#include "matrix/Terra.hpp"

#include "client/Camera.hpp"
#include "client/Context.hpp"
#include "client/Frustum.hpp"
#include "client/Colours.hpp"

namespace oz
{
namespace client
{

  const int   Terra::TILE_INDICES  = TILE_QUADS * ( TILE_QUADS + 1 ) * 2 + ( TILE_QUADS - 1 ) * 2;
  const int   Terra::TILE_VERTICES = ( TILE_QUADS + 1 ) * ( TILE_QUADS + 1 );

  const float Terra::TILE_SIZE     = TILE_QUADS * oz::Terra::Quad::SIZE;
  const float Terra::TILE_INV_SIZE = 1.0f / TILE_SIZE;

  const float Terra::WAVE_BIAS_INC = 2.0f * Timer::TICK_TIME;

  Terra terra;

  Terra::Terra() : ibo( 0 ), waterTexId( 0 ), detailTexId( 0 ), mapTexId( 0 )
  {
    for( int i = 0; i < TILES; ++i ) {
      for( int j = 0; j < TILES; ++j ) {
        vbos[i][j] = 0;
        vaos[i][j] = 0;
      }
    }
  }

#ifdef OZ_SDK
  void Terra::prebuild( const char* name_ )
  {
    String name       = name_;
    String configFile = "terra/" + name + ".rc";
    String outFile    = "terra/" + name + ".ozcTerra";

    log.println( "Compiling client terrain data to '%s' {", outFile.cstr() );
    log.indent();

    Config terraConfig;
    terraConfig.load( configFile );

    // just to mark them used to prevent unused config variables warning
    terraConfig.get( "step", 0.5f );
    terraConfig.get( "bias", 0.0f );

    String terraDir      = "terra/";
    String waterTexture  = terraDir + terraConfig.get( "waterTexture", "" );
    String detailTexture = terraDir + terraConfig.get( "detailTexture", "" );
    String mapTexture    = terraDir + terraConfig.get( "mapTexture", "" );

    uint waterTexId  = context.loadRawTexture( waterTexture );
    uint detailTexId = context.loadRawTexture( detailTexture );
    uint mapTexId    = context.loadRawTexture( mapTexture, true,
                                               GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR );

    OutputStream os = buffer.outputStream();

    context.writeTexture( waterTexId, &os );
    context.writeTexture( detailTexId, &os );
    context.writeTexture( mapTexId, &os );

    glDeleteTextures( 1, &waterTexId );
    glDeleteTextures( 1, &detailTexId );
    glDeleteTextures( 1, &mapTexId );

    // generate index buffer
    int index = 0;
    for( int x = 0; x < TILE_QUADS; ++x ) {
      if( x != 0 ) {
        os.writeShort( short( index + TILE_QUADS + 1 ) );
      }
      for( int y = 0; y <= TILE_QUADS; ++y ) {
        os.writeShort( short( index + TILE_QUADS + 1 ) );
        os.writeShort( short( index ) );
        ++index;
      }
      if( x != TILE_QUADS - 1 ) {
        os.writeShort( short( index - 1 ) );
      }
    }

    // generate vertex buffers
    Point3 pos;
    Vec3   normal;
    Vertex vertex;

    waterTiles.clearAll();

    for( int i = 0; i < TILES; ++i ) {
      for( int j = 0; j < TILES; ++j ) {
        // tile
        const auto& quads = orbis.terra.quads;

        for( int k = 0; k <= TILE_QUADS; ++k ) {
          for( int l = 0; l <= TILE_QUADS; ++l ) {
            int x = i * TILE_QUADS + k;
            int y = j * TILE_QUADS + l;

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
              waterTiles.set( i * TILES + j );
            }

            vertex.pos = pos;
            vertex.texCoord.u = float( x ) / float( oz::Terra::VERTS );
            vertex.texCoord.v = float( y ) / float( oz::Terra::VERTS );
            vertex.normal = normal;
            vertex.write( &os );
          }
        }
      }
    }

    for( int i = 0; i < waterTiles.length(); ++i ) {
      os.writeChar( waterTiles.get( i ) );
    }

    buffer.write( outFile, os.length() );

    log.unindent();
    log.println( "}" );
  }
#endif

  void Terra::draw()
  {
    span.minX = max( int( ( camera.p.x - frustum.radius + oz::Terra::DIM ) * TILE_INV_SIZE ), 0 );
    span.minY = max( int( ( camera.p.y - frustum.radius + oz::Terra::DIM ) * TILE_INV_SIZE ), 0 );
    span.maxX = min( int( ( camera.p.x + frustum.radius + oz::Terra::DIM ) * TILE_INV_SIZE ), TILES - 1 );
    span.maxY = min( int( ( camera.p.y + frustum.radius + oz::Terra::DIM ) * TILE_INV_SIZE ), TILES - 1 );

    shader.use( shader.isInWater ? submergedLandShaderId : landShaderId );

    tf.model = Mat44::ID;
    tf.apply();

    glBindTexture( GL_TEXTURE_2D, detailTexId );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, mapTexId );

    hard_assert( glGetError() == GL_NO_ERROR );

    // to match strip triangles with matrix terrain we have to make them clockwise since
    // we draw column-major (strips along y axis) for better cache performance
    glFrontFace( GL_CW );

    for( int i = span.minX; i <= span.maxX; ++i ) {
      for( int j = span.minY; j <= span.maxY; ++j ) {
        glBindVertexArray( vaos[i][j] );
        glDrawElements( GL_TRIANGLE_STRIP, TILE_INDICES, GL_UNSIGNED_SHORT, 0 );
      }
    }

    glFrontFace( GL_CCW );

    glBindTexture( GL_TEXTURE_2D, 0 );
    glActiveTexture( GL_TEXTURE0 );

    hard_assert( glGetError() == GL_NO_ERROR );
  }

  void Terra::drawWater()
  {
    waveBias = Math::mod( waveBias + WAVE_BIAS_INC, Math::TAU );

    shader.use( waterShaderId );

    glUniform1f( param.oz_Specular, 0.5f );
    glUniform1f( param.oz_WaveBias, waveBias );
    tf.model = Mat44::ID;
    tf.apply();

    glBindTexture( GL_TEXTURE_2D, waterTexId );

    if( camera.p.z >= 0.0f ) {
      glFrontFace( GL_CW );
    }

    glEnable( GL_BLEND );

#ifdef OZ_MESA_COMPATIBLE
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
#endif

    for( int i = span.minX; i <= span.maxX; ++i ) {
      for( int j = span.minY; j <= span.maxY; ++j ) {
        if( waterTiles.get( i * TILES + j ) ) {
#ifdef OZ_MESA_COMPATIBLE
          glBindBuffer( GL_ARRAY_BUFFER, vbos[i][j] );
          Vertex::setFormat();
#else
          glBindVertexArray( vaos[i][j] );
#endif
          glDrawElements( GL_TRIANGLE_STRIP, TILE_INDICES, GL_UNSIGNED_SHORT, 0 );
        }
      }
    }

    glDisable( GL_BLEND );

    if( camera.p.z >= 0.0f ) {
      glFrontFace( GL_CCW );
    }

    hard_assert( glGetError() == GL_NO_ERROR );
  }

  void Terra::load()
  {
    const String& name = translator.terras[orbis.terra.id].name;
    String path = "terra/" + name + ".ozcTerra";

    log.print( "Loading terra '%s' ...", name.cstr() );

    if( !buffer.read( path ) ) {
      log.printEnd( " Failed" );
      throw Exception( "Terra loading failed" );
    }

    InputStream is = buffer.inputStream();

    ushort* indices  = new ushort[TILE_INDICES];
    Vertex* vertices = new Vertex[TILE_VERTICES];

    waterTexId  = context.readTexture( &is );
    detailTexId = context.readTexture( &is );
    mapTexId    = context.readTexture( &is );

    glGenVertexArrays( TILES * TILES, &vaos[0][0] );
    glGenBuffers( TILES * TILES, &vbos[0][0] );
    glGenBuffers( 1, &ibo );

    for( int i = 0; i < TILE_INDICES; ++i ) {
      indices[i] = ushort( is.readShort() );
    }

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, TILE_INDICES * sizeof( ushort ), indices,
                  GL_STATIC_DRAW );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    for( int i = 0; i < TILES; ++i ) {
      for( int j = 0; j < TILES; ++j ) {
        for( int k = 0; k < TILE_VERTICES; ++k ) {
          vertices[k].read( &is );
        }

        glBindVertexArray( vaos[i][j] );

        glBindBuffer( GL_ARRAY_BUFFER, vbos[i][j] );
        glBufferData( GL_ARRAY_BUFFER, TILE_VERTICES * sizeof( Vertex ), vertices,
                      GL_STATIC_DRAW );

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );

        Vertex::setFormat();

        glBindVertexArray( 0 );
      }
    }

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    waterTiles.clearAll();
    for( int i = 0; i < waterTiles.length(); ++i ) {
      if( is.readChar() ) {
        waterTiles.set( i );
      }
    }

    landShaderId = translator.shaderIndex( "terraLand" );
    waterShaderId = translator.shaderIndex( "terraWater" );
    submergedLandShaderId = translator.shaderIndex( "submergedTerraLand" );
    submergedWaterShaderId = translator.shaderIndex( "submergedTerraWater" );

    delete[] indices;
    delete[] vertices;

    log.printEnd( " OK" );
  }

  void Terra::unload()
  {
    if( ibo != 0 ) {
      glDeleteTextures( 1, &mapTexId );
      glDeleteTextures( 1, &detailTexId );
      glDeleteTextures( 1, &waterTexId );

      glDeleteBuffers( 1, &ibo );
      glDeleteBuffers( TILES * TILES, &vbos[0][0] );
      glDeleteVertexArrays( TILES * TILES, &vaos[0][0] );

      mapTexId = 0;
      detailTexId = 0;
      waterTexId = 0;

      ibo = 0;
      for( int i = 0; i < TILES; ++i ) {
        for( int j = 0; j < TILES; ++j ) {
          vbos[i][j] = 0;
          vaos[i][j] = 0;
        }
      }
    }
  }

}
}
