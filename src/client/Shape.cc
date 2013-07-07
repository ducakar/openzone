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
 * @file client/Shape.cc
 */

#include <client/Shape.hh>

#include <client/Camera.hh>
#include <client/Context.hh>

#include <ozDynamics/ozDynamics.hh>

namespace oz
{
namespace client
{

const Shape::Vertex Shape::VERTICES[40] = {
  // filled rectangle
  { { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
  { { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
  { { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
  { { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },

  // line loop rectangle
  { { 0.0f, 0.0f, 0.0f }, {}, {} },
  { { 1.0f, 0.0f, 0.0f }, {}, {} },
  { { 1.0f, 1.0f, 0.0f }, {}, {} },
  { { 0.0f, 1.0f, 0.0f }, {}, {} },

  // tag box
  { { -1.5f, -1.5f, 0.0f }, {}, {} },
  { { -1.5f, +3.5f, 0.0f }, {}, {} },
  { { -0.5f, -1.5f, 0.0f }, {}, {} },
  { { +3.5f, -1.5f, 0.0f }, {}, {} },

  { { +1.5f, -1.5f, 0.0f }, {}, {} },
  { { -3.5f, -1.5f, 0.0f }, {}, {} },
  { { +1.5f, -0.5f, 0.0f }, {}, {} },
  { { +1.5f, +3.5f, 0.0f }, {}, {} },

  { { +1.5f, +1.5f, 0.0f }, {}, {} },
  { { -3.5f, +1.5f, 0.0f }, {}, {} },
  { { +1.5f, +0.5f, 0.0f }, {}, {} },
  { { +1.5f, -3.5f, 0.0f }, {}, {} },

  { { -1.5f, +1.5f, 0.0f }, {}, {} },
  { { +3.5f, +1.5f, 0.0f }, {}, {} },
  { { -1.5f, +0.5f, 0.0f }, {}, {} },
  { { -1.5f, -3.5f, 0.0f }, {}, {} },

  // sprite
  { { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
  { { +1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
  { { -1.0f, +1.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
  { { +1.0f, +1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },

  // box (base index 32)
  { { -1.0f, -1.0f, -1.0f }, {}, {} },
  { { -1.0f, -1.0f, +1.0f }, {}, {} },
  { { -1.0f, +1.0f, -1.0f }, {}, {} },
  { { -1.0f, +1.0f, +1.0f }, {}, {} },
  { { +1.0f, -1.0f, -1.0f }, {}, {} },
  { { +1.0f, -1.0f, +1.0f }, {}, {} },
  { { +1.0f, +1.0f, -1.0f }, {}, {} },
  { { +1.0f, +1.0f, +1.0f }, {}, {} }
};

const ushort Shape::INDICES[46] = {
  /*
   * Full box (GL_TRIANGLE_STRIP)
   */

  // left
  28 + 0,
  28 + 1,
  28 + 2,
  28 + 3,
  // back
  28 + 6,
  28 + 7,
  // right
  28 + 4,
  28 + 5,
  // front
  28 + 0,
  28 + 1,
  28 + 1,
  // bottom
  28 + 0,
  28 + 0,
  28 + 2,
  28 + 4,
  28 + 6,
  28 + 6,
  // top
  28 + 1,
  28 + 1,
  28 + 5,
  28 + 3,
  28 + 7,

  /*
   * Wire box (GL_LINES)
   */

  // parallel to z
  28 + 0,
  28 + 1,
  28 + 2,
  28 + 3,
  28 + 4,
  28 + 5,
  28 + 6,
  28 + 7,
  // parallel to y
  28 + 0,
  28 + 2,
  28 + 1,
  28 + 3,
  28 + 4,
  28 + 6,
  28 + 5,
  28 + 7,
  // parallel to x
  28 + 0,
  28 + 4,
  28 + 1,
  28 + 5,
  28 + 2,
  28 + 6,
  28 + 3,
  28 + 7
};

Shape::Shape() :
  vbo( 0 ), ibo( 0 )
{}

void Shape::bind() const
{
  glBindBuffer( GL_ARRAY_BUFFER, vbo );

  glEnableVertexAttribArray( Attrib::POSITION );
  glVertexAttribPointer( Attrib::POSITION, 3, GL_FLOAT, GL_FALSE, int( sizeof( Vertex ) ),
                         static_cast<char*>( nullptr ) + offsetof( Vertex, pos ) );

  glEnableVertexAttribArray( Attrib::TEXCOORD );
  glVertexAttribPointer( Attrib::TEXCOORD, 2, GL_FLOAT, GL_FALSE, int( sizeof( Vertex ) ),
                         static_cast<char*>( nullptr ) + offsetof( Vertex, texCoord ) );

  glEnableVertexAttribArray( Attrib::NORMAL );
  glVertexAttribPointer( Attrib::NORMAL, 3, GL_FLOAT, GL_FALSE, int( sizeof( Vertex ) ),
                         static_cast<char*>( nullptr ) + offsetof( Vertex, normal ) );

  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
}

void Shape::unbind() const
{
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void Shape::colour( const Vec4& c )
{
  glUniformMatrix4fv( uniform.colourTransform, 1, GL_FALSE, Mat44::scaling( c ) );
}

void Shape::colour( float r, float g, float b, float a )
{
  glUniformMatrix4fv( uniform.colourTransform, 1, GL_FALSE,
                      Mat44(    r, 0.0f, 0.0f, 0.0f,
                             0.0f,    g, 0.0f, 0.0f,
                             0.0f, 0.0f,    b, 0.0f,
                             0.0f, 0.0f, 0.0f,    a ) );
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

void Shape::rect( float x, float y, float width, float height )
{
  tf.model = Mat44::translation( Vec3( x + 0.5f, y + 0.5f, 0.0f ) );
  tf.model.scale( Vec3( width - 1.0f, height - 1.0f, 0.0f ) );
  tf.apply();

  glDrawArrays( GL_LINE_LOOP, 4, 4 );
}

void Shape::rect( int x, int y, int width, int height )
{
  x = x < 0 ? camera.width  + x : x;
  y = y < 0 ? camera.height + y : y;

  rect( float( x ), float( y ), float( width ), float( height ) );
}

void Shape::tag( float minX, float minY, float maxX, float maxY )
{
  tf.model = Mat44::translation( Vec3( minX, minY, 0.0f ) );
  tf.apply();

  glDrawArrays( GL_LINES, 8, 4 );

  tf.model = Mat44::translation( Vec3( maxX, minY, 0.0f ) );
  tf.apply();

  glDrawArrays( GL_LINES, 12, 4 );

  tf.model = Mat44::translation( Vec3( maxX, maxY, 0.0f ) );
  tf.apply();

  glDrawArrays( GL_LINES, 16, 4 );

  tf.model = Mat44::translation( Vec3( minX, maxY, 0.0f ) );
  tf.apply();

  glDrawArrays( GL_LINES, 20, 4 );
}

void Shape::quad( float dimX, float dimY )
{
  tf.model.scale( Vec3( dimX, 1.0f, dimY ) );
  tf.apply();

  glDrawArrays( GL_TRIANGLE_STRIP, 24, 4 );
}

void Shape::box( const AABB& bb )
{
  tf.model = Mat44::translation( bb.p - Point::ORIGIN );
  tf.model.scale( bb.dim );
  tf.apply();

  glDrawElements( GL_TRIANGLE_STRIP, 22, GL_UNSIGNED_SHORT, static_cast<ushort*>( nullptr ) + 0 );
}

void Shape::wireBox( const AABB& bb )
{
  tf.model = Mat44::translation( bb.p - Point::ORIGIN );
  tf.model.scale( bb.dim );
  tf.apply();

  glDrawElements( GL_LINES, 24, GL_UNSIGNED_SHORT, static_cast<ushort*>( nullptr ) + 22 );
}

#ifdef OZ_DYNAMICS

void Shape::object( const Point& pos, const Mat33& rot, const void* shape_ )
{
  const oz::Shape* shape = static_cast<const oz::Shape*>( shape_ );

  if( shape->type == oz::Shape::BOX ) {
    const Box* box = static_cast<const Box*>( shape );

    tf.model = Mat44::translation( pos - Point::ORIGIN );
    tf.model = tf.model * Mat44( rot );
    tf.model.scale( box->ext );
    tf.apply();

    colour( 0.5f, 0.5f, 0.5f, 1.0f );
    glDrawElements( GL_TRIANGLE_STRIP, 22, GL_UNSIGNED_SHORT, static_cast<ushort*>( nullptr ) + 0 );
    colour( 1.0f, 0.0f, 0.0f, 1.0f );
    glDrawElements( GL_LINES, 24, GL_UNSIGNED_SHORT, static_cast<ushort*>( nullptr ) + 22 );
  }
  else if( shape->type == oz::Shape::CAPSULE ) {
    const Capsule* capsule = static_cast<const Capsule*>( shape );

    tf.model = Mat44::translation( pos - Point::ORIGIN );
    tf.model = tf.model * Mat44( rot );
    tf.model.scale( Vec3( capsule->radius, capsule->radius, capsule->ext + capsule->radius ) );
    tf.apply();

    colour( 0.5f, 0.5f, 0.5f, 1.0f );
    glDrawElements( GL_TRIANGLE_STRIP, 22, GL_UNSIGNED_SHORT, static_cast<ushort*>( nullptr ) + 0 );
    colour( 1.0f, 0.0f, 0.0f, 1.0f );
    glDrawElements( GL_LINES, 24, GL_UNSIGNED_SHORT, static_cast<ushort*>( nullptr ) + 22 );

  }
  else if( shape->type == oz::Shape::COMPOUND ) {
    const Compound* compound = static_cast<const Compound*>( shape );

    foreach( child, compound->citer() ) {
      object( pos + rot * child->off, rot * child->rot, child->shape );
    }
  }
}

#endif

void Shape::init()
{
  glGenBuffers( 1, &vbo );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, sizeof( VERTICES ), VERTICES, GL_STATIC_DRAW );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );

  glGenBuffers( 1, &ibo );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( INDICES ), INDICES, GL_STATIC_DRAW );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void Shape::destroy()
{
  if( vbo != 0 ) {
    glDeleteBuffers( 1, &ibo );
    glDeleteBuffers( 1, &vbo );

    ibo = 0;
    vbo = 0;
  }
}

Shape shape;

}
}
