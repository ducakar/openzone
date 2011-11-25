/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
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
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file client/Mesh.cpp
 */

#include "stable.hpp"

#include "client/Mesh.hpp"

#include "client/Colours.hpp"
#include "client/Context.hpp"
#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{

#ifndef OZ_BUMPMAP

Vertex::Vertex( const Point3& pos_, const TexCoord& texCoord_, const Vec3& normal_ )
{
  pos[0] = pos_.x;
  pos[1] = pos_.y;
  pos[2] = pos_.z;

  texCoord[0] = texCoord_.u;
  texCoord[1] = texCoord_.v;

  normal[0] = normal_.x;
  normal[1] = normal_.y;
  normal[2] = normal_.z;
}

bool Vertex::operator == ( const Vertex& v ) const
{
  return pos[0] == v.pos[0] && pos[1] == v.pos[1] && pos[2] == v.pos[2] &&
      texCoord[0] == v.texCoord[0] && texCoord[1] == v.texCoord[1] &&
      normal[0] == v.normal[0] && normal[1] == v.normal[1] && normal[2] == v.normal[2];
}

#else

Vertex::Vertex( const Point3& pos_, const TexCoord& texCoord_, const Vec3& normal_,
                const Vec3& tangent_, const Vec3& binormal_ )
{
  pos[0] = pos_.x;
  pos[1] = pos_.y;
  pos[2] = pos_.z;

  texCoord[0] = texCoord_.u;
  texCoord[1] = texCoord_.v;

  normal[0] = normal_.x;
  normal[1] = normal_.y;
  normal[2] = normal_.z;

  tangent[0] = tangent_.x;
  tangent[1] = tangent_.y;
  tangent[2] = tangent_.z;

  binormal[0] = binormal_.x;
  binormal[1] = binormal_.y;
  binormal[2] = binormal_.z;
}

bool Vertex::operator == ( const Vertex& v ) const
{
  return pos[0] == v.pos[0] && pos[1] == v.pos[1] && pos[2] == v.pos[2] &&
      texCoord[0] == v.texCoord[0] && texCoord[1] == v.texCoord[1] &&
      normal[0] == v.normal[0] && normal[1] == v.normal[1] && normal[2] == v.normal[2] &&
      tangent[0] == v.tangent[0] && tangent[1] == v.tangent[1] && tangent[2] == v.tangent[2] &&
      binormal[0] == v.binormal[0] && binormal[1] == v.binormal[1] && binormal[2] == v.binormal[2];
}

#endif

void Vertex::read( InputStream* stream )
{
  pos[0] = stream->readFloat();
  pos[1] = stream->readFloat();
  pos[2] = stream->readFloat();

  texCoord[0] = stream->readFloat();
  texCoord[1] = stream->readFloat();

  normal[0] = stream->readFloat();
  normal[1] = stream->readFloat();
  normal[2] = stream->readFloat();

#ifdef OZ_BUMPMAP
  tangent[0] = stream->readFloat();
  tangent[1] = stream->readFloat();
  tangent[2] = stream->readFloat();

  binormal[0] = stream->readFloat();
  binormal[1] = stream->readFloat();
  binormal[2] = stream->readFloat();
#endif
}

void Vertex::write( BufferStream* stream ) const
{
  stream->writeFloat( pos[0] );
  stream->writeFloat( pos[1] );
  stream->writeFloat( pos[2] );

  stream->writeFloat( texCoord[0] );
  stream->writeFloat( texCoord[1] );

  stream->writeFloat( normal[0] );
  stream->writeFloat( normal[1] );
  stream->writeFloat( normal[2] );

#ifdef OZ_BUMPMAP
  stream->writeFloat( tangent[0] );
  stream->writeFloat( tangent[1] );
  stream->writeFloat( tangent[2] );

  stream->writeFloat( binormal[0] );
  stream->writeFloat( binormal[1] );
  stream->writeFloat( binormal[2] );
#endif
}

void Vertex::setFormat()
{
  glEnableVertexAttribArray( Attrib::POSITION );
  glVertexAttribPointer( Attrib::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                         reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, pos ) );

  glEnableVertexAttribArray( Attrib::TEXCOORD );
  glVertexAttribPointer( Attrib::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                         reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, texCoord ) );

  glEnableVertexAttribArray( Attrib::NORMAL );
  glVertexAttribPointer( Attrib::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                         reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, normal ) );

#ifdef OZ_BUMPMAP
  glEnableVertexAttribArray( Attrib::TANGENT );
  glVertexAttribPointer( Attrib::TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                         reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, tangent ) );

  glEnableVertexAttribArray( Attrib::BINORMAL );
  glVertexAttribPointer( Attrib::BINORMAL, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                         reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, binormal ) );
#endif
}

const Mesh* Mesh::lastMesh = null;

void Mesh::reset()
{
  lastMesh = null;
}

Mesh::Mesh() : vao( 0 )
{}

Mesh::~Mesh()
{
  hard_assert( vao == 0 );
}

void Mesh::load( InputStream* stream, uint usage )
{
  flags = 0;

  int nVertices = stream->readInt();
  int nIndices  = stream->readInt();

# ifdef OZ_GL_COMPATIBLE
  vao = 1;
# else
  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );
# endif

  glGenBuffers( 1, &vbo );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, nVertices * int( sizeof( Vertex ) ), 0, usage );

  Vertex* vertices = reinterpret_cast<Vertex*>( glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY ) );

  for( int i = 0; i < nVertices; ++i ) {
    vertices[i].read( stream );
  }

  glUnmapBuffer( GL_ARRAY_BUFFER );

  glGenBuffers( 1, &ibo );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, nIndices * int( sizeof( ushort ) ), 0, GL_STATIC_DRAW );

  ushort* indices =
      reinterpret_cast<ushort*>( glMapBuffer( GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY ) );

  for( int i = 0; i < nIndices; ++i ) {
    indices[i] = ushort( stream->readShort() );
  }

  glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );

# ifndef OZ_GL_COMPATIBLE
  Vertex::setFormat();

  glBindVertexArray( 0 );
# endif

  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

  DArray<uint> textures;

  int nTextures = stream->readInt();

  if( nTextures < 0 ) {
    nTextures = ~nTextures;
    textures.alloc( nTextures );

    flags |= EMBEDED_TEX_BIT;
    textures[0] = 0;

    for( int i = 1; i < nTextures; ++i ) {
      textures[i] = context.readTexture( stream );
    }
  }
  else {
    textures.alloc( nTextures );
    texIds.alloc( nTextures );

    for( int i = 0; i < nTextures; ++i ) {
      const String& name = stream->readString();

      if( name.isEmpty() ) {
        texIds[i]   = -1;
        textures[i] = 0;
      }
      else {
        texIds[i]   = library.textureIndex( name );
        textures[i] = context.requestTexture( texIds[i] );
      }
    }
  }

  nParts = stream->readInt();

  parts = new Part[nParts];

  for( int i = 0; i < nParts; ++i ) {
    parts[i].flags      = stream->readInt();
    parts[i].mode       = uint( stream->readInt() );

    parts[i].texture    = textures[ stream->readInt() ];
    parts[i].masks      = textures[ stream->readInt() ];
    parts[i].masks      = parts[i].masks == 0 ? shader.defaultMasks : parts[i].masks;
    parts[i].alpha      = stream->readFloat();
    parts[i].specular   = stream->readFloat();

    parts[i].nIndices   = stream->readInt();
    parts[i].firstIndex = stream->readInt();

    parts[i].flags |= parts[i].alpha == 1.0f ? 0x0100 : 0x0200;
    flags |= parts[i].flags & ( 0x0100 | 0x0200 );
  }

  textures.dealloc();

  OZ_GL_CHECK_ERROR();
}

void Mesh::unload()
{
  if( vao != 0 ) {
    if( flags & EMBEDED_TEX_BIT ) {
      for( int i = 0; i < nParts; ++i ) {
        if( parts[i].texture != 0 ) {
          glDeleteTextures( 1, &parts[i].texture );
        }
        if( parts[i].masks != shader.defaultMasks ) {
          glDeleteTextures( 1, &parts[i].masks );
        }
      }
    }
    else {
      foreach( id, texIds.citer() ) {
        if( *id != -1 ) {
          context.releaseTexture( *id );
        }
      }
      texIds.dealloc();
    }

    glDeleteBuffers( 1, &vbo );
    glDeleteBuffers( 1, &ibo );
# ifndef OZ_GL_COMPATIBLE
    glDeleteVertexArrays( 1, &vao );
# endif

    vao = 0;
  }

  OZ_GL_CHECK_ERROR();
}

void Mesh::upload( const Vertex* vertices, int nVertices, uint usage ) const
{
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, nVertices * int( sizeof( Vertex ) ), vertices, usage );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

Vertex* Mesh::map( uint access ) const
{
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  return reinterpret_cast<Vertex*>( glMapBuffer( GL_ARRAY_BUFFER, access ) );
}

void Mesh::unmap() const
{
  glUnmapBuffer( GL_ARRAY_BUFFER );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void Mesh::bind() const
{
  if( this != lastMesh ) {
# ifdef OZ_GL_COMPATIBLE
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    Vertex::setFormat();
# else
    glBindVertexArray( vao );
# endif
  }
}

void Mesh::drawComponent( int id, int mask ) const
{
  mask &= flags;

  if( mask == 0 ) {
    return;
  }

  for( int i = 0; i < nParts; ++i ) {
    const Part& part = parts[i];

    int component = part.flags & COMPONENT_MASK;
    if( component < id ) {
      continue;
    }
    else if( component > id ) {
      break;
    }
    else if( part.flags & mask ) {
      glActiveTexture( GL_TEXTURE0 );
      glBindTexture( GL_TEXTURE_2D, part.texture );
      glActiveTexture( GL_TEXTURE1 );
      glBindTexture( GL_TEXTURE_2D, part.masks );
      glUniform1f( param.oz_Specular, part.specular );
      glUniform4f( param.oz_Colour, shader.colour.x, shader.colour.y, shader.colour.z,
                   shader.colour.w * part.alpha );
      glDrawElements( part.mode, part.nIndices, GL_UNSIGNED_SHORT,
                      reinterpret_cast<const ushort*>( 0 ) + part.firstIndex );
    }
  }
}

void Mesh::draw( int mask ) const
{
  mask &= flags;

  if( mask == 0 ) {
    return;
  }

  if( this != lastMesh ) {
# ifdef OZ_GL_COMPATIBLE
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    Vertex::setFormat();
# else
    glBindVertexArray( vao );
# endif
  }

  for( int i = 0; i < nParts; ++i ) {
    const Part& part = parts[i];

    if( part.flags & mask ) {
      glActiveTexture( GL_TEXTURE0 );
      glBindTexture( GL_TEXTURE_2D, part.texture );
      glActiveTexture( GL_TEXTURE1 );
      glBindTexture( GL_TEXTURE_2D, part.masks );
      glUniform1f( param.oz_Specular, part.specular );
      glUniform4f( param.oz_Colour, shader.colour.x, shader.colour.y, shader.colour.z,
                   shader.colour.w * part.alpha );
      glDrawElements( part.mode, part.nIndices, GL_UNSIGNED_SHORT,
                      reinterpret_cast<const ushort*>( 0 ) + part.firstIndex );
    }
  }
}

}
}
