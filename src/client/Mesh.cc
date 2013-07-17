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
 * @file client/Mesh.cc
 */

#include <client/Mesh.hh>

#include <client/Context.hh>
#include <client/Terra.hh>

namespace oz
{
namespace client
{

void Vertex::setFormat()
{
  glEnableVertexAttribArray( Attrib::POSITION );
  glVertexAttribPointer( Attrib::POSITION, 3, GL_FLOAT, GL_FALSE, int( sizeof( Vertex ) ),
                         static_cast<char*>( nullptr ) + offsetof( Vertex, pos ) );

  glEnableVertexAttribArray( Attrib::TEXCOORD );
  glVertexAttribPointer( Attrib::TEXCOORD, 2, GL_FLOAT, GL_FALSE, int( sizeof( Vertex ) ),
                         static_cast<char*>( nullptr ) + offsetof( Vertex, texCoord ) );

  glEnableVertexAttribArray( Attrib::NORMAL );
  glVertexAttribPointer( Attrib::NORMAL, 3, GL_FLOAT, GL_FALSE, int( sizeof( Vertex ) ),
                         static_cast<char*>( nullptr ) + offsetof( Vertex, normal ) );
}

struct Mesh::PreloadData
{
  struct TexFiles
  {
    File diffuse;
    File masks;
    File normals;
  };

  File           modelFile;
  List<TexFiles> textures;
};

Set<Mesh*> Mesh::loadedMeshes;
Vertex*    Mesh::vertexAnimBuffer       = nullptr;
int        Mesh::vertexAnimBufferLength = 0;

void Mesh::animate( const Instance* instance )
{
  if( shader.hasVertexTexture ) {
    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_2D, animationTexId );

    glUniform3f( uniform.meshAnimation,
                 float( instance->firstFrame ) / float( nFrames ),
                 float( instance->secondFrame ) / float( nFrames ),
                 instance->interpolation );
  }
  else {
    const Point* currFramePositions = &positions[instance->firstFrame * nFramePositions];
    const Vec3*  currFrameNormals   = &normals[instance->firstFrame * nFramePositions];

    if( instance->interpolation == 0.0f ) {
      for( int i = 0; i < nVertices; ++i ) {
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

      for( int i = 0; i < nVertices; ++i ) {
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

    upload( vertexAnimBuffer, nVertices, GL_STREAM_DRAW );
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
      const Texture& texture = textures[part.texture];

      glActiveTexture( GL_TEXTURE0 );
      glBindTexture( GL_TEXTURE_2D, texture.diffuse );
      glActiveTexture( GL_TEXTURE1 );
      glBindTexture( GL_TEXTURE_2D, texture.masks );

      glDrawElements( part.mode, part.nIndices, GL_UNSIGNED_SHORT,
                      static_cast<ushort*>( nullptr ) + part.firstIndex );
    }
  }
}

void Mesh::drawScheduled( int mask )
{
  foreach( i, loadedMeshes.iter() ) {
    Mesh* mesh = *i;

    if( mesh->instances.isEmpty() ) {
      continue;
    }

    glBindBuffer( GL_ARRAY_BUFFER, mesh->vbo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->ibo );

    Vertex::setFormat();

    shader.program( mesh->shaderId );

    foreach( instance, mesh->instances.citer() ) {
      // HACK This is not a nice way to draw non-transparent parts for which alpha < 1 has been set.
      int instanceMask = mask;

      if( instance->alpha != 1.0f ) {
        if( mask & ALPHA_BIT ) {
          instanceMask |= SOLID_BIT;
        }
        else {
          continue;
        }
      }

      if( !( mesh->flags & instanceMask ) ) {
        continue;
      }

      if( mesh->nFrames != 0 ) {
        mesh->animate( instance );
      }

      mesh->draw( instance, instanceMask );
    }
  }

  glActiveTexture( GL_TEXTURE2 );
  glBindTexture( GL_TEXTURE_2D, 0 );

  glActiveTexture( GL_TEXTURE1 );
  glBindTexture( GL_TEXTURE_2D, shader.defaultMasks );

  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );

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
  vbo( 0 ), ibo( 0 ), animationTexId( 0 ),
  nTextures( 0 ), nVertices( 0 ), nIndices( 0 ), nFrames( 0 ), nFramePositions( 0 ),
  vertices( nullptr ), positions( nullptr ), normals( nullptr ),
  instances( 8 ), preloadData( nullptr ), dim( Vec3::ONE )
{}

Mesh::~Mesh()
{
  unload();
}

const File* Mesh::preload( const char* path )
{
  hard_assert( preloadData == nullptr );

  preloadData = new PreloadData();
  preloadData->modelFile = path;

  if( !preloadData->modelFile.map() ) {
    OZ_ERROR( "Failed to map '%s'", path );
  }

  InputStream istream = preloadData->modelFile.inputStream();

  dim             = istream.readVec3();
  nTextures       = istream.readInt();
  nVertices       = istream.readInt();
  nIndices        = istream.readInt();
  nFrames         = istream.readInt();
  nFramePositions = istream.readInt();

  istream.readInt();
  istream.readInt();

  shaderId = liber.shaderIndex( istream.readString() );

  if( nTextures > 0 ) {
    for( int i = 0; i < nTextures; ++i ) {
      const String& name = istream.readString();

      preloadData->textures.add();

      if( !name.isEmpty() ) {
        PreloadData::TexFiles& texFiles = preloadData->textures.last();

        texFiles.diffuse = name + ".dds";
        texFiles.masks   = name + "_m.dds";
        texFiles.normals = name + "_n.dds";

        if( !texFiles.diffuse.map() ) {
          OZ_ERROR( "Failed to map '%s'", texFiles.diffuse.path().cstr() );
        }

        texFiles.masks.map();
//         texFiles.normals.map();
      }
    }
  }

  return &preloadData->modelFile;
}

void Mesh::upload( const Vertex* vertices, int nVertices, uint usage ) const
{
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, nVertices * int( sizeof( Vertex ) ), vertices, usage );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void Mesh::load( uint usage )
{
  flags = 0;

  hard_assert( preloadData != nullptr && preloadData->modelFile.isMapped() );
  InputStream istream = preloadData->modelFile.inputStream();

  OZ_GL_CHECK_ERROR();

  istream.readVec3();
  istream.readInt();
  istream.readInt();
  istream.readInt();
  istream.readInt();
  istream.readInt();

  int nComponents = istream.readInt();
  int nParts      = istream.readInt();

  liber.shaderIndex( istream.readString() );

  if( nTextures < 0 ) {
    nTextures = ~nTextures;

    textures.resize( nTextures );

    for( int i = 0; i < nTextures; ++i ) {
      const String& name = istream.readString();

      int id = name.isEmpty()             ? -1 :
               name.beginsWith( "@sea:" ) ? terra.liquidTexId : liber.textureIndex( name );

      textures[i] = context.requestTexture( id );
    }
  }
  else {
    textures.resize( nTextures );

    for( int i = 0; i < nTextures; ++i ) {
      istream.readString();

      textures[i] = context.loadTexture( preloadData->textures[i].diffuse,
                                         preloadData->textures[i].masks,
                                         preloadData->textures[i].normals );
    }
  }

  int vboSize = nVertices * int( sizeof( Vertex ) );
  int iboSize = nIndices  * int( sizeof( ushort ) );

  const void* vertexBuffer = istream.forward( vboSize );

  glGenBuffers( 1, &vbo );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, vboSize, vertexBuffer, usage );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );

  glGenBuffers( 1, &ibo );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, iboSize, istream.forward( iboSize ), GL_STATIC_DRAW );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

  if( nFrames != 0 ) {
    if( shader.hasVertexTexture ) {
      int vertexBufferSize = nFramePositions * nFrames * int( sizeof( float[3] ) );
      int normalBufferSize = nFramePositions * nFrames * int( sizeof( float[3] ) );

      glGenTextures( 1, &animationTexId );
      glBindTexture( GL_TEXTURE_2D, animationTexId );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB16F, nFramePositions, 2 * nFrames, 0, GL_RGB, GL_FLOAT,
                    istream.forward( vertexBufferSize + normalBufferSize ) );
      glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );

      OZ_GL_CHECK_ERROR();
    }
    else {
      vertices  = new Vertex[nVertices];
      positions = new Point[nFramePositions * nFrames];
      normals   = new Vec3[nFramePositions * nFrames];

      for( int i = 0; i < nFramePositions * nFrames; ++i ) {
        positions[i] = istream.readPoint();
      }
      for( int i = 0; i < nFramePositions * nFrames; ++i ) {
        normals[i] = istream.readVec3();
      }

      mCopy( vertices, vertexBuffer, size_t( nVertices ) * sizeof( Vertex ) );

      if( nVertices > vertexAnimBufferLength ) {
        delete[] vertexAnimBuffer;

        vertexAnimBuffer = new Vertex[nVertices];
        vertexAnimBufferLength = nVertices;
      }
    }
  }

  componentIndices.resize( nComponents + 1 );
  parts.resize( nParts );

  int lastComponent = 0;
  if( nComponents != 0 ) {
    componentIndices[0] = 0;
    componentIndices[nComponents] = nParts;
  }

  for( int i = 0; i < nParts; ++i ) {
    parts[i].flags      = istream.readInt();
    parts[i].mode       = istream.readUInt();
    parts[i].texture    = istream.readInt();

    parts[i].nIndices   = istream.readInt();
    parts[i].firstIndex = istream.readInt();

    int j = parts[i].flags & COMPONENT_MASK;
    if( j != lastComponent ) {
      hard_assert( j == lastComponent + 1 && j < nComponents );

      componentIndices[j] = i;
      lastComponent = j;
    }

    flags |= parts[i].flags & ( SOLID_BIT | ALPHA_BIT );
  }

  hard_assert( nComponents == 0 || lastComponent == nComponents - 1 );

  loadedMeshes.add( this );

  delete preloadData;
  preloadData = nullptr;

  OZ_GL_CHECK_ERROR();
}

void Mesh::unload()
{
  if( preloadData != nullptr ) {
    delete preloadData;
    preloadData = nullptr;
  }

  if( vbo == 0 ) {
    return;
  }

  foreach( texture, textures.citer() ) {
    if( texture->id >= -1 ) {
      context.releaseTexture( texture->id );
    }
    else {
      context.unloadTexture( texture );
    }
  }

  componentIndices.clear();
  parts.clear();
  textures.clear();

  if( nFrames != 0 ) {
    if( shader.hasVertexTexture ) {
      glDeleteTextures( 1, &animationTexId );
    }
    else {
      delete[] normals;
      delete[] positions;
      delete[] vertices;
    }
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
