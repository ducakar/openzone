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

const int   Terra::TILE_INDICES  = TILE_QUADS * ( TILE_QUADS + 1 ) * 2 + ( TILE_QUADS - 1 ) * 2;
const int   Terra::TILE_VERTICES = ( TILE_QUADS + 1 ) * ( TILE_QUADS + 1 );

const float Terra::TILE_SIZE     = TILE_QUADS * float( matrix::Terra::Quad::SIZEI );
const float Terra::TILE_INV_SIZE = 1.0f / TILE_SIZE;

const float Terra::WAVE_BIAS_INC = 2.0f * Timer::TICK_TIME;

Terra terra;

Terra::Terra() :
  ibo( 0 ), waterTexId( 0 ), detailTexId( 0 ), mapTexId( 0 ), id( -1 )
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

  span.minX = max( int( ( camera.p.x - frustum.radius + matrix::Terra::DIM ) * TILE_INV_SIZE ), 0 );
  span.minY = max( int( ( camera.p.y - frustum.radius + matrix::Terra::DIM ) * TILE_INV_SIZE ), 0 );
  span.maxX = min( int( ( camera.p.x + frustum.radius + matrix::Terra::DIM ) * TILE_INV_SIZE ), TILES - 1 );
  span.maxY = min( int( ( camera.p.y + frustum.radius + matrix::Terra::DIM ) * TILE_INV_SIZE ), TILES - 1 );

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

  waveBias = Math::fmod( waveBias + WAVE_BIAS_INC, Math::TAU );

  shader.use( waterShaderId );

  glUniform1f( param.oz_Specular, 0.5f );
  glUniform1f( param.oz_WaveBias, waveBias );
  tf.model = Mat44::ID;
  tf.apply();

  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( GL_TEXTURE_2D, waterTexId );
  glActiveTexture( GL_TEXTURE1 );
  glBindTexture( GL_TEXTURE_2D, waterTexId );

  if( camera.p.z >= 0.0f ) {
    glFrontFace( GL_CW );
  }

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

  log.print( "Loading terra '%s' ...", name.cstr() );

  PhysFile file( path );
  if( !file.map() ) {
    throw Exception( "Terra file mmap failed" );
  }

  InputStream is = file.inputStream();

  waterTexId  = context.readTexture( &is, path );
  detailTexId = context.readTexture( &is, path );
  mapTexId    = context.readTexture( &is, path );

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

  file.unmap();

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

}
}
