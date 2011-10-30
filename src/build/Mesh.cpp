/*
 *  Mesh.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "build/Mesh.hpp"

#include "client/OpenGL.hpp"

#include "build/Context.hpp"

namespace oz
{
namespace build
{

void MeshData::write( OutputStream* stream, bool embedTextures ) const
{
  hard_assert( parts.length() > 0 );
  hard_assert( indices.length() > 0 );
  hard_assert( vertices.length() > 0 );

  log.println( "Writing mesh {" );
  log.indent();

  stream->writeInt( vertices.length() );
  stream->writeInt( indices.length() );

  for( auto vertex : vertices.citer() ) {
    vertex->write( stream );
  }
  for( auto index : indices.citer() ) {
    stream->writeShort( short( *index ) );
  }

  Vector<String> textures;
  textures.add( "" );

  for( auto part : parts.citer() ) {
    textures.include( part->texture );
  }

  if( embedTextures ) {
    stream->writeInt( ~textures.length() );

    for( int i = 1; i < textures.length(); ++i ) {
      uint id = Context::loadRawTexture( textures[i] );

      Context::writeTexture( id, stream );
      glDeleteTextures( 1, &id );
    }
  }
  else {
    stream->writeInt( textures.length() );
    for( auto texture : textures.citer() ) {
      stream->writeString( *texture );
    }
  }

  stream->writeInt( parts.length() );

  for( auto part : parts.citer() ) {
    stream->writeInt( part->component );
    stream->writeInt( int( part->mode ) );

    stream->writeInt( textures.index( part->texture ) );
    stream->writeFloat( part->alpha );
    stream->writeFloat( part->specular );

    stream->writeInt( part->nIndices );
    stream->writeInt( part->firstIndex );
  }

  log.unindent();
  log.println( "}" );
}

}
}