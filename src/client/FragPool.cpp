/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file client/FragPool.cpp
 */

#include "stable.hpp"

#include "client/FragPool.hpp"

#include "client/Shader.hpp"
#include "client/Mesh.hpp"
#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{

FragPool fragPool;

const float FragPool::FRAG_RADIUS = 1.0f;

const float FragPool::SQRT_3_THIRDS = Math::sqrt( 3.0f ) / 3.0f;
const float FragPool::DIM = 1.0f / 2.0f;

FragPool::FragPool() : vao( 0 ), vbo( 0 )
{}

void FragPool::bindVertexArray() const
{
#ifdef OZ_GL_COMPATIBLE
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  Vertex::setFormat();
#else
  glBindVertexArray( vao );
#endif
}

void FragPool::draw( const Frag* frag )
{
  glUniform4f( param.oz_Colour, frag->colour.x, frag->colour.y, frag->colour.z, 1.0f );

  tf.model = Mat44::translation( frag->p - Point3::ORIGIN );
  tf.apply();

  int index = frag->index % MAX_FRAGS;
  glDrawArrays( GL_TRIANGLES, index * 12, 12 );
}

void FragPool::load()
{
  log.println( "Loading FragPool {" );
  log.indent();

  DArray<Vertex> vertices( MAX_FRAGS * 12 );

  int  k = 0;
  Vec3 normal;

  for( int i = 0; i < MAX_FRAGS; ++i ) {
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

#ifdef OZ_GL_COMPATIBLE
  vao = 1;
#else
  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );
#endif

  glGenBuffers( 1, &vbo );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, vertices.length() * int( sizeof( Vertex ) ), vertices,
                GL_STATIC_DRAW );

#ifndef OZ_GL_COMPATIBLE
  Vertex::setFormat();

  glBindVertexArray( 0 );
#endif

  glBindBuffer( GL_ARRAY_BUFFER, 0 );

  log.unindent();
  log.println( "}" );
}

void FragPool::unload()
{
  if( vao != 0 ) {
    log.print( "Unloading FragPool ..." );

    glDeleteBuffers( 1, &vbo );
#ifndef OZ_GL_COMPATIBLE
    glDeleteVertexArrays( 1, &vao );
#endif

    vbo = 0;
    vao = 0;

    log.printEnd( " OK" );
  }
}

}
}
