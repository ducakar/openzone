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
 * @file client/Terra.cc
 */

#include "stable.hh"

#include "client/Terra.hh"

#include "client/Frustum.hh"
#include "client/Context.hh"
#include "client/Colours.hh"
#include "client/OpenGL.hh"

namespace oz
{
namespace client
{

const float Terra::WAVE_BIAS_INC = 2.0f;

Terra terra;

Terra::Terra() :
  ibo( 0 ), id( -1 ), waterTexId( 0 ), detailTexId( 0 ), mapTexId( 0 )
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

  span.minX = max( int( ( camera.p.x - frustum.radius + matrix::Terra::DIM ) / TILE_SIZE ), 0 );
  span.minY = max( int( ( camera.p.y - frustum.radius + matrix::Terra::DIM ) / TILE_SIZE ), 0 );
  span.maxX = min( int( ( camera.p.x + frustum.radius + matrix::Terra::DIM ) / TILE_SIZE ), TILES - 1 );
  span.maxY = min( int( ( camera.p.y + frustum.radius + matrix::Terra::DIM ) / TILE_SIZE ), TILES - 1 );

  shader.use( landShaderId );

  tf.model = Mat44::ID;
  tf.apply();

  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( GL_TEXTURE_2D, detailTexId );
  glActiveTexture( GL_TEXTURE1 );
  glBindTexture( GL_TEXTURE_2D, detailTexId );
  glActiveTexture( GL_TEXTURE2 );
  glBindTexture( GL_TEXTURE_2D, mapTexId );

  OZ_GL_CHECK_ERROR();

  // to match strip triangles with matrix terrain we have to make them clockwise since
  // we draw column-major (strips along y axis) for better cache performance
  glFrontFace( GL_CW );

#ifdef OZ_GL_ES
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
#endif

  for( int i = span.minX; i <= span.maxX; ++i ) {
    for( int j = span.minY; j <= span.maxY; ++j ) {
#ifdef OZ_GL_ES
      glBindBuffer( GL_ARRAY_BUFFER, vbos[i][j] );

      Vertex::setFormat();
#else
      glBindVertexArray( vaos[i][j] );
#endif
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

  waveBias = Math::fmod( waveBias + WAVE_BIAS_INC * Timer::TICK_TIME, Math::TAU );

  shader.use( waterShaderId );

  glUniform1f( param.oz_WaveBias, waveBias );
  tf.model = Mat44::ID;
  tf.apply();

  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( GL_TEXTURE_2D, waterTexId );
  glActiveTexture( GL_TEXTURE1 );
  glBindTexture( GL_TEXTURE_2D, shader.defaultMasks );

  if( camera.p.z >= 0.0f ) {
    glFrontFace( GL_CW );
  }

#ifdef OZ_GL_ES
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
#endif

  for( int i = span.minX; i <= span.maxX; ++i ) {
    for( int j = span.minY; j <= span.maxY; ++j ) {
      if( waterTiles.get( i * TILES + j ) ) {
#ifdef OZ_GL_ES
        glBindBuffer( GL_ARRAY_BUFFER, vbos[i][j] );

        Vertex::setFormat();
#else
        glBindVertexArray( vaos[i][j] );
#endif
        glDrawElements( GL_TRIANGLE_STRIP, TILE_INDICES, GL_UNSIGNED_SHORT, 0 );
      }
    }
  }

  if( camera.p.z >= 0.0f ) {
    glFrontFace( GL_CCW );
  }

  OZ_GL_CHECK_ERROR();
}

void Terra::load()
{
  id = orbis.terra.id;

  const String& name = library.terrae[id].name;
  String path = "terra/" + name + ".ozcTerra";

  Log::print( "Loading terra '%s' ...", name.cstr() );

  PFile file( path );
  if( !file.map() ) {
    throw Exception( "Terra file mmap failed" );
  }

  InputStream is = file.inputStream();

  waterTexId  = context.readTextureLayer( &is, path );
  detailTexId = context.readTextureLayer( &is, path );
  mapTexId    = context.readTextureLayer( &is, path );

#ifndef OZ_GL_ES
  glGenVertexArrays( TILES * TILES, &vaos[0][0] );
#endif
  glGenBuffers( TILES * TILES, &vbos[0][0] );
  glGenBuffers( 1, &ibo );

  int vboSize = TILE_VERTICES * int( sizeof( Vertex ) );
  int iboSize = TILE_INDICES * int( sizeof( ushort ) );

  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, iboSize, is.forward( iboSize ), GL_STATIC_DRAW );

  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

  for( int i = 0; i < TILES; ++i ) {
    for( int j = 0; j < TILES; ++j ) {
#ifdef OZ_GL_ES

      Vertex* vertices = new Vertex[TILE_VERTICES];

      for( int k = 0; k <= TILE_QUADS; ++k ) {
        for( int l = 0; l <= TILE_QUADS; ++l ) {
          int x = i * TILE_QUADS + k;
          int y = j * TILE_QUADS + l;

          Vertex& vertex = vertices[ k * ( TILE_QUADS + 1 ) + l ];

          vertex.pos[0] = orbis.terra.quads[x][y].vertex.x;
          vertex.pos[1] = orbis.terra.quads[x][y].vertex.y;
          vertex.pos[2] = orbis.terra.quads[x][y].vertex.z;

          vertex.texCoord[0] = float( x ) / float( matrix::Terra::VERTS );
          vertex.texCoord[1] = float( y ) / float( matrix::Terra::VERTS );

          vertex.normal[0] = is.readByte();
          vertex.normal[1] = is.readByte();
          vertex.normal[2] = is.readByte();
        }
      }

      glBindBuffer( GL_ARRAY_BUFFER, vbos[i][j] );
      glBufferData( GL_ARRAY_BUFFER, vboSize, vertices, GL_STATIC_DRAW );

      delete[] vertices;

#else

      glBindVertexArray( vaos[i][j] );

      glBindBuffer( GL_ARRAY_BUFFER, vbos[i][j] );
      glBufferData( GL_ARRAY_BUFFER, vboSize, null, GL_STATIC_DRAW );

      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );

      Vertex* vertices = static_cast<Vertex*>( glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY ) );

      for( int k = 0; k <= TILE_QUADS; ++k ) {
        for( int l = 0; l <= TILE_QUADS; ++l ) {
          int x = i * TILE_QUADS + k;
          int y = j * TILE_QUADS + l;

          Vertex& vertex = vertices[ k * ( TILE_QUADS + 1 ) + l ];

          vertex.pos[0] = orbis.terra.quads[x][y].vertex.x;
          vertex.pos[1] = orbis.terra.quads[x][y].vertex.y;
          vertex.pos[2] = orbis.terra.quads[x][y].vertex.z;

          vertex.texCoord[0] = float( x ) / float( matrix::Terra::VERTS );
          vertex.texCoord[1] = float( y ) / float( matrix::Terra::VERTS );

          vertex.normal[0] = is.readByte();
          vertex.normal[1] = is.readByte();
          vertex.normal[2] = is.readByte();
        }
      }

      glUnmapBuffer( GL_ARRAY_BUFFER );

      Vertex::setFormat();

      glBindVertexArray( 0 );

#endif
    }
  }

  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

  waterTiles.clearAll();
  for( int i = 0; i < waterTiles.length(); ++i ) {
    if( is.readBool() ) {
      waterTiles.set( i );
    }
  }

  landShaderId = library.shaderIndex( "terraLand" );
  waterShaderId = library.shaderIndex( "terraWater" );

  liquidFogColour = is.readVec4();

  hard_assert( !is.isAvailable() );

  file.unmap();

  Log::printEnd( " OK" );
}

void Terra::unload()
{
  if( id != -1 ) {
    glDeleteTextures( 1, &mapTexId );
    glDeleteTextures( 1, &detailTexId );
    glDeleteTextures( 1, &waterTexId );

    glDeleteBuffers( 1, &ibo );
    glDeleteBuffers( TILES * TILES, &vbos[0][0] );
#ifndef OZ_GL_ES
    glDeleteVertexArrays( TILES * TILES, &vaos[0][0] );
#endif

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

}
}
