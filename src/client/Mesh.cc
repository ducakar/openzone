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

#include "client/Context.hh"
#include "client/Terra.hh"
#include "client/OpenGL.hh"

namespace oz
{
namespace client
{

void Vertex::setFormat()
{
  glEnableVertexAttribArray( Attrib::POSITION );
  glVertexAttribPointer( Attrib::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                         static_cast<char*>( null ) + offsetof( Vertex, pos ) );

  glEnableVertexAttribArray( Attrib::TEXCOORD );
  glVertexAttribPointer( Attrib::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                         static_cast<char*>( null ) + offsetof( Vertex, texCoord ) );

  glEnableVertexAttribArray( Attrib::NORMAL );
  glVertexAttribPointer( Attrib::NORMAL, 3, GL_BYTE, GL_TRUE, sizeof( Vertex ),
                         static_cast<char*>( null ) + offsetof( Vertex, normal ) );

  glEnableVertexAttribArray( Attrib::TANGENT );
  glVertexAttribPointer( Attrib::TANGENT, 3, GL_BYTE, GL_TRUE, sizeof( Vertex ),
                         static_cast<char*>( null ) + offsetof( Vertex, tangent ) );

  glEnableVertexAttribArray( Attrib::BINORMAL );
  glVertexAttribPointer( Attrib::BINORMAL, 3, GL_BYTE, GL_TRUE, sizeof( Vertex ),
                         static_cast<char*>( null ) + offsetof( Vertex, binormal ) );

  glEnableVertexAttribArray( Attrib::BONES );
  glVertexAttribPointer( Attrib::BONES, 2, GL_BYTE, GL_FALSE, sizeof( Vertex ),
                         static_cast<char*>( null ) + offsetof( Vertex, bones ) );

  glEnableVertexAttribArray( Attrib::BLEND );
  glVertexAttribPointer( Attrib::BLEND, 1, GL_UNSIGNED_BYTE, GL_TRUE, sizeof( Vertex ),
                         static_cast<char*>( null ) + offsetof( Vertex, blend ) );
}

void Texture::free()
{
  if( diffuse != 0 ) {
    glDeleteTextures( 1, &diffuse );
    diffuse = 0;
  }
  if( masks != shader.defaultMasks ) {
    glDeleteTextures( 1, &masks );
    masks = 0;
  }
  if( normals != shader.defaultNormals ) {
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
  vbo( 0 ), ibo( 0 ), parts( null )
{}

Mesh::~Mesh()
{
  hard_assert( vbo == 0 );

  delete[] parts;
}

void Mesh::load( oz::InputStream* istream, oz::uint usage, const char* path )
{
  flags = 0;

  int nVertices = istream->readInt();
  int nIndices  = istream->readInt();

  int vboSize = nVertices * int( sizeof( Vertex ) );
  int iboSize = nIndices * int( sizeof( ushort ) );

  glGenBuffers( 1, &vbo );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, vboSize, istream->forward( vboSize ), usage );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );

  glGenBuffers( 1, &ibo );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, iboSize, istream->forward( iboSize ), GL_STATIC_DRAW );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

  DArray<Texture> textures;

  int nTextures = istream->readInt();

  if( nTextures < 0 ) {
    nTextures = ~nTextures;
    textures.alloc( nTextures );
    texIds.alloc( nTextures * 3 );

    flags |= EMBEDED_TEX_BIT;

    textures[0].diffuse = 0;
    textures[0].masks   = shader.defaultMasks;
    textures[0].normals = shader.defaultNormals;

    texIds[0] = 0;
    texIds[1] = 0;
    texIds[2] = 0;

    for( int i = 1; i < nTextures; ++i ) {
      int flags = istream->readInt();

      textures[i].diffuse = flags & DIFFUSE_BIT ? context.readTextureLayer( istream, path ) : 0;
      textures[i].masks   = flags & MASKS_BIT   ? context.readTextureLayer( istream, path ) :
                                                  shader.defaultMasks;
      textures[i].normals = flags & NORMALS_BIT ? context.readTextureLayer( istream, path ) :
                                                  shader.defaultNormals;

      texIds[i*3 + 0] = int( textures[i].diffuse );
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
        textures[i].diffuse = 0;
        textures[i].masks   = shader.defaultMasks;
        textures[i].normals = shader.defaultNormals;
      }
      else if( name.beginsWith( "@sea:" ) ) {
        texIds[i]           = -1;
        textures[i].diffuse = terra.waterTexId;
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
    parts[i].mode       = istream->readUInt();
    parts[i].texture    = textures[ istream->readInt() ];

    parts[i].nIndices   = istream->readInt();
    parts[i].firstIndex = istream->readInt();

    flags              |= parts[i].flags & ( SOLID_BIT | ALPHA_BIT );
  }

  textures.dealloc();

  OZ_GL_CHECK_ERROR();
}

void Mesh::unload()
{
  if( vbo == 0 ) {
    return;
  }

  if( flags & EMBEDED_TEX_BIT ) {
    foreach( texId, texIds.citer() ) {
      uint id = uint( *texId );

      if( id != 0 && id != shader.defaultMasks && id != shader.defaultNormals ) {
        glDeleteTextures( 1, &id );
      }
    }
  }
  else {
    foreach( id, texIds.citer() ) {
      if( *id >= 0 ) {
        context.releaseTexture( *id );
      }
    }
  }

  texIds.dealloc();

  glDeleteBuffers( 1, &ibo );
  glDeleteBuffers( 1, &vbo );

  ibo = 0;
  vbo = 0;

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
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );

    Vertex::setFormat();
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
      glBindTexture( GL_TEXTURE_2D, part.texture.diffuse );
      glActiveTexture( GL_TEXTURE1 );
      glBindTexture( GL_TEXTURE_2D, part.texture.masks );
      glActiveTexture( GL_TEXTURE2 );
      glBindTexture( GL_TEXTURE_2D, part.texture.normals );

      glUniformMatrix4fv( param.oz_ColourTransform, 1, GL_FALSE, shader.colourTransform );

      glDrawElements( part.mode, part.nIndices, GL_UNSIGNED_SHORT,
                      static_cast<ushort*>( null ) + part.firstIndex );
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
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );

    Vertex::setFormat();
  }

  for( int i = 0; i < nParts; ++i ) {
    const Part& part = parts[i];

    if( part.flags & mask ) {
      glActiveTexture( GL_TEXTURE0 );
      glBindTexture( GL_TEXTURE_2D, part.texture.diffuse );
      glActiveTexture( GL_TEXTURE1 );
      glBindTexture( GL_TEXTURE_2D, part.texture.masks );
      glActiveTexture( GL_TEXTURE2 );
      glBindTexture( GL_TEXTURE_2D, part.texture.normals );

      glUniformMatrix4fv( param.oz_ColourTransform, 1, GL_FALSE, shader.colourTransform );

      glDrawElements( part.mode, part.nIndices, GL_UNSIGNED_SHORT,
                      static_cast<ushort*>( null ) + part.firstIndex );
    }
  }
}

}
}
