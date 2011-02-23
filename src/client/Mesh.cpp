/*
 *  Mesh.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Mesh.hpp"

#include "client/Colours.hpp"
#include "client/Context.hpp"

#define GL_GLEXT_PROTOTYPES
#include <SDL_opengl.h>

namespace oz
{
namespace client
{

  bool Vertex::operator == ( const Vertex& v ) const
  {
    return pos[0] == v.pos[0] && pos[1] == v.pos[1] && pos[2] == v.pos[2] &&
        normal[0] == v.normal[0] && normal[1] == v.normal[1] && normal[2] == v.normal[2] &&
        texCoord[0] == v.texCoord[0] && texCoord[1] == v.texCoord[1];
  }

  void Vertex::set( float x, float y, float z, float nx, float ny, float nz, float u, float v )
  {
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;

    normal[0] = nx;
    normal[1] = ny;
    normal[2] = nz;

    texCoord[0] = u;
    texCoord[1] = v;
  }

  void Vertex::set( const Point3& p, const Vec3& n, const TexCoord& t )
  {
    pos[0] = p.x;
    pos[1] = p.y;
    pos[2] = p.z;

    normal[0] = n.x;
    normal[1] = n.y;
    normal[2] = n.z;

    texCoord[0] = t.u;
    texCoord[1] = t.v;
  }

  void Vertex::read( InputStream* stream )
  {
    pos[0] = stream->readFloat();
    pos[1] = stream->readFloat();
    pos[2] = stream->readFloat();

    normal[0] = stream->readFloat();
    normal[1] = stream->readFloat();
    normal[2] = stream->readFloat();

    texCoord[0] = stream->readFloat();
    texCoord[1] = stream->readFloat();
  }

  void Vertex::write( OutputStream* stream ) const
  {
    stream->writeFloat( pos[0] );
    stream->writeFloat( pos[1] );
    stream->writeFloat( pos[2] );

    stream->writeFloat( normal[0] );
    stream->writeFloat( normal[1] );
    stream->writeFloat( normal[2] );

    stream->writeFloat( texCoord[0] );
    stream->writeFloat( texCoord[1] );
  }

  Mesh::Mesh() : arrayId( ~0u )
  {}

  Mesh::~Mesh()
  {
    hard_assert( arrayId == ~0u );
  }

  void Mesh::load( InputStream* stream, int usage )
  {
    int nSolidParts = stream->readInt();
    for( int i = 0; i < nSolidParts; ++i ) {
      solidParts.add();
      Part& part = solidParts.last();

      part.diffuse    = stream->readQuat();
      part.specular   = stream->readQuat();

      String texture0 = stream->readString();
      String texture1 = stream->readString();
      String texture2 = stream->readString();

      part.texture[0] = texture0.isEmpty() ? GL_NONE : context.loadTexture( texture0 );
      part.texture[1] = texture1.isEmpty() ? GL_NONE : context.loadTexture( texture1 );
      part.texture[2] = texture2.isEmpty() ? GL_NONE : context.loadTexture( texture2 );

      part.mode       = stream->readInt();

      part.firstIndex = stream->readInt();
      part.nIndices   = stream->readInt();
    }

    int nAlphaParts = stream->readInt();
    for( int i = 0; i < nAlphaParts; ++i ) {
      alphaParts.add();
      Part& part = alphaParts.last();

      part.diffuse    = stream->readQuat();
      part.specular   = stream->readQuat();

      String texture0 = stream->readString();
      String texture1 = stream->readString();
      String texture2 = stream->readString();

      part.texture[0] = texture0.isEmpty() ? GL_NONE : context.loadTexture( texture0 );
      part.texture[1] = texture1.isEmpty() ? GL_NONE : context.loadTexture( texture1 );
      part.texture[2] = texture2.isEmpty() ? GL_NONE : context.loadTexture( texture2 );

      part.mode       = stream->readInt();

      part.firstIndex = stream->readInt();
      part.nIndices   = stream->readInt();
    }

    int nIndices = stream->readInt();
    ushort* indices = new ushort[nIndices];
    for( int i = 0; i < nIndices; ++i ) {
      indices[i] = stream->readShort();
    }

    int nVertices = stream->readInt();
    Vertex* vertices = new Vertex[nVertices];
    for( int i = 0; i < nVertices; ++i ) {
      vertices[i].read( stream );
    }

    arrayId = context.genArray( usage,
                                vertices, nVertices,
                                indices, nIndices );
    delete[] vertices;
    delete[] indices;

    flags = 0;

    if( nSolidParts != 0 ) {
      flags |= SOLID_BIT;
    }
    if( nAlphaParts != 0 ) {
      flags |= ALPHA_BIT;
    }

    hard_assert( glGetError() == GL_NO_ERROR );
  }

  void Mesh::unload()
  {
    if( arrayId != ~0u ) {
      foreach( part, solidParts.citer() ) {
        context.deleteTexture( part->texture[0] );
        context.deleteTexture( part->texture[1] );
        context.deleteTexture( part->texture[2] );
      }
      foreach( part, alphaParts.citer() ) {
        context.deleteTexture( part->texture[0] );
        context.deleteTexture( part->texture[1] );
        context.deleteTexture( part->texture[2] );
      }

      context.deleteArray( arrayId );

      arrayId = ~0u;
    }

    hard_assert( glGetError() == GL_NO_ERROR );
  }

  void Mesh::upload( const Vertex* vertices, int nVertices ) const
  {
    context.uploadArray( arrayId, vertices, nVertices );
  }

  Vertex* Mesh::map( int access, int size ) const
  {
    return context.mapArray( arrayId, access, size );
  }

  void Mesh::unmap() const
  {
    context.unmapArray( arrayId );
  }

  void Mesh::draw( int mask ) const
  {
    mask &= flags;

    if( mask == 0 ) {
      return;
    }

    context.bindArray( arrayId );

    if( mask & SOLID_BIT ) {
      foreach( part, solidParts.citer() ) {
        glMaterialfv( GL_FRONT, GL_DIFFUSE,  part->diffuse  );
        glMaterialfv( GL_FRONT, GL_SPECULAR, part->specular );

        context.bindTextures( part->texture[0], part->texture[1], part->texture[2] );
        context.drawIndexedArray( part->mode, part->firstIndex, part->nIndices );
      }
    }
    if( mask & ALPHA_BIT ) {
      glEnable( GL_BLEND );

      foreach( part, alphaParts.citer() ) {
        glMaterialfv( GL_FRONT, GL_DIFFUSE,  part->diffuse  );
        glMaterialfv( GL_FRONT, GL_SPECULAR, part->specular );

        context.bindTextures( part->texture[0], part->texture[1], part->texture[2] );
        context.drawIndexedArray( part->mode, part->firstIndex, part->nIndices );
      }

      glDisable( GL_BLEND );
    }
  }

  int MeshData::getSize() const
  {
    size_t size = 0;

    size += 4 * sizeof( int );

    foreach( part, solidParts.citer() ) {
      size += 2 * sizeof( Quat );
      size += 3 * sizeof( int );

      size += part->texture[0].length() + 1;
      size += part->texture[1].length() + 1;
      size += part->texture[2].length() + 1;
    }
    foreach( part, alphaParts.citer() ) {
      size += 2 * sizeof( Quat );
      size += 3 * sizeof( int );

      size += part->texture[0].length() + 1;
      size += part->texture[1].length() + 1;
      size += part->texture[2].length() + 1;
    }

    size += indices.length() * sizeof( ushort );
    size += vertices.length() * sizeof( Vertex );

    return int( size );
  }

  void MeshData::write( OutputStream* stream ) const
  {
    hard_assert( solidParts.length() > 0 || alphaParts.length() > 0 );
    hard_assert( indices.length() > 0 );
    hard_assert( vertices.length() > 0 );

    stream->writeInt( solidParts.length() );
    foreach( part, solidParts.citer() ) {
      stream->writeQuat( part->diffuse );
      stream->writeQuat( part->specular );

      stream->writeString( part->texture[0] );
      stream->writeString( part->texture[1] );
      stream->writeString( part->texture[2] );

      stream->writeInt( part->mode );

      stream->writeInt( part->firstIndex );
      stream->writeInt( part->nIndices );
    }

    stream->writeInt( alphaParts.length() );
    foreach( part, alphaParts.citer() ) {
      stream->writeQuat( part->diffuse );
      stream->writeQuat( part->specular );

      stream->writeString( part->texture[0] );
      stream->writeString( part->texture[1] );
      stream->writeString( part->texture[2] );

      stream->writeInt( part->mode );

      stream->writeInt( part->firstIndex );
      stream->writeInt( part->nIndices );
    }

    stream->writeInt( indices.length() );
    foreach( index, indices.citer() ) {
      stream->writeShort( *index );
    }

    stream->writeInt( vertices.length() );
    foreach( vertex, vertices.citer() ) {
      vertex->write( stream );
    }
  }

}
}
