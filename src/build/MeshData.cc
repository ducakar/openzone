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

#include "build/MeshData.hh"

#include "build/Context.hh"

namespace oz
{
namespace build
{

bool Vertex::operator == ( const Vertex& v ) const
{
  return pos == v.pos && texCoord == v.texCoord && normal == v.normal;
}

void Vertex::write( BufferStream* ostream ) const
{
  ostream->writePoint( pos );
  ostream->writeFloat( texCoord.u );
  ostream->writeFloat( texCoord.v );
  ostream->writeVec3( normal );

  if( context.bumpmap ) {
    ostream->writeVec3( tangent );
    ostream->writeVec3( binormal );
    ostream->writeFloat( detailCoord.u );
    ostream->writeFloat( detailCoord.v );
  }
}

void MeshData::write( BufferStream* os, bool embedTextures ) const
{
  hard_assert( parts.length() > 0 );
  hard_assert( indices.length() > 0 );
  hard_assert( vertices.length() > 0 );

  Log::println( "Writing mesh {" );
  Log::indent();

  os->writeInt( vertices.length() );
  os->writeInt( indices.length() );

  foreach( vertex, vertices.citer() ) {
    vertex->write( os );
  }
  foreach( index, indices.citer() ) {
    os->writeShort( short( *index ) );
  }

  Vector<String> textures;
  textures.add( "" ); // No texture.

  foreach( part, parts.citer() ) {
    textures.include( part->texture );
  }

  if( embedTextures ) {
    os->writeInt( ~textures.length() );

    for( int i = 1; i < textures.length(); ++i ) {
      uint diffuseId, masksId, normalsId;
      context.loadTexture( &diffuseId, &masksId, &normalsId, textures[i] );

      int textureFlags = 0;

      if( diffuseId != 0 ) {
        textureFlags |= Mesh::DIFFUSE_BIT;
      }
      if( masksId != 0 ) {
        textureFlags |= Mesh::MASKS_BIT;
      }
      if( normalsId != 0 ) {
        textureFlags |= Mesh::NORMALS_BIT;
      }

      os->writeInt( textureFlags );

      if( diffuseId != 0 ) {
        context.writeLayer( diffuseId, os );
        glDeleteTextures( 1, &diffuseId );
      }
      if( masksId != 0 ) {
        context.writeLayer( masksId, os );
        glDeleteTextures( 1, &masksId );
      }
      if( normalsId != 0 ) {
        context.writeLayer( normalsId, os );
        glDeleteTextures( 1, &normalsId );
      }
    }
  }
  else {
    os->writeInt( textures.length() );

    foreach( texture, textures.citer() ) {
      os->writeString( *texture );
    }
  }

  os->writeInt( parts.length() );

  foreach( part, parts.citer() ) {
    os->writeInt( part->component | part->material );
    os->writeInt( int( part->mode ) );

    os->writeInt( textures.index( part->texture ) );

    os->writeInt( part->nIndices );
    os->writeInt( part->firstIndex );
  }

  Log::unindent();
  Log::println( "}" );
}

}
}
