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
 * @file client/Mesh.cc
 */

#include "stable.hh"

#include "client/Mesh.hh"

#include "client/Colours.hh"
#include "client/Context.hh"
#include "client/OpenGL.hh"

namespace oz
{
namespace client
{

bool Vertex::operator == ( const Vertex& v ) const
{
  return pos[0] == v.pos[0] && pos[1] == v.pos[1] && pos[2] == v.pos[2] &&
         texCoord[0] == v.texCoord[0] && texCoord[1] == v.texCoord[1] &&
         detailCoord[0] == v.detailCoord[0] && detailCoord[1] == v.detailCoord[1] &&
         normal[0] == v.normal[0] && normal[1] == v.normal[1] && normal[2] == v.normal[2] &&
         tangent[0] == v.tangent[0] && tangent[1] == v.tangent[1] && tangent[2] == v.tangent[2] &&
         binormal[0] == v.binormal[0] && binormal[1] == v.binormal[1] && binormal[2] == v.binormal[2];
}

void Vertex::read( InputStream* istream )
{
  pos[0] = istream->readFloat();
  pos[1] = istream->readFloat();
  pos[2] = istream->readFloat();

  texCoord[0] = istream->readFloat();
  texCoord[1] = istream->readFloat();

  detailCoord[0] = istream->readFloat();
  detailCoord[1] = istream->readFloat();

  normal[0] = istream->readFloat();
  normal[1] = istream->readFloat();
  normal[2] = istream->readFloat();

  tangent[0] = istream->readFloat();
  tangent[1] = istream->readFloat();
  tangent[2] = istream->readFloat();

  binormal[0] = istream->readFloat();
  binormal[1] = istream->readFloat();
  binormal[2] = istream->readFloat();
}

void Vertex::write( BufferStream* ostream ) const
{
  ostream->writeFloat( pos[0] );
  ostream->writeFloat( pos[1] );
  ostream->writeFloat( pos[2] );

  ostream->writeFloat( texCoord[0] );
  ostream->writeFloat( texCoord[1] );

  ostream->writeFloat( detailCoord[0] );
  ostream->writeFloat( detailCoord[1] );

  ostream->writeFloat( normal[0] );
  ostream->writeFloat( normal[1] );
  ostream->writeFloat( normal[2] );

  ostream->writeFloat( tangent[0] );
  ostream->writeFloat( tangent[1] );
  ostream->writeFloat( tangent[2] );

  ostream->writeFloat( binormal[0] );
  ostream->writeFloat( binormal[1] );
  ostream->writeFloat( binormal[2] );
}

void Vertex::setFormat()
{
  glEnableVertexAttribArray( Attrib::POSITION );
  glVertexAttribPointer( Attrib::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                         reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, pos ) );

  glEnableVertexAttribArray( Attrib::TEXCOORD );
  glVertexAttribPointer( Attrib::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                         reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, texCoord ) );

  glEnableVertexAttribArray( Attrib::DETAILCOORD );
  glVertexAttribPointer( Attrib::DETAILCOORD, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                         reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, detailCoord ) );

  glEnableVertexAttribArray( Attrib::NORMAL );
  glVertexAttribPointer( Attrib::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                         reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, normal ) );

//   glEnableVertexAttribArray( Attrib::TANGENT );
//   glVertexAttribPointer( Attrib::TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
//                          reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, tangent ) );
//
//   glEnableVertexAttribArray( Attrib::BINORMAL );
//   glVertexAttribPointer( Attrib::BINORMAL, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
//                          reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, binormal ) );
}

void Texture::free()
{
  if( albedo != 0 ) {
    glDeleteTextures( 1, &albedo );
    albedo = 0;
  }
  if( masks != 0 ) {
    glDeleteTextures( 1, &masks );
    masks = 0;
  }
  if( normals != 0 ) {
    glDeleteTextures( 1, &normals );
    normals = 0;
  }
}

const Mesh* Mesh::lastMesh = null;

void Mesh::reset()
{
  lastMesh = null;
}

Mesh::Mesh() :
  vao( 0 ), parts( null )
{}

Mesh::~Mesh()
{
  hard_assert( vao == 0 );

  delete[] parts;
}

void Mesh::load( oz::InputStream* istream, oz::uint usage, const char* path )
{
  flags = 0;

  int nVertices = istream->readInt();
  int nIndices  = istream->readInt();

#ifdef OZ_GL_COMPATIBLE
  vao = 1;
#else
  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );
#endif

  int vboSize = nVertices * int( sizeof( Vertex ) );
  int iboSize = nIndices * int( sizeof( ushort ) );

  glGenBuffers( 1, &vbo );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, vboSize, istream->forward( vboSize ), usage );

  glGenBuffers( 1, &ibo );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, iboSize, istream->forward( iboSize ), GL_STATIC_DRAW );

#ifndef OZ_GL_COMPATIBLE
  Vertex::setFormat();

  glBindVertexArray( 0 );
#endif

  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

  DArray<Texture> textures;

  int nTextures = istream->readInt();

  if( nTextures < 0 ) {
    nTextures = ~nTextures;
    textures.alloc( nTextures );
    texIds.alloc( nTextures * 3 );

    flags |= EMBEDED_TEX_BIT;

    textures[0].albedo  = 0;
    textures[0].masks   = shader.defaultMasks;
    textures[0].normals = shader.defaultNormals;

    texIds[0] = 0;
    texIds[1] = 0;
    texIds[2] = 0;

    for( int i = 1; i < nTextures; ++i ) {
      int flags = istream->readInt();

      textures[i].albedo  = flags & ALBEDO_BIT  ? context.readTextureLayer( istream, path ) : 0;
      textures[i].masks   = flags & MASKS_BIT   ? context.readTextureLayer( istream, path ) : shader.defaultMasks;
      textures[i].normals = flags & NORMALS_BIT ? context.readTextureLayer( istream, path ) : shader.defaultNormals;

      texIds[i*3 + 0] = int( textures[i].albedo );
      texIds[i*3 + 1] = int( textures[i].masks );
      texIds[i*3 + 2] = int( textures[i].normals );
    }
  }
  else {
    textures.alloc( nTextures );
    texIds.alloc( nTextures );

    for( int i = 0; i < nTextures; ++i ) {
      const String& name = istream->readString();

      if( name.isEmpty() ) {
        texIds[i]           = -1;
        textures[i].albedo  = 0;
        textures[i].masks   = shader.defaultMasks;
        textures[i].normals = shader.defaultNormals;
      }
      else {
        texIds[i]   = library.textureIndex( name );
        textures[i] = context.requestTexture( texIds[i] );
      }
    }
  }

  nParts = istream->readInt();
  parts = new Part[nParts];

  for( int i = 0; i < nParts; ++i ) {
    parts[i].flags      = istream->readInt();
    parts[i].mode       = uint( istream->readInt() );

    parts[i].texture    = textures[ istream->readInt() ];
    parts[i].alpha      = istream->readFloat();
    parts[i].specular   = istream->readFloat();

    parts[i].nIndices   = istream->readInt();
    parts[i].firstIndex = istream->readInt();

    parts[i].flags     |= parts[i].alpha == 1.0f ? int( SOLID_BIT ) : int( ALPHA_BIT );
    flags              |= parts[i].flags & ( SOLID_BIT | ALPHA_BIT );
  }

  textures.dealloc();

  OZ_GL_CHECK_ERROR();
}

void Mesh::unload()
{
  if( vao != 0 ) {
    if( flags & EMBEDED_TEX_BIT ) {
      foreach( texId, texIds.citer() ) {
        if( *texId != 0 ) {
          uint id = uint( *texId );
          glDeleteTextures( 1, &id );
        }
      }
    }
    else {
      foreach( id, texIds.citer() ) {
        if( *id != -1 ) {
          context.releaseTexture( *id );
        }
      }
    }

    texIds.dealloc();

    glDeleteBuffers( 1, &vbo );
    glDeleteBuffers( 1, &ibo );
#ifndef OZ_GL_COMPATIBLE
    glDeleteVertexArrays( 1, &vao );
#endif

    vbo = 0;
    ibo = 0;
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

void Mesh::bind() const
{
  if( this != lastMesh ) {
#ifdef OZ_GL_COMPATIBLE
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    Vertex::setFormat();
#else
    glBindVertexArray( vao );
#endif
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
      glBindTexture( GL_TEXTURE_2D, part.texture.albedo );
      glActiveTexture( GL_TEXTURE1 );
      glBindTexture( GL_TEXTURE_2D, part.texture.masks );

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
#ifdef OZ_GL_COMPATIBLE
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    Vertex::setFormat();
#else
    glBindVertexArray( vao );
#endif
  }

  for( int i = 0; i < nParts; ++i ) {
    const Part& part = parts[i];

    if( part.flags & mask ) {
      glActiveTexture( GL_TEXTURE0 );
      glBindTexture( GL_TEXTURE_2D, part.texture.albedo );
      glActiveTexture( GL_TEXTURE1 );
      glBindTexture( GL_TEXTURE_2D, part.texture.masks );

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
