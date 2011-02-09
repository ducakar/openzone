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
  }

  void Mesh::end()
  {
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,  Colours::WHITE );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, Colours::BLACK );

    glEnableClientState( GL_NORMAL_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glEnableClientState( GL_VERTEX_ARRAY );
  }

  void Mesh::draw() const
  {
    glBindBuffer( GL_ARRAY_BUFFER, arrayBuffer );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, elementBuffer );

    glVertexPointer( 3, GL_FLOAT, sizeof( Vertex ),
                     reinterpret_cast<const char*>( null ) + offsetof( Vertex, pos ) );
    glNormalPointer( GL_FLOAT, sizeof( Vertex ),
                     reinterpret_cast<const char*>( null ) + offsetof( Vertex, normal ) );
    glClientActiveTexture( GL_TEXTURE0 );
    glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ),
                       reinterpret_cast<const char*>( null ) + offsetof( Vertex, texCoord0 ) );
    glClientActiveTexture( GL_TEXTURE1 );
    glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ),
                       reinterpret_cast<const char*>( null ) + offsetof( Vertex, texCoord1 ) );

    for( int i = 0; i < parts.length(); ++i ) {
      glMaterialfv( GL_FRONT, GL_DIFFUSE,  parts[i].diffuse );
      glMaterialfv( GL_FRONT, GL_SPECULAR, parts[i].specular );

      glDrawElements( GL_TRIANGLE_STRIP, parts[i].nElements, GL_UNSIGNED_INT,
                      reinterpret_cast<uint*>( null ) + parts[i].firstElement );
    }
  }

}
}
