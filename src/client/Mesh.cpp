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

  void Vertex::set( float px, float py, float pz,
                    float nx, float ny, float nz,
                    float t0u, float t0v,
                    float t1u, float t1v,
                    float t2u, float t2v )
  {
    pos[0] = px;
    pos[1] = py;
    pos[2] = pz;

    normal[0] = nx;
    normal[1] = ny;
    normal[2] = nz;

    texCoord[0] = t0u;
    texCoord[1] = t0v;

    texCoord[2] = t1u;
    texCoord[3] = t1v;

    texCoord[4] = t2u;
    texCoord[5] = t2v;
  }

  void Vertex::set( const Point3& p, const Vec3& n,
                    const TexCoord& t0, const TexCoord& t1, const TexCoord& t2 )
  {
    pos[0] = p.x;
    pos[1] = p.y;
    pos[2] = p.z;

    normal[0] = n.x;
    normal[1] = n.y;
    normal[2] = n.z;

    texCoord[0] = t0.u;
    texCoord[1] = t0.v;

    texCoord[2] = t1.u;
    texCoord[3] = t1.v;

    texCoord[4] = t2.u;
    texCoord[5] = t2.v;
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

    texCoord[2] = stream->readFloat();
    texCoord[3] = stream->readFloat();

    texCoord[4] = stream->readFloat();
    texCoord[5] = stream->readFloat();
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

    stream->writeFloat( texCoord[2] );
    stream->writeFloat( texCoord[3] );

    stream->writeFloat( texCoord[4] );
    stream->writeFloat( texCoord[5] );
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
      part.flags      = stream->readInt();

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
      part.flags      = stream->readInt();

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

    hard_assert( glGetError() == GL_NO_ERROR );
  }

  void Mesh::unload()
  {
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

    hard_assert( glGetError() == GL_NO_ERROR );
  }

  Vertex* Mesh::map( int access ) const
  {
    return context.mapArray( arrayId, access );
  }

  void Mesh::unmap() const
  {
    context.unmapArray( arrayId );
  }

  void Mesh::drawSolid() const
  {
    context.bindArray( arrayId );

    foreach( part, solidParts.citer() ) {
      glMaterialfv( GL_FRONT, GL_DIFFUSE,  part->diffuse  );
      glMaterialfv( GL_FRONT, GL_SPECULAR, part->specular );

      context.bindTextures( part->texture[0], part->texture[1], part->texture[2] );
      context.drawIndexedArray( part->mode, part->firstIndex, part->nIndices );
    }
  }

  void Mesh::drawAlpha() const
  {
    if( alphaParts.isEmpty() ) {
      return;
    }

    glEnable( GL_BLEND );

    context.bindArray( arrayId );

    foreach( part, alphaParts.citer() ) {
      glMaterialfv( GL_FRONT, GL_DIFFUSE,  part->diffuse  );
      glMaterialfv( GL_FRONT, GL_SPECULAR, part->specular );

      context.bindTextures( part->texture[0], part->texture[1], part->texture[2] );
      context.drawIndexedArray( part->mode, part->firstIndex, part->nIndices );
    }

    glDisable( GL_BLEND );
  }

  int MeshData::getSize() const
  {
    size_t size = 0;

    size += 4 * sizeof( int );

    foreach( part, solidParts.citer() ) {
      size += 2 * sizeof( Quat );
      size += 4 * sizeof( int );

      size += part->texture[0].length() + 1;
      size += part->texture[1].length() + 1;
      size += part->texture[2].length() + 1;
    }
    foreach( part, alphaParts.citer() ) {
      size += 2 * sizeof( Quat );
      size += 4 * sizeof( int );

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
      stream->writeInt( part->flags );

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
      stream->writeInt( part->flags );

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
