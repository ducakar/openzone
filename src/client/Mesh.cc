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

#include <stable.hh>
#include <client/Mesh.hh>

#include <client/Context.hh>
#include <client/Terra.hh>
#include <client/OpenGL.hh>

namespace oz
{
namespace client
{

void Vertex::setFormat()
{
  glEnableVertexAttribArray( Attrib::POSITION );
  glVertexAttribPointer( Attrib::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                         static_cast<char*>( nullptr ) + offsetof( Vertex, pos ) );

  glEnableVertexAttribArray( Attrib::TEXCOORD );
  glVertexAttribPointer( Attrib::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                         static_cast<char*>( nullptr ) + offsetof( Vertex, texCoord ) );

  glEnableVertexAttribArray( Attrib::NORMAL );
  glVertexAttribPointer( Attrib::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                         static_cast<char*>( nullptr ) + offsetof( Vertex, normal ) );
}

void Texture::destroy()
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

Set<Mesh*> Mesh::loadedMeshes;
Vertex*    Mesh::vertexAnimBuffer       = nullptr;
int        Mesh::vertexAnimBufferLength = 0;

void Mesh::animate( const Instance* instance )
{
  if( shader.hasVertexTexture ) {
    glActiveTexture( GL_TEXTURE3 );
    glBindTexture( GL_TEXTURE_2D, positionsTexId );
    glActiveTexture( GL_TEXTURE4 );
    glBindTexture( GL_TEXTURE_2D, normalsTexId );

    glUniform3f( uniform.meshAnimation,
                 float( instance->firstFrame ) / float( nFrames ),
                 float( instance->secondFrame ) / float( nFrames ),
                 instance->interpolation );
  }
  else {
    const Point* currFramePositions = &positions[instance->firstFrame * nFramePositions];
    const Vec3*  currFrameNormals   = &normals[instance->firstFrame * nFramePositions];

    if( instance->interpolation == 0.0f ) {
      for( int i = 0; i < nFrameVertices; ++i ) {
        int j = Math::lround( vertices[i].pos[0] * float( nFramePositions - 1 ) );

        Point pos    = currFramePositions[j];
        Vec3  normal = currFrameNormals[j];

        vertexAnimBuffer[i].pos[0] = pos.x;
        vertexAnimBuffer[i].pos[1] = pos.y;
        vertexAnimBuffer[i].pos[2] = pos.z;

        vertexAnimBuffer[i].texCoord[0] = vertices[i].texCoord[0];
        vertexAnimBuffer[i].texCoord[1] = vertices[i].texCoord[1];

        vertexAnimBuffer[i].normal[0] = normal.x;
        vertexAnimBuffer[i].normal[1] = normal.y;
        vertexAnimBuffer[i].normal[2] = normal.z;
      }
    }
    else {
      const Point* nextFramePositions = &positions[instance->secondFrame * nFramePositions];
      const Vec3*  nextFrameNormals   = &normals[instance->secondFrame * nFramePositions];

      for( int i = 0; i < nFrameVertices; ++i ) {
        int j = Math::lround( vertices[i].pos[0] * float( nFramePositions - 1 ) );

        Point pos    = Math::mix( currFramePositions[j], nextFramePositions[j], instance->interpolation );
        Vec3  normal = Math::mix( currFrameNormals[j],   nextFrameNormals[j],   instance->interpolation );

        vertexAnimBuffer[i].pos[0] = pos.x;
        vertexAnimBuffer[i].pos[1] = pos.y;
        vertexAnimBuffer[i].pos[2] = pos.z;

        vertexAnimBuffer[i].texCoord[0] = vertices[i].texCoord[0];
        vertexAnimBuffer[i].texCoord[1] = vertices[i].texCoord[1];

        vertexAnimBuffer[i].normal[0] = normal.x;
        vertexAnimBuffer[i].normal[1] = normal.y;
        vertexAnimBuffer[i].normal[2] = normal.z;
      }
    }

    upload( vertexAnimBuffer, nFrameVertices, GL_STREAM_DRAW );
  }
}

void Mesh::draw( const Instance* instance, int mask )
{
  tf.model = instance->transform;
  tf.apply();

  tf.colour.w.w = instance->alpha;
  tf.applyColour();

  int firstPart = 0;
  int pastPart  = parts.length();

  if( instance->component >= 0 ) {
    firstPart = componentIndices[instance->component];
    pastPart  = componentIndices[instance->component + 1];
  }

  for( int i = firstPart; i < pastPart; ++i ) {
    const Part& part = parts[i];

    if( part.flags & mask ) {
      glActiveTexture( GL_TEXTURE0 );
      glBindTexture( GL_TEXTURE_2D, part.texture.diffuse );
      glActiveTexture( GL_TEXTURE1 );
      glBindTexture( GL_TEXTURE_2D, part.texture.masks );
      glActiveTexture( GL_TEXTURE2 );
      glBindTexture( GL_TEXTURE_2D, part.texture.normals );

      glDrawElements( part.mode, part.nIndices, GL_UNSIGNED_SHORT,
                      static_cast<ushort*>( nullptr ) + part.firstIndex );
    }
  }
}

void Mesh::drawScheduled( int mask )
{
  foreach( i, loadedMeshes.iter() ) {
    Mesh* mesh = *i;

    glBindBuffer( GL_ARRAY_BUFFER, mesh->vbo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->ibo );

    Vertex::setFormat();

    shader.program( mesh->shaderId );

    foreach( instance, mesh->instances.citer() ) {
      if( !( mesh->flags & mask ) ) {
        continue;
      }

      if( mesh->nFrames != 0 ) {
        mesh->animate( instance );
      }

      mesh->draw( instance, mask );
    }
  }

  for( int i = 4; i >= 0; --i ) {
    glActiveTexture( GL_TEXTURE0 + uint( i ) );
    glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
  }

  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void Mesh::clearScheduled()
{
  foreach( i, loadedMeshes.iter() ) {
    Mesh* mesh = *i;

    mesh->instances.clear();
  }
}

void Mesh::deallocate()
{
  loadedMeshes.deallocate();

  delete[] vertexAnimBuffer;
  vertexAnimBuffer = nullptr;
  vertexAnimBufferLength = 0;
}

Mesh::Mesh() :
  vbo( 0 ), ibo( 0 ), positionsTexId( 0 ), normalsTexId( 0 ),
  vertices( nullptr ), positions( nullptr ), normals( nullptr ),
  instances( 8 )
{}

Mesh::~Mesh()
{
  unload();
}

void Mesh::draw( int mask ) const
{
  mask &= flags;

  if( mask == 0 ) {
    return;
  }

  for( int i = 0; i < parts.length(); ++i ) {
    const Part& part = parts[i];

    if( part.flags & mask ) {
      glActiveTexture( GL_TEXTURE0 );
      glBindTexture( GL_TEXTURE_2D, part.texture.diffuse );
      glActiveTexture( GL_TEXTURE1 );
      glBindTexture( GL_TEXTURE_2D, part.texture.masks );
      glActiveTexture( GL_TEXTURE2 );
      glBindTexture( GL_TEXTURE_2D, part.texture.normals );

      glUniformMatrix4fv( uniform.colourTransform, 1, GL_FALSE, tf.colour );

      glDrawElements( part.mode, part.nIndices, GL_UNSIGNED_SHORT,
                      static_cast<ushort*>( nullptr ) + part.firstIndex );
    }
  }
}

void Mesh::upload( const Vertex* vertices, int nVertices, uint usage ) const
{
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, nVertices * int( sizeof( Vertex ) ), vertices, usage );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void Mesh::load( oz::InputStream* istream, oz::uint usage )
{
  flags = 0;

  int nVertices = istream->readInt();
  int nIndices  = istream->readInt();

  int vboSize = nVertices * int( sizeof( Vertex ) );
  int iboSize = nIndices * int( sizeof( ushort ) );

  const void* vertexBuffer = istream->forward( vboSize );

  glGenBuffers( 1, &vbo );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, vboSize, vertexBuffer, usage );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );

  glGenBuffers( 1, &ibo );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, iboSize, istream->forward( iboSize ), GL_STATIC_DRAW );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

  nFrames = istream->readInt();

  if( nFrames != 0 ) {
    nFramePositions = istream->readInt();
    nFrameVertices  = istream->readInt();

    if( shader.hasVertexTexture ) {
      int vertexBufferSize = nFramePositions * nFrames * int( sizeof( float[3] ) );
      int normalBufferSize = nFramePositions * nFrames * int( sizeof( float[3] ) );

      glGenTextures( 1, &positionsTexId );
      glBindTexture( GL_TEXTURE_2D, positionsTexId );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB16F, nFramePositions, nFrames, 0, GL_RGB, GL_FLOAT,
                    istream->forward( vertexBufferSize ) );
      glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );

      glGenTextures( 1, &normalsTexId );
      glBindTexture( GL_TEXTURE_2D, normalsTexId );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB16F, nFramePositions, nFrames, 0, GL_RGB, GL_FLOAT,
                    istream->forward( normalBufferSize ) );
      glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
    }
    else {
      vertices  = new Vertex[nFrameVertices];
      positions = new Point[nFramePositions * nFrames];
      normals   = new Vec3[nFramePositions * nFrames];

      for( int i = 0; i < nFramePositions * nFrames; ++i ) {
        positions[i] = istream->readPoint();
      }
      for( int i = 0; i < nFramePositions * nFrames; ++i ) {
        normals[i] = istream->readVec3();
      }

      mCopy( vertices, vertexBuffer, size_t( nFrameVertices ) * sizeof( Vertex ) );

      if( nFrameVertices > vertexAnimBufferLength ) {
        delete[] vertexAnimBuffer;

        vertexAnimBuffer = new Vertex[nFrameVertices];
        vertexAnimBufferLength = nFrameVertices;
      }
    }
  }

  shaderId = liber.shaderIndex( istream->readString() );

  DArray<Texture> textures;
  int nTextures = istream->readInt();

  if( nTextures < 0 ) {
    nTextures = ~nTextures;

    textures.resize( nTextures );
    texIds.resize( nTextures * 3 );

    flags |= EMBEDED_TEX_BIT;

    textures[0].diffuse = 0;
    textures[0].masks   = shader.defaultMasks;
    textures[0].normals = shader.defaultNormals;

    texIds[0] = 0;
    texIds[1] = 0;
    texIds[2] = 0;

    for( int i = 1; i < nTextures; ++i ) {
      textures[i]     = context.readTexture( istream );
      texIds[i*3 + 0] = int( textures[i].diffuse );
      texIds[i*3 + 1] = int( textures[i].masks );
      texIds[i*3 + 2] = int( textures[i].normals );
    }
  }
  else {
    textures.resize( nTextures );
    texIds.resize( nTextures );

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
        texIds[i]   = liber.textureIndex( name );
        textures[i] = context.requestTexture( texIds[i] );
      }
    }
  }

  int nComponents = istream->readInt();
  componentIndices.resize( nComponents + 1 );

  int nParts = istream->readInt();
  parts.resize( nParts );

  int lastComponent = 0;
  if( nComponents != 0 ) {
    componentIndices[0] = 0;
    componentIndices[nComponents] = nParts;
  }

  for( int i = 0; i < nParts; ++i ) {
    parts[i].flags      = istream->readInt();
    parts[i].mode       = istream->readUInt();
    parts[i].texture    = textures[ istream->readInt() ];

    parts[i].nIndices   = istream->readInt();
    parts[i].firstIndex = istream->readInt();

    int j = parts[i].flags & COMPONENT_MASK;
    if( j != lastComponent ) {
      hard_assert( j == lastComponent + 1 && j < nComponents );

      componentIndices[j] = i;
      lastComponent = j;
    }

    flags |= parts[i].flags & ( SOLID_BIT | ALPHA_BIT );
  }

  hard_assert( nComponents == 0 || lastComponent == nComponents - 1 );

  textures.clear();

  loadedMeshes.add( this );

  OZ_GL_CHECK_ERROR();
}

void Mesh::unload()
{
  if( vbo == 0 ) {
    return;
  }

  componentIndices.clear();
  parts.clear();

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

  texIds.clear();

  if( shader.hasVertexTexture ) {
    glDeleteTextures( 1, &normalsTexId );
    glDeleteTextures( 1, &positionsTexId );
  }
  else {
    delete[] normals;
    delete[] positions;
    delete[] vertices;
  }

  glDeleteBuffers( 1, &ibo );
  glDeleteBuffers( 1, &vbo );

  ibo = 0;
  vbo = 0;

  loadedMeshes.exclude( this );

  OZ_GL_CHECK_ERROR();
}

}
}
