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
 * @file client/Shape.cc
 */

#include "stable.hh"

#include "client/Shape.hh"

#include "client/Camera.hh"
#include "client/Context.hh"
#include "client/Shader.hh"
#include "client/OpenGL.hh"

namespace oz
{
namespace client
{

Shape shape;

const Vertex Shape::VERTICES[40] = {
  // filled rectangle
  { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }, {}, {}, {}, {} },
  { { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }, {}, {}, {}, {} },
  { { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }, {}, {}, {}, {} },
  { { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }, {}, {}, {}, {} },

  // filled rectangle (inverted)
  { { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }, {}, {}, {}, {} },
  { { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, {}, {}, {}, {} },
  { { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }, {}, {}, {}, {} },
  { { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f }, {}, {}, {}, {} },

  // line loop rectangle
  { { 0.0f, 0.0f, 0.0f }, {}, {}, {}, {}, {} },
  { { 1.0f, 0.0f, 0.0f }, {}, {}, {}, {}, {} },
  { { 1.0f, 1.0f, 0.0f }, {}, {}, {}, {}, {} },
  { { 0.0f, 1.0f, 0.0f }, {}, {}, {}, {}, {} },

  // tag box
  { { -1.5f, -1.5f, 0.0f }, {}, {}, {}, {}, {} },
  { { -1.5f, +3.5f, 0.0f }, {}, {}, {}, {}, {} },
  { { -0.5f, -1.5f, 0.0f }, {}, {}, {}, {}, {} },
  { { +3.5f, -1.5f, 0.0f }, {}, {}, {}, {}, {} },

  { { +1.5f, -1.5f, 0.0f }, {}, {}, {}, {}, {} },
  { { -3.5f, -1.5f, 0.0f }, {}, {}, {}, {}, {} },
  { { +1.5f, -0.5f, 0.0f }, {}, {}, {}, {}, {} },
  { { +1.5f, +3.5f, 0.0f }, {}, {}, {}, {}, {} },

  { { +1.5f, +1.5f, 0.0f }, {}, {}, {}, {}, {} },
  { { -3.5f, +1.5f, 0.0f }, {}, {}, {}, {}, {} },
  { { +1.5f, +0.5f, 0.0f }, {}, {}, {}, {}, {} },
  { { +1.5f, -3.5f, 0.0f }, {}, {}, {}, {}, {} },

  { { -1.5f, +1.5f, 0.0f }, {}, {}, {}, {}, {} },
  { { +3.5f, +1.5f, 0.0f }, {}, {}, {}, {}, {} },
  { { -1.5f, +0.5f, 0.0f }, {}, {}, {}, {}, {} },
  { { -1.5f, -3.5f, 0.0f }, {}, {}, {}, {}, {} },

  // sprite
  { { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f }, {}, { 0.0f, 0.0f, 1.0f }, {}, {} },
  { { +1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f }, {}, { 0.0f, 0.0f, 1.0f }, {}, {} },
  { { -1.0f, +1.0f, 0.0f }, { 0.0f, 1.0f }, {}, { 0.0f, 0.0f, 1.0f }, {}, {} },
  { { +1.0f, +1.0f, 0.0f }, { 1.0f, 1.0f }, {}, { 0.0f, 0.0f, 1.0f }, {}, {} },

  // box (base index 32)
  { { -1.0f, -1.0f, -1.0f }, {}, {}, {}, {}, {} },
  { { -1.0f, -1.0f, +1.0f }, {}, {}, {}, {}, {} },
  { { -1.0f, +1.0f, -1.0f }, {}, {}, {}, {}, {} },
  { { -1.0f, +1.0f, +1.0f }, {}, {}, {}, {}, {} },
  { { +1.0f, -1.0f, -1.0f }, {}, {}, {}, {}, {} },
  { { +1.0f, -1.0f, +1.0f }, {}, {}, {}, {}, {} },
  { { +1.0f, +1.0f, -1.0f }, {}, {}, {}, {}, {} },
  { { +1.0f, +1.0f, +1.0f }, {}, {}, {}, {}, {} }
};

const ushort Shape::INDICES[46] = {
  /*
   * Full box (GL_TRIANGLE_STRIP)
   */

  // left
  32 + 0,
  32 + 1,
  32 + 2,
  32 + 3,
  // back
  32 + 6,
  32 + 7,
  // right
  32 + 4,
  32 + 5,
  // front
  32 + 0,
  32 + 1,
  32 + 1,
  // bottom
  32 + 0,
  32 + 0,
  32 + 2,
  32 + 4,
  32 + 6,
  32 + 6,
  // top
  32 + 1,
  32 + 1,
  32 + 5,
  32 + 3,
  32 + 7,

  /*
   * Wire box (GL_LINES)
   */

  // parallel to z
  32 + 0,
  32 + 1,
  32 + 2,
  32 + 3,
  32 + 4,
  32 + 5,
  32 + 6,
  32 + 7,
  // parallel to y
  32 + 0,
  32 + 2,
  32 + 1,
  32 + 3,
  32 + 4,
  32 + 6,
  32 + 5,
  32 + 7,
  // parallel to x
  32 + 0,
  32 + 4,
  32 + 1,
  32 + 5,
  32 + 2,
  32 + 6,
  32 + 3,
  32 + 7
};

Shape::Shape() : vao( 0 ), vbo( 0 ), ibo( 0 )
{}

void Shape::bindVertexArray() const
{
#ifdef OZ_GL_COMPATIBLE
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
  Vertex::setFormat();
#else
  glBindVertexArray( vao );
#endif
}

void Shape::fill( float x, float y, float width, float height )
{
  tf.model = Mat44::translation( Vec3( x, y, 0.0f ) );
  tf.model.scale( Vec3( width, height, 0.0f ) );
  tf.apply();

  glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
}

void Shape::fill( int x, int y, int width, int height )
{
  fill( float( x ), float( y ), float( width ), float( height ) );
}

void Shape::fillInv( float x, float y, float width, float height )
{
  tf.model = Mat44::translation( Vec3( x, y, 0.0f ) );
  tf.model.scale( Vec3( width, height, 0.0f ) );
  tf.apply();

  glDrawArrays( GL_TRIANGLE_STRIP, 4, 4 );
}

void Shape::fillInv( int x, int y, int width, int height )
{
  fillInv( float( x ), float( y ), float( width ), float( height ) );
}

void Shape::rect( float x, float y, float width, float height )
{
  tf.model = Mat44::translation( Vec3( x + 0.5f, y + 0.5f, 0.0f ) );
  tf.model.scale( Vec3( width - 1.0f, height - 1.0f, 0.0f ) );
  tf.apply();

  glDrawArrays( GL_LINE_LOOP, 8, 4 );
}

void Shape::rect( int x, int y, int width, int height )
{
  rect( float( x ), float( y ), float( width ), float( height ) );
}

void Shape::tag( float minX, float minY, float maxX, float maxY )
{
  tf.model = Mat44::translation( Vec3( minX, minY, 0.0f ) );
  tf.apply();

  glDrawArrays( GL_LINES, 12, 4 );

  tf.model = Mat44::translation( Vec3( maxX, minY, 0.0f ) );
  tf.apply();

  glDrawArrays( GL_LINES, 16, 4 );

  tf.model = Mat44::translation( Vec3( maxX, maxY, 0.0f ) );
  tf.apply();

  glDrawArrays( GL_LINES, 20, 4 );

  tf.model = Mat44::translation( Vec3( minX, maxY, 0.0f ) );
  tf.apply();

  glDrawArrays( GL_LINES, 24, 4 );
}

void Shape::quad( float dimX, float dimY )
{
  tf.model.scale( Vec3( dimX, 1.0f, dimY ) );
  tf.apply();

  glDrawArrays( GL_TRIANGLE_STRIP, 28, 4 );
}

void Shape::box( const AABB& bb )
{
  tf.model = Mat44::translation( bb.p - Point3::ORIGIN );
  tf.model.scale( bb.dim );
  tf.apply();

  glDrawRangeElements( GL_TRIANGLE_STRIP, 32, 39, 22, GL_UNSIGNED_SHORT,
                       reinterpret_cast<const ushort*>( 0 ) + 0 );
}

void Shape::wireBox( const AABB& bb )
{
  tf.model = Mat44::translation( bb.p - Point3::ORIGIN );
  tf.model.scale( bb.dim );
  tf.apply();

  glDrawRangeElements( GL_LINES, 32, 39, 24, GL_UNSIGNED_SHORT,
                       reinterpret_cast<const ushort*>( 0 ) + 22 );
}

void Shape::load()
{
#ifdef OZ_GL_COMPATIBLE
  vao = 1;
#else
  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );
#endif

  glGenBuffers( 1, &vbo );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, sizeof( VERTICES ), VERTICES, GL_STATIC_DRAW );

  glGenBuffers( 1, &ibo );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( INDICES ), INDICES, GL_STATIC_DRAW );

#ifndef OZ_GL_COMPATIBLE
  Vertex::setFormat();

  glBindVertexArray( 0 );
#endif

  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void Shape::unload()
{
  if( vao != 0 ) {
    glDeleteBuffers( 1, &ibo );
    glDeleteBuffers( 1, &vbo );
#ifndef OZ_GL_COMPATIBLE
    glDeleteVertexArrays( 1, &vao );
#endif

    ibo = 0;
    vbo = 0;
    vao = 0;
  }
}

}
}
