/*
 *  Shape.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Shape.hpp"
#include "client/Camera.hpp"

#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{

  Shape shape;

  const float Shape::SQRT_3_THIRDS = Math::sqrt( 3.0f ) / 3.0f;
  const float Shape::DIM = 1.0f / 2.0f;

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

  void Shape::rect( float x, float y, float width, float height )
  {
    tf.model = Mat44::translation( Vec3( x + 0.5f, y + 0.5f, 0.0f ) );
    tf.model.scale( Vec3( width - 1.0f, height - 1.0f, 0.0f ) );
    tf.apply();

    glDrawArrays( GL_LINE_LOOP, 4, 4 );
  }

  void Shape::rect( int x, int y, int width, int height )
  {
    rect( float( x ), float( y ), float( width ), float( height ) );
  }

  void Shape::tag( float minX, float minY, float maxX, float maxY )
  {
    tf.model = Mat44::translation( Vec3( minX, minY, 0.0f ) );
    tf.apply();

    glDrawArrays( GL_LINES,  8, 4 );

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
    tf.model = Mat44::translation( bb.p - Point3::ORIGIN );
    tf.model.scale( bb.dim );
    tf.apply();

    glDrawRangeElements( GL_TRIANGLE_STRIP, 28, 35, 22, GL_UNSIGNED_SHORT,
                         reinterpret_cast<const ushort*>( 0 ) + 0 );
  }

  void Shape::wireBox( const AABB& bb )
  {
    tf.model = Mat44::translation( bb.p - Point3::ORIGIN );
    tf.model.scale( bb.dim );
    tf.apply();

    glDrawRangeElements( GL_LINES, 28, 35, 24, GL_UNSIGNED_SHORT,
                         reinterpret_cast<const ushort*>( 0 ) + 22 );
  }

  void Shape::draw( const Particle* part )
  {
    glUniform4f( param.oz_Colour, part->colour.x, part->colour.y, part->colour.z, part->lifeTime );

    tf.apply();

    int index = part->index % MAX_PARTS;
    glDrawArrays( GL_TRIANGLES, 36 + index * 12, 12 );
  }

  void Shape::load()
  {
    DArray<ushort> indices( 46 );
    DArray<Vertex> vertices( 36 + MAX_PARTS * 12 );

    // filled rectangle
    vertices[ 0] = Vertex( Point3( 0.0f, 0.0f, 0.0f ), TexCoord( 0.0f, 1.0f ) );
    vertices[ 1] = Vertex( Point3( 1.0f, 0.0f, 0.0f ), TexCoord( 1.0f, 1.0f ) );
    vertices[ 2] = Vertex( Point3( 0.0f, 1.0f, 0.0f ), TexCoord( 0.0f, 0.0f ) );
    vertices[ 3] = Vertex( Point3( 1.0f, 1.0f, 0.0f ), TexCoord( 1.0f, 0.0f ) );

    // line rectangle
    vertices[ 4] = Vertex( Point3( 0.0f, 0.0f, 0.0f ) );
    vertices[ 5] = Vertex( Point3( 1.0f, 0.0f, 0.0f ) );
    vertices[ 6] = Vertex( Point3( 1.0f, 1.0f, 0.0f ) );
    vertices[ 7] = Vertex( Point3( 0.0f, 1.0f, 0.0f ) );

    // tag box
    vertices[ 8] = Vertex( Point3( -1.5f, -1.5f, 0.0f ) );
    vertices[ 9] = Vertex( Point3( -1.5f, +3.5f, 0.0f ) );
    vertices[10] = Vertex( Point3( -0.5f, -1.5f, 0.0f ) );
    vertices[11] = Vertex( Point3( +3.5f, -1.5f, 0.0f ) );

    vertices[12] = Vertex( Point3( +1.5f, -1.5f, 0.0f ) );
    vertices[13] = Vertex( Point3( -3.5f, -1.5f, 0.0f ) );
    vertices[14] = Vertex( Point3( +1.5f, -0.5f, 0.0f ) );
    vertices[15] = Vertex( Point3( +1.5f, +3.5f, 0.0f ) );

    vertices[16] = Vertex( Point3( +1.5f, +1.5f, 0.0f ) );
    vertices[17] = Vertex( Point3( -3.5f, +1.5f, 0.0f ) );
    vertices[18] = Vertex( Point3( +1.5f, +0.5f, 0.0f ) );
    vertices[19] = Vertex( Point3( +1.5f, -3.5f, 0.0f ) );

    vertices[20] = Vertex( Point3( -1.5f, +1.5f, 0.0f ) );
    vertices[21] = Vertex( Point3( +3.5f, +1.5f, 0.0f ) );
    vertices[22] = Vertex( Point3( -1.5f, +0.5f, 0.0f ) );
    vertices[23] = Vertex( Point3( -1.5f, -3.5f, 0.0f ) );

    // sprite
    vertices[24] = Vertex( Point3( -1.0f, -1.0f, 0.0f ),
                           TexCoord( 0.0f, 0.0f ),
                           Vec3( 0.0f, 0.0f, 1.0f ) );
    vertices[25] = Vertex( Point3( +1.0f, -1.0f, 0.0f ),
                           TexCoord( 1.0f, 0.0f ),
                           Vec3( 0.0f, 0.0f, 1.0f ) );
    vertices[26] = Vertex( Point3( -1.0f, +1.0f, 0.0f ),
                           TexCoord( 0.0f, 1.0f ),
                           Vec3( 0.0f, 0.0f, 1.0f ) );
    vertices[27] = Vertex( Point3( +1.0f, +1.0f, 0.0f ),
                           TexCoord( 1.0f, 1.0f ),
                           Vec3( 0.0f, 0.0f, 1.0f ) );

    // box
    vertices[28] = Vertex( Point3( -1.0f, -1.0f, -1.0f ) );
    vertices[29] = Vertex( Point3( -1.0f, -1.0f, +1.0f ) );
    vertices[30] = Vertex( Point3( -1.0f, +1.0f, -1.0f ) );
    vertices[31] = Vertex( Point3( -1.0f, +1.0f, +1.0f ) );
    vertices[32] = Vertex( Point3( +1.0f, -1.0f, -1.0f ) );
    vertices[33] = Vertex( Point3( +1.0f, -1.0f, +1.0f ) );
    vertices[34] = Vertex( Point3( +1.0f, +1.0f, -1.0f ) );
    vertices[35] = Vertex( Point3( +1.0f, +1.0f, +1.0f ) );

    int  k = 36;
    Vec3 normal;

    for( int i = 0; i < MAX_PARTS; ++i ) {
      Point3 v0 = Point3::ORIGIN + Math::rand() * DIM * Vec3( 0.0f,            0.0f,        1.0f );
      Point3 v1 = Point3::ORIGIN + Math::rand() * DIM * Vec3( 0.0f,            2.0f / 3.0f, 0.0f );
      Point3 v2 = Point3::ORIGIN + Math::rand() * DIM * Vec3( -SQRT_3_THIRDS, -1.0f / 3.0f, 0.0f );
      Point3 v3 = Point3::ORIGIN + Math::rand() * DIM * Vec3(  SQRT_3_THIRDS, -1.0f / 3.0f, 0.0f );

      // fore
      normal = ~( ( v2 - v1 ) ^ ( v0 - v1 ) );

      vertices[k++] = Vertex( v0, TexCoord(), normal );
      vertices[k++] = Vertex( v1, TexCoord(), normal );
      vertices[k++] = Vertex( v2, TexCoord(), normal );

      // left
      normal = ~( ( v1 - v3 ) ^ ( v0 - v3 ) );

      vertices[k++] = Vertex( v0, TexCoord(), normal );
      vertices[k++] = Vertex( v3, TexCoord(), normal );
      vertices[k++] = Vertex( v1, TexCoord(), normal );

      // right
      normal = ~( ( v3 - v2 ) ^ ( v0 - v2 ) );

      vertices[k++] = Vertex( v0, TexCoord(), normal );
      vertices[k++] = Vertex( v2, TexCoord(), normal );
      vertices[k++] = Vertex( v3, TexCoord(), normal );

      // bottom
      normal = ~( ( v3 - v1 ) ^ ( v2 - v1 ) );

      vertices[k++] = Vertex( v1, TexCoord(), normal );
      vertices[k++] = Vertex( v3, TexCoord(), normal );
      vertices[k++] = Vertex( v2, TexCoord(), normal );
    }

    /*
     * Full box (GL_TRIANGLE_STRIP)
     */

    // left
    indices[ 0] = 28 + 0;
    indices[ 1] = 28 + 1;
    indices[ 2] = 28 + 2;
    indices[ 3] = 28 + 3;

    // back
    indices[ 4] = 28 + 6;
    indices[ 5] = 28 + 7;

    // right
    indices[ 6] = 28 + 4;
    indices[ 7] = 28 + 5;

    // front
    indices[ 8] = 28 + 0;
    indices[ 9] = 28 + 1;
    indices[10] = 28 + 1;

    // bottom
    indices[11] = 28 + 0;
    indices[12] = 28 + 0;
    indices[13] = 28 + 2;
    indices[14] = 28 + 4;
    indices[15] = 28 + 6;
    indices[16] = 28 + 6;

    // top
    indices[17] = 28 + 1;
    indices[18] = 28 + 1;
    indices[19] = 28 + 5;
    indices[20] = 28 + 3;
    indices[21] = 28 + 7;

    /*
     * Wire box (GL_LINES)
     */

    // parallel to z
    indices[22] = 28 + 0;
    indices[23] = 28 + 1;
    indices[24] = 28 + 2;
    indices[25] = 28 + 3;
    indices[26] = 28 + 4;
    indices[27] = 28 + 5;
    indices[28] = 28 + 6;
    indices[29] = 28 + 7;

    // parallel to y
    indices[30] = 28 + 0;
    indices[31] = 28 + 2;
    indices[32] = 28 + 1;
    indices[33] = 28 + 3;
    indices[34] = 28 + 4;
    indices[35] = 28 + 6;
    indices[36] = 28 + 5;
    indices[37] = 28 + 7;

    // parallel to x
    indices[38] = 28 + 0;
    indices[39] = 28 + 4;
    indices[40] = 28 + 1;
    indices[41] = 28 + 5;
    indices[42] = 28 + 2;
    indices[43] = 28 + 6;
    indices[44] = 28 + 3;
    indices[45] = 28 + 7;

#ifdef OZ_GL_COMPATIBLE
    vao = 1;
#else
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );
#endif

    glGenBuffers( 1, &ibo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 46 * int( sizeof( ushort ) ), indices, GL_STATIC_DRAW );

    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, vertices.length() * int( sizeof( Vertex ) ), vertices, GL_STATIC_DRAW );

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
