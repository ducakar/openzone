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
#include "client/Water.hpp"

namespace oz
{
namespace client
{

  const int Terra::TILE_INDICES =
      Terra::TILE_QUADS * ( Terra::TILE_QUADS + 1 ) * 2 + ( Terra::TILE_QUADS - 1 ) * 2;

  const int Terra::TILE_VERTICES   = ( Terra::TILE_QUADS + 1 ) * ( Terra::TILE_QUADS + 1 );

  const float Terra::TILE_SIZE     = Terra::TILE_QUADS * oz::Terra::Quad::SIZE;
  const float Terra::TILE_INV_SIZE = 1.0f / Terra::TILE_SIZE;

  const float Terra::DETAIL_SCALE  = 4.00f;
  const float Terra::WATER_SCALE   = 0.25f;

  Terra terra;

  void Terra::prebuild()
  {
    String configFile = "terra/" + orbis.terra.name + ".rc";
    String outFile    = "terra/" + orbis.terra.name + ".ozcTerra";

    log.print( "Saving client terrain data to '%s' ...", outFile.cstr() );

    Config terraConfig;
    terraConfig.load( configFile );

    // just to mark them used for OZ_VERBOSE_CONFIG
    terraConfig.get( "step", 0.5f );
    terraConfig.get( "bias", 0.0f );

    String terraDir      = "terra/";
    String waterTexture  = terraDir + terraConfig.get( "waterTexture", "" );
    String detailTexture = terraDir + terraConfig.get( "detailTexture", "" );
    String mapTexture    = terraDir + terraConfig.get( "mapTexture", "" );

    int size = 0;
    size += waterTexture.length() + 1;
    size += detailTexture.length() + 1;
    size += mapTexture.length() + 1;
    size += int( TILE_INDICES * sizeof( ushort ) );
    size += int( TILES * TILES * ( TILE_VERTICES + 16 ) * sizeof( Vertex ) );

    Buffer buffer( size );
    OutputStream os = buffer.outputStream();

    os.writeString( waterTexture );
    os.writeString( detailTexture );
    os.writeString( mapTexture );

    // generate index buffer
    int index = 0;
    for( int x = 0; x < TILE_QUADS; ++x ) {
      if( x != 0 ) {
        os.writeShort( ushort( index + TILE_QUADS + 1 ) );
      }
      for( int y = 0; y <= TILE_QUADS; ++y ) {
        os.writeShort( ushort( index + TILE_QUADS + 1 ) );
        os.writeShort( ushort( index ) );
        ++index;
      }
      if( x != TILE_QUADS - 1 ) {
        os.writeShort( ushort( index - 1 ) );
      }
    }

    // generate vertex buffers
    Point3 pos;
    Vec3   normal;
    Vertex vertex;

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

            vertex.set( pos,
                        ~normal,
                        TexCoord( float( x ) / float( oz::Terra::VERTS ),
                                  float( y ) / float( oz::Terra::VERTS ) ) );
            vertex.write( &os );
          }
        }

        int x = i * TILE_QUADS;
        int y = j * TILE_QUADS;

        float minX = quads[x][y].vertex.x;
        float minY = quads[x][y].vertex.y;
        float maxX = quads[x + TILE_QUADS][y + TILE_QUADS].vertex.x;
        float maxY = quads[x + TILE_QUADS][y + TILE_QUADS].vertex.y;

        // front, blend 1
        normal = Vec3( 0.0f, 0.0f, 1.0f );

        vertex.set( Point3( minX, minY, 0.0f ),
                    normal,
                    TexCoord( 0.0f,
                              0.0f ) );
        vertex.write( &os );

        vertex.set( Point3( maxX, minY, 0.0f ),
                    normal,
                    TexCoord( TILE_SIZE * WATER_SCALE,
                              0.0f ) );
        vertex.write( &os );

        vertex.set( Point3( minX, maxY, 0.0f ),
                    normal,
                    TexCoord( 0.0f,
                              TILE_SIZE * WATER_SCALE ) );
        vertex.write( &os );

        vertex.set( Point3( maxX, maxY, 0.0f ),
                    normal,
                    TexCoord( TILE_SIZE * WATER_SCALE,
                              TILE_SIZE * WATER_SCALE ) );
        vertex.write( &os );

        // front, blend 2
        vertex.set( Point3( minX, minY, 0.0f ),
                    normal,
                    TexCoord( Water::TEX_BIAS,
                              Water::TEX_BIAS ) );
        vertex.write( &os );

        vertex.set( Point3( maxX, minY, 0.0f ),
                    normal,
                    TexCoord( TILE_SIZE * WATER_SCALE + Water::TEX_BIAS,
                              Water::TEX_BIAS ) );
        vertex.write( &os );

        vertex.set( Point3( minX, maxY, 0.0f ),
                    normal,
                    TexCoord( Water::TEX_BIAS,
                              TILE_SIZE * WATER_SCALE + Water::TEX_BIAS ) );
        vertex.write( &os );

        vertex.set( Point3( maxX, maxY, 0.0f ),
                    normal,
                    TexCoord( TILE_SIZE * WATER_SCALE + Water::TEX_BIAS,
                              TILE_SIZE * WATER_SCALE + Water::TEX_BIAS ) );
        vertex.write( &os );

        // back, blend 1
        normal = Vec3( 0.0f, 0.0f, -1.0f );

        vertex.set( Point3( minX, minY, 0.0f ),
                    normal,
                    TexCoord( 0.0f,
                              0.0f ) );
        vertex.write( &os );

        vertex.set( Point3( minX, maxY, 0.0f ),
                    normal,
                    TexCoord( 0.0f,
                              TILE_SIZE * WATER_SCALE ) );
        vertex.write( &os );

        vertex.set( Point3( maxX, minY, 0.0f ),
                    normal,
                    TexCoord( TILE_SIZE * WATER_SCALE,
                              0.0f ) );
        vertex.write( &os );

        vertex.set( Point3( maxX, maxY, 0.0f ),
                    normal,
                    TexCoord( TILE_SIZE * WATER_SCALE,
                              TILE_SIZE * WATER_SCALE ) );
        vertex.write( &os );

        // front, blend 2
        vertex.set( Point3( minX, minY, 0.0f ),
                    normal,
                    TexCoord( Water::TEX_BIAS,
                              Water::TEX_BIAS ) );
        vertex.write( &os );

        vertex.set( Point3( minX, maxY, 0.0f ),
                    normal,
                    TexCoord( Water::TEX_BIAS,
                              TILE_SIZE * WATER_SCALE + Water::TEX_BIAS ) );
        vertex.write( &os );

        vertex.set( Point3( maxX, minY, 0.0f ),
                    normal,
                    TexCoord( TILE_SIZE * WATER_SCALE + Water::TEX_BIAS,
                              Water::TEX_BIAS ) );
        vertex.write( &os );

        vertex.set( Point3( maxX, maxY, 0.0f ),
                    normal,
                    TexCoord( TILE_SIZE * WATER_SCALE + Water::TEX_BIAS,
                              TILE_SIZE * WATER_SCALE + Water::TEX_BIAS ) );
        vertex.write( &os );
      }
    }

    hard_assert( !os.isAvailable() );
    buffer.write( outFile );

    log.printEnd( " OK" );
  }

  void Terra::load()
  {
    String path = "terra/" + orbis.terra.name + ".ozcTerra";

    log.println( "Loading terrain '%s' {", path.cstr() );
    log.indent();

    ushort* indices  = new ushort[TILE_INDICES];
    Vertex* vertices = new Vertex[TILE_VERTICES + 16];

    Buffer buffer;
    buffer.read( path );

    InputStream is = buffer.inputStream();

    String waterTexture  = is.readString();
    String detailTexture = is.readString();
    String mapTexture    = is.readString();

    glGenVertexArrays( TILES * TILES, &vaos[0][0] );
    glGenBuffers( TILES * TILES, &vbos[0][0] );
    glGenBuffers( 1, &ibo );

    for( int i = 0; i < TILE_INDICES; ++i ) {
      indices[i] = is.readShort();
    }

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, TILE_INDICES * sizeof( ushort ), indices,
                  GL_STATIC_DRAW );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    for( int i = 0; i < TILES; ++i ) {
      for( int j = 0; j < TILES; ++j ) {
        for( int k = 0; k < TILE_VERTICES + 16; ++k ) {
          vertices[k].read( &is );
        }

        glBindVertexArray( vaos[i][j] );

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );

        glBindBuffer( GL_ARRAY_BUFFER, vbos[i][j] );
        glBufferData( GL_ARRAY_BUFFER, ( TILE_VERTICES + 16 ) * sizeof( Vertex ), vertices,
                      GL_STATIC_DRAW );

        glEnableClientState( GL_VERTEX_ARRAY );
        glVertexPointer( 3, GL_FLOAT, sizeof( Vertex ),
                        reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, pos ) );

        glEnableClientState( GL_NORMAL_ARRAY );
        glNormalPointer( GL_FLOAT, sizeof( Vertex ),
                        reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, normal ) );

        glClientActiveTexture( GL_TEXTURE0 );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ),
                          reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, texCoord ) );

        glClientActiveTexture( GL_TEXTURE1 );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ),
                          reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, texCoord ) );

        glClientActiveTexture( GL_TEXTURE2 );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ),
                          reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, texCoord ) );

        glBindVertexArray( 0 );
      }
    }

    delete[] indices;
    delete[] vertices;

    waterTexId  = context.loadTexture( waterTexture );
    detailTexId = context.loadTexture( detailTexture );
    mapTexId    = context.loadTexture( mapTexture );

    hard_assert( !is.isAvailable() );

    log.unindent();
    log.println( "}" );
  }

  void Terra::unload()
  {
    glDeleteTextures( 1, &mapTexId );
    glDeleteTextures( 1, &detailTexId );
    glDeleteTextures( 1, &waterTexId );

    glDeleteBuffers( 1, &ibo );
    glDeleteBuffers( TILES * TILES, &vbos[0][0] );
    glDeleteVertexArrays( TILES * TILES, &vaos[0][0] );
  }

  void Terra::draw()
  {
    span.minX = max( int( ( camera.p.x - frustum.radius + oz::Terra::DIM ) * TILE_INV_SIZE ), 0 );
    span.minY = max( int( ( camera.p.y - frustum.radius + oz::Terra::DIM ) * TILE_INV_SIZE ), 0 );
    span.maxX = min( int( ( camera.p.x + frustum.radius + oz::Terra::DIM ) * TILE_INV_SIZE ), TILES - 1 );
    span.maxY = min( int( ( camera.p.y + frustum.radius + oz::Terra::DIM ) * TILE_INV_SIZE ), TILES - 1 );

    context.bindTextures( detailTexId, detailTexId, mapTexId );

    glMatrixMode( GL_TEXTURE );

    glActiveTexture( GL_TEXTURE0 );
    glScalef( float( oz::Terra::QUADS ) * 0.5f, float( oz::Terra::QUADS ) * 0.5f, 0.0f );

    glActiveTexture( GL_TEXTURE1 );
    glScalef( float( oz::Terra::QUADS ) * 4.0f, float( oz::Terra::QUADS ) * 4.0f, 0.0f );

    // to match strip triangles with matrix terrain we have to make them clockwise since
    // we draw column-major (strips along y axis) for better cache performance
    glFrontFace( GL_CW );

    for( int i = span.minX; i <= span.maxX; ++i ) {
      for( int j = span.minY; j <= span.maxY; ++j ) {
        glBindVertexArray( vaos[i][j] );
        glDrawElements( GL_TRIANGLE_STRIP, TILE_INDICES, GL_UNSIGNED_SHORT, 0 );
      }
    }

    glLoadIdentity();
    glActiveTexture( GL_TEXTURE0 );
    glLoadIdentity();

    glMatrixMode( GL_MODELVIEW );

    glFrontFace( GL_CCW );

    hard_assert( glGetError() == GL_NO_ERROR );
  }

  void Terra::drawWater()
  {
    int sideVertices = 0;

    if( camera.p.z < 0.0f ) {
      span.minX = max( int( ( camera.p.x - frustum.radius + oz::Terra::DIM ) * TILE_INV_SIZE ), 0 );
      span.minY = max( int( ( camera.p.y - frustum.radius + oz::Terra::DIM ) * TILE_INV_SIZE ), 0 );
      span.maxX = min( int( ( camera.p.x + frustum.radius + oz::Terra::DIM ) * TILE_INV_SIZE ), TILES - 1 );
      span.maxY = min( int( ( camera.p.y + frustum.radius + oz::Terra::DIM ) * TILE_INV_SIZE ), TILES - 1 );

      sideVertices = 8;
    }

    glEnable( GL_BLEND );

    context.bindTextures( waterTexId );

    for( int i = span.minX; i <= span.maxX; ++i ) {
      for( int j = span.minY; j <= span.maxY; ++j ) {
        glBindVertexArray( vaos[i][j] );

        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colours::waterBlend1 );
        glDrawArrays( GL_TRIANGLE_STRIP, TILE_VERTICES + sideVertices, 4 );

        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Colours::waterBlend2 );
        glDrawArrays( GL_TRIANGLE_STRIP, TILE_VERTICES + sideVertices + 4, 4 );
      }
    }

    glDisable( GL_BLEND );

    hard_assert( glGetError() == GL_NO_ERROR );
  }

}
}
