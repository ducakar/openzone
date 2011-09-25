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

#include "client/OpenGL.hpp"

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

#ifndef OZ_TOOLS

  Terra::Terra() : ibo( 0 ), waterTexId( 0 ), detailTexId( 0 ), mapTexId( 0 ), id( -1 )
  {
    for( int i = 0; i < TILES; ++i ) {
      for( int j = 0; j < TILES; ++j ) {
        vbos[i][j] = 0;
        vaos[i][j] = 0;
      }
    }
  }

  void Terra::draw()
  {
    if( id == -1 ) {
      return;
    }

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

    OZ_GL_CHECK_ERROR();

    // to match strip triangles with matrix terrain we have to make them clockwise since
    // we draw column-major (strips along y axis) for better cache performance
    glFrontFace( GL_CW );

# ifdef OZ_GL_COMPATIBLE
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
# endif

    for( int i = span.minX; i <= span.maxX; ++i ) {
      for( int j = span.minY; j <= span.maxY; ++j ) {
# ifdef OZ_GL_COMPATIBLE
        glBindBuffer( GL_ARRAY_BUFFER, vbos[i][j] );
        Vertex::setFormat();
# else
        glBindVertexArray( vaos[i][j] );
# endif
        glDrawElements( GL_TRIANGLE_STRIP, TILE_INDICES, GL_UNSIGNED_SHORT, 0 );
      }
    }

    glFrontFace( GL_CCW );

    glBindTexture( GL_TEXTURE_2D, 0 );
    glActiveTexture( GL_TEXTURE0 );

    OZ_GL_CHECK_ERROR();
  }

  void Terra::drawWater()
  {
    if( id == -1 ) {
      return;
    }

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

# ifdef OZ_GL_COMPATIBLE
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
# endif

    for( int i = span.minX; i <= span.maxX; ++i ) {
      for( int j = span.minY; j <= span.maxY; ++j ) {
        if( waterTiles.get( i * TILES + j ) ) {
# ifdef OZ_GL_COMPATIBLE
          glBindBuffer( GL_ARRAY_BUFFER, vbos[i][j] );
          Vertex::setFormat();
# else
          glBindVertexArray( vaos[i][j] );
# endif
          glDrawElements( GL_TRIANGLE_STRIP, TILE_INDICES, GL_UNSIGNED_SHORT, 0 );
        }
      }
    }

    glDisable( GL_BLEND );

    if( camera.p.z >= 0.0f ) {
      glFrontFace( GL_CCW );
    }

    OZ_GL_CHECK_ERROR();
  }

  void Terra::load()
  {
    id = orbis.terra.id;

    const String& name = library.terras[id].name;
    String path = "terra/" + name + ".ozcTerra";

    log.print( "Loading terra '%s' ...", name.cstr() );

    Buffer buffer;
    if( !buffer.read( path ) ) {
      log.printEnd( " Failed" );
      throw Exception( "Terra loading failed" );
    }

    InputStream is = buffer.inputStream();

    waterTexId  = context.readTexture( &is );
    detailTexId = context.readTexture( &is );
    mapTexId    = context.readTexture( &is );

# ifndef OZ_GL_COMPATIBLE
    glGenVertexArrays( TILES * TILES, &vaos[0][0] );
# endif
    glGenBuffers( TILES * TILES, &vbos[0][0] );
    glGenBuffers( 1, &ibo );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, TILE_INDICES * sizeof( ushort ), 0, GL_STATIC_DRAW );

    ushort* indices =
        reinterpret_cast<ushort*>( glMapBuffer( GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY ) );

    for( int i = 0; i < TILE_INDICES; ++i ) {
      indices[i] = ushort( is.readShort() );
    }

    glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    for( int i = 0; i < TILES; ++i ) {
      for( int j = 0; j < TILES; ++j ) {
# ifndef OZ_GL_COMPATIBLE
        glBindVertexArray( vaos[i][j] );
# endif

        glBindBuffer( GL_ARRAY_BUFFER, vbos[i][j] );
        glBufferData( GL_ARRAY_BUFFER, TILE_VERTICES * sizeof( Vertex ), 0, GL_STATIC_DRAW );

        Vertex* vertices =
            reinterpret_cast<Vertex*>( glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY ) );

        for( int k = 0; k < TILE_VERTICES; ++k ) {
          vertices[k].read( &is );
        }

        glUnmapBuffer( GL_ARRAY_BUFFER );

# ifndef OZ_GL_COMPATIBLE
        Vertex::setFormat();

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
        glBindVertexArray( 0 );
# endif
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

    landShaderId = library.shaderIndex( "terraLand" );
    waterShaderId = library.shaderIndex( "terraWater" );
    submergedLandShaderId = library.shaderIndex( "submergedTerraLand" );
    submergedWaterShaderId = library.shaderIndex( "submergedTerraWater" );

    log.printEnd( " OK" );
  }

  void Terra::unload()
  {
    if( id != -1 ) {
      glDeleteTextures( 1, &mapTexId );
      glDeleteTextures( 1, &detailTexId );
      glDeleteTextures( 1, &waterTexId );

      glDeleteBuffers( 1, &ibo );
      glDeleteBuffers( TILES * TILES, &vbos[0][0] );
# ifndef OZ_GL_COMPATIBLE
      glDeleteVertexArrays( TILES * TILES, &vaos[0][0] );
# endif

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

      id = -1;
    }
  }

#else // OZ_TOOLS

  void Terra::prebuild( const char* name_ )
  {
    String name       = name_;
    String configFile = "terra/" + name + ".rc";
    String outFile    = "terra/" + name + ".ozcTerra";

    log.println( "Compiling client terrain data to '%s' {", outFile.cstr() );
    log.indent();

    Config terraConfig;
    terraConfig.load( configFile );

    // just to mark them used to prevent unused config variable warning
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

    Buffer buffer( 10 * 1024 * 1024 );
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

    Bitset waterTiles( TILES * TILES );
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

    buffer.write( outFile, os.length() );

    log.unindent();
    log.println( "}" );
  }

#endif // OZ_TOOLS

}
}
