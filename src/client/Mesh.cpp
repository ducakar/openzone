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

#define GL_GLEXT_PROTOTYPES
#include <SDL_opengl.h>
#include "Context.hpp"

namespace oz
{
namespace client
{

  void Mesh::begin()
  {
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    glClientActiveTexture( GL_TEXTURE0 );
    glEnable( GL_TEXTURE_2D );
    glClientActiveTexture( GL_TEXTURE1 );
    glEnable( GL_TEXTURE_2D );
    glClientActiveTexture( GL_TEXTURE2 );
    glEnable( GL_TEXTURE_2D );
  }

  void Mesh::end()
  {
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,  Colours::WHITE );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, Colours::BLACK );

    glClientActiveTexture( GL_TEXTURE0 );
    glDisable( GL_TEXTURE_2D );
    glClientActiveTexture( GL_TEXTURE1 );
    glDisable( GL_TEXTURE_2D );
    glClientActiveTexture( GL_TEXTURE2 );
    glDisable( GL_TEXTURE_2D );

    glEnableClientState( GL_NORMAL_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glEnableClientState( GL_VERTEX_ARRAY );
  }

  void Mesh::loadStatic( InputStream* stream )
  {
    int nVertices = stream->readInt();
    int nIndices  = stream->readInt();
    int nParts    = stream->readInt();

    Vertex* vertices = new Vertex[nVertices];
    ushort* indices  = new ushort[nIndices];

    for( int i = 0; i < nVertices; ++i ) {
      vertices[i].pos[0] = stream->readFloat();
      vertices[i].pos[1] = stream->readFloat();
      vertices[i].pos[2] = stream->readFloat();

      vertices[i].normal[0] = stream->readFloat();
      vertices[i].normal[1] = stream->readFloat();
      vertices[i].normal[2] = stream->readFloat();

      vertices[i].texCoord[0] = stream->readFloat();
      vertices[i].texCoord[1] = stream->readFloat();
    }

    for( int i = 0; i < nIndices; ++i ) {
      indices[i] = stream->readShort();
    }

    for( int i = 0; i < nParts; ++i ) {
      parts.add();
      Part& part = parts.last();

      part.diffuse      = stream->readQuat();
      part.specular     = stream->readQuat();
      part.texture[0]   = context.loadTexture( stream->readString() );
      part.texture[1]   = context.loadTexture( stream->readString() );
      part.texture[2]   = context.loadTexture( stream->readString() );
      part.flags        = stream->readInt();
      part.firstElement = stream->readInt();
      part.nElements    = stream->readInt();
    }
  }

  void Mesh::draw() const
  {
    glBindBuffer( GL_ARRAY_BUFFER, arrayBuffer );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, elementBuffer );

    glVertexPointer( 3, GL_FLOAT, sizeof( Vertex ),
                     reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, pos ) );
    glNormalPointer( GL_FLOAT, sizeof( Vertex ),
                     reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, normal ) );
    glClientActiveTexture( GL_TEXTURE0 );
    glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ),
                       reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, texCoord[0] ) );
    glClientActiveTexture( GL_TEXTURE1 );
    glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ),
                       reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, texCoord[1] ) );
    glClientActiveTexture( GL_TEXTURE2 );
    glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ),
                       reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, texCoord[2] ) );

    for( int i = 0; i < parts.length(); ++i ) {
      glMaterialfv( GL_FRONT, GL_DIFFUSE,  parts[i].diffuse );
      glMaterialfv( GL_FRONT, GL_SPECULAR, parts[i].specular );

      glDrawElements( GL_TRIANGLE_STRIP, parts[i].nElements, GL_UNSIGNED_SHORT,
                      reinterpret_cast<uint*>( 0 ) + parts[i].firstElement );
    }
  }

}
}
