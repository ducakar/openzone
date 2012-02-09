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
 * @file build/Mesh.cc
 */

#include "stable.hh"

#include "build/Mesh.hh"

#include "build/Context.hh"

namespace oz
{
namespace build
{

void MeshData::write( BufferStream* stream, bool embedTextures ) const
{
  hard_assert( parts.length() > 0 );
  hard_assert( indices.length() > 0 );
  hard_assert( vertices.length() > 0 );

  log.println( "Writing mesh {" );
  log.indent();

  stream->writeInt( vertices.length() );
  stream->writeInt( indices.length() );

  foreach( vertex, vertices.citer() ) {
    vertex->write( stream );
  }
  foreach( index, indices.citer() ) {
    stream->writeShort( short( *index ) );
  }

  Vector<String> textures;
  textures.add( "" );

  foreach( part, parts.citer() ) {
    textures.include( part->texture );
    textures.include( part->masks );
  }

  if( embedTextures ) {
    stream->writeInt( ~textures.length() );

    for( int i = 1; i < textures.length(); ++i ) {
      uint id = context.loadRawTexture( textures[i] );

      context.writeTexture( id, stream );
      glDeleteTextures( 1, &id );
    }
  }
  else {
    stream->writeInt( textures.length() );

    foreach( texture, textures.citer() ) {
      stream->writeString( *texture );
    }
  }

  stream->writeInt( parts.length() );

  foreach( part, parts.citer() ) {
    stream->writeInt( part->component );
    stream->writeInt( int( part->mode ) );

    stream->writeInt( textures.index( part->texture ) );
    stream->writeInt( textures.index( part->masks ) );
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
