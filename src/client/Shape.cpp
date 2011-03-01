/*
 *  Shape.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Shape.hpp"
#include "client/Camera.hpp"

namespace oz
{
namespace client
{

  Shape shape;

  void Shape::bindVertexArray() const
  {
    glBindVertexArray( vao );
  }

  void Shape::drawSprite( const Point3& p, float dimX, float dimY )
  {
    Mat44 transf;
    glGetFloatv( GL_MODELVIEW_MATRIX, transf );

    transf.w = Quat::ID;
    transf = ~transf;

    glPushMatrix();
    glTranslatef( p.x, p.y, p.z );
    glMultMatrixf( transf );
    glScalef( dimX, 1.0f, dimY );

    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    glPopMatrix();
  }

  void Shape::drawBox( const AABB& bb )
  {
    glPushMatrix();
    glTranslatef( bb.p.x, bb.p.y, bb.p.z );
    glScalef( bb.dim.x, bb.dim.y, bb.dim.z );

    glDrawRangeElements( GL_TRIANGLE_STRIP, 4, 11, 22, GL_UNSIGNED_SHORT,
                         reinterpret_cast<const ushort*>( 0 ) + 0 );

    glPopMatrix();
  }

  void Shape::drawWireBox( const AABB& bb )
  {
    glPushMatrix();
    glTranslatef( bb.p.x, bb.p.y, bb.p.z );
    glScalef( bb.dim.x, bb.dim.y, bb.dim.z );

    glDrawRangeElements( GL_LINES, 4, 11, 24, GL_UNSIGNED_SHORT,
                         reinterpret_cast<const ushort*>( 0 ) + 22 );

    glPopMatrix();
  }

  void Shape::draw( const Particle* part )
  {
    glRotatef( Math::deg( part->rot.y ), 0.0f, 1.0f, 0.0f );
    glRotatef( Math::deg( part->rot.x ), 1.0f, 0.0f, 0.0f );
    glRotatef( Math::deg( part->rot.z ), 0.0f, 0.0f, 1.0f );

    glColor4f( part->colour.x, part->colour.y, part->colour.z, part->lifeTime );

    int index = part->index % MAX_PARTS;
    glDrawArrays( GL_TRIANGLES, 12 + index * 12, 12 );
  }

  void Shape::load()
  {
    DArray<ushort> indices( 46 );
    DArray<Vertex> vertices( 12 + MAX_PARTS * 12 );

    vertices[ 0].set( -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f );
    vertices[ 1].set( +1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f );
    vertices[ 2].set( -1.0f, +1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f );
    vertices[ 3].set( +1.0f, +1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f );

    vertices[ 4].set( -1.0f, -1.0f, -1.0f );
    vertices[ 5].set( -1.0f, -1.0f, +1.0f );
    vertices[ 6].set( -1.0f, +1.0f, -1.0f );
    vertices[ 7].set( -1.0f, +1.0f, +1.0f );
    vertices[ 8].set( +1.0f, -1.0f, -1.0f );
    vertices[ 9].set( +1.0f, -1.0f, +1.0f );
    vertices[10].set( +1.0f, +1.0f, -1.0f );
    vertices[11].set( +1.0f, +1.0f, +1.0f );

    int  k = 12;
    Vec3 normal;

    for( int i = 0; i < MAX_PARTS; ++i ) {
      static const float SQRT_3_THIRDS = Math::sqrt( 3.0f ) / 3.0f;
      static const float DIM = 1.0f / 2.0f;

      Point3 v0 = Point3::ORIGIN + Math::frand() * DIM * Vec3( 0.0f,            0.0f,        1.0f );
      Point3 v1 = Point3::ORIGIN + Math::frand() * DIM * Vec3( 0.0f,            2.0f / 3.0f, 0.0f );
      Point3 v2 = Point3::ORIGIN + Math::frand() * DIM * Vec3( -SQRT_3_THIRDS, -1.0f / 3.0f, 0.0f );
      Point3 v3 = Point3::ORIGIN + Math::frand() * DIM * Vec3(  SQRT_3_THIRDS, -1.0f / 3.0f, 0.0f );

      // fore
      normal = ~( ( v2 - v1 ) ^ ( v0 - v1 ) );

      vertices[k++].set( v0, normal );
      vertices[k++].set( v1, normal );
      vertices[k++].set( v2, normal );

      // left
      normal = ~( ( v1 - v3 ) ^ ( v0 - v3 ) );

      vertices[k++].set( v0, normal );
      vertices[k++].set( v3, normal );
      vertices[k++].set( v1, normal );

      // right
      normal = ~( ( v3 - v2 ) ^ ( v0 - v2 ) );

      vertices[k++].set( v0, normal );
      vertices[k++].set( v2, normal );
      vertices[k++].set( v3, normal );

      // bottom
      normal = ~( ( v3 - v1 ) ^ ( v2 - v1 ) );

      vertices[k++].set( v1, normal );
      vertices[k++].set( v3, normal );
      vertices[k++].set( v2, normal );
    }

    /*
     * Full box (GL_TRIANGLE_STRIP)
     */

    // left
    indices[ 0] =  4;
    indices[ 1] =  5;
    indices[ 2] =  6;
    indices[ 3] =  7;

    // back
    indices[ 4] = 10;
    indices[ 5] = 11;

    // right
    indices[ 6] =  8;
    indices[ 7] =  9;

    // front
    indices[ 8] =  4;
    indices[ 9] =  5;
    indices[10] =  5;

    // bottom
    indices[11] =  4;
    indices[12] =  4;
    indices[13] =  6;
    indices[14] =  8;
    indices[15] = 10;
    indices[16] = 10;

    // top
    indices[17] =  5;
    indices[18] =  5;
    indices[19] =  9;
    indices[20] =  7;
    indices[21] = 11;

    /*
     * Wire box (GL_LINES)
     */

    // parallel to z
    indices[22] =  4;
    indices[23] =  5;
    indices[24] =  6;
    indices[25] =  7;
    indices[26] =  8;
    indices[27] =  9;
    indices[28] = 10;
    indices[29] = 11;

    // parallel to y
    indices[30] =  4;
    indices[31] =  6;
    indices[32] =  5;
    indices[33] =  7;
    indices[34] =  8;
    indices[35] = 10;
    indices[36] =  9;
    indices[37] = 11;

    // parallel to x
    indices[38] =  4;
    indices[39] =  8;
    indices[40] =  5;
    indices[41] =  9;
    indices[42] =  6;
    indices[43] = 10;
    indices[44] =  7;
    indices[45] = 11;

    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    glGenBuffers( 1, &ibo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 46 * sizeof( ushort ), indices, GL_STATIC_DRAW );

    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, vertices.length() * sizeof( Vertex ), vertices, GL_STATIC_DRAW );

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

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
  }

  void Shape::unload()
  {
    glDeleteVertexArrays( 1, &vao );
    glDeleteBuffers( 1, &ibo );
    glDeleteBuffers( 1, &vbo );
  }

}
}
