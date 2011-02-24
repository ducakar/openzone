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

#include <GL/gl.h>

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

  Mesh::Mesh() : vao( 0 )
  {}

  Mesh::~Mesh()
  {
    hard_assert( vao == 0 );
  }

  void Mesh::load( InputStream* stream, int usage )
  {
    DArray<uint> textures;

    flags = 0;

    int nTextures = stream->readInt();

    if( nTextures < 0 ) {
      nTextures = ~nTextures;
      textures.alloc( nTextures );

      flags |= EMBEDED_TEX_BIT;
      textures[0] = 0;

      for( int i = 1; i < nTextures; ++i ) {
        textures[i] = context.readTexture( stream );

        hard_assert( textures[i] != 0 );
      }
    }
    else {
      textures.alloc( nTextures );
      texIds.alloc( nTextures );

      for( int i = 0; i < nTextures; ++i ) {
        const String& name = stream->readString();

        if( name.isEmpty() ) {
          texIds[i] = -1;
          textures[i] = 0;
        }
        else {
          texIds[i] = translator.textureIndex( name );
          textures[i] = context.requestTexture( texIds[i] );
        }
      }
    }

    int nSolidParts = stream->readInt();
    for( int i = 0; i < nSolidParts; ++i ) {
      solidParts.add();
      Part& part = solidParts.last();

      part.diffuse    = stream->readQuat();
      part.specular   = stream->readQuat();

      part.texture[0] = textures[ stream->readInt() ];
      part.texture[1] = textures[ stream->readInt() ];
      part.texture[2] = textures[ stream->readInt() ];

      part.mode       = stream->readInt();

      part.minIndex   = stream->readShort();
      part.maxIndex   = stream->readShort();
      part.nIndices   = stream->readShort();
      part.firstIndex = stream->readShort();
    }

    int nAlphaParts = stream->readInt();
    for( int i = 0; i < nAlphaParts; ++i ) {
      alphaParts.add();
      Part& part = alphaParts.last();

      part.diffuse    = stream->readQuat();
      part.specular   = stream->readQuat();

      part.texture[0] = textures[ stream->readInt() ];
      part.texture[1] = textures[ stream->readInt() ];
      part.texture[2] = textures[ stream->readInt() ];

      part.mode       = stream->readInt();

      part.minIndex   = stream->readShort();
      part.maxIndex   = stream->readShort();
      part.nIndices   = stream->readShort();
      part.firstIndex = stream->readShort();
    }

    textures.dealloc();

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

    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    glGenBuffers( 1, &ibo );
    glGenBuffers( 1, &vbo );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof( ushort ), indices, usage );

    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, nVertices * sizeof( Vertex ), vertices, usage );

    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, sizeof( Vertex ),
                     reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, pos ) );

    glEnableClientState( GL_NORMAL_ARRAY );
    glNormalPointer( GL_FLOAT, sizeof( Vertex ),
                     reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, normal ) );

    glClientActiveTexture( GL_TEXTURE0 );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ),
                       reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, texCoord ) );

    glClientActiveTexture( GL_TEXTURE1 );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ),
                       reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, texCoord ) );

    glClientActiveTexture( GL_TEXTURE2 );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ),
                       reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, texCoord ) );

    glBindVertexArray( 0 );

    delete[] indices;
    delete[] vertices;

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
    if( vao != 0 ) {
      if( flags & EMBEDED_TEX_BIT ) {
        foreach( part, solidParts.citer() ) {
          glDeleteTextures( 3, part->texture );
        }
        foreach( part, alphaParts.citer() ) {
          glDeleteTextures( 3, part->texture );
        }
      }
      else {
        foreach( id, texIds.citer() ) {
          if( *id != -1 ) {
            context.releaseTexture( *id );
          }
        }
        texIds.dealloc();
      }

      glDeleteBuffers( 1, &vbo );
      glDeleteBuffers( 1, &ibo );
      glDeleteVertexArrays( 1, &vao );

      vao = 0;
    }

    hard_assert( glGetError() == GL_NO_ERROR );
  }

  void Mesh::upload( const Vertex* vertices, int nVertices, int usage ) const
  {
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, nVertices * sizeof( Vertex ), vertices, usage );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
  }

  Vertex* Mesh::map( int access ) const
  {
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    return reinterpret_cast<Vertex*>( glMapBuffer( GL_ARRAY_BUFFER, access ) );
  }

  void Mesh::unmap() const
  {
    glUnmapBuffer( GL_ARRAY_BUFFER );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
  }

  void Mesh::draw( int mask ) const
  {
    mask &= flags;

    if( mask == 0 ) {
      return;
    }

    glMatrixMode( GL_TEXTURE );
    glActiveTexture( GL_TEXTURE2 );
    glScalef( 4.0f, 4.0f, 0.0f );

    glBindVertexArray( vao );

    if( mask & SOLID_BIT ) {
      foreach( part, solidParts.citer() ) {
        glMaterialfv( GL_FRONT, GL_DIFFUSE,  part->diffuse  );
        glMaterialfv( GL_FRONT, GL_SPECULAR, part->specular );

        context.bindTextures( part->texture[0], part->texture[1], 0 );
        glDrawElements( part->mode, part->nIndices, GL_UNSIGNED_SHORT,
                        reinterpret_cast<const ushort*>( 0 ) + part->firstIndex );
      }
    }
    if( mask & ALPHA_BIT ) {
      glEnable( GL_BLEND );

      foreach( part, alphaParts.citer() ) {
        glMaterialfv( GL_FRONT, GL_DIFFUSE,  part->diffuse  );
        glMaterialfv( GL_FRONT, GL_SPECULAR, part->specular );

        context.bindTextures( part->texture[0], part->texture[1], 0 );
        glDrawElements( part->mode, part->nIndices, GL_UNSIGNED_SHORT,
                        reinterpret_cast<const ushort*>( 0 ) + part->firstIndex );
      }

      glDisable( GL_BLEND );
    }

    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
  }

  int MeshData::getSize( bool embedTextures ) const
  {
    size_t size = 5 * sizeof( int );

    Vector<String> textures;
    textures.add( "" );

    foreach( part, solidParts.citer() ) {
      for( int i = 0; i < 3; ++i ) {
        textures.include( part->texture[i] );
      }
    }
    foreach( part, alphaParts.citer() ) {
      for( int i = 0; i < 3; ++i ) {
        textures.include( part->texture[i] );
      }
    }

    if( embedTextures ) {
      for( int i = 1; i < textures.length(); ++i ) {
        uint id = context.loadTexture( textures[i] );

        int nMipmaps, texSize;
        context.getTextureSize( id, &nMipmaps, &texSize );

        glDeleteTextures( 1, &id );

        size += texSize;
      }
    }
    else {
      foreach( texture, textures.citer() ) {
        size += texture->length() + 1;
      }
    }

    size += solidParts.length() * ( 2 * sizeof( Quat ) + 4 * sizeof( int ) + 4 * sizeof( ushort ) );
    size += alphaParts.length() * ( 2 * sizeof( Quat ) + 4 * sizeof( int ) + 4 * sizeof( ushort ) );

    size += indices.length() * sizeof( ushort );
    size += vertices.length() * sizeof( Vertex );

    return int( size );
  }

  void MeshData::write( OutputStream* stream, bool embedTextures ) const
  {
    hard_assert( solidParts.length() > 0 || alphaParts.length() > 0 );
    hard_assert( indices.length() > 0 );
    hard_assert( vertices.length() > 0 );

    Vector<String> textures;
    textures.add( "" );

    foreach( part, solidParts.citer() ) {
      for( int i = 0; i < 3; ++i ) {
        textures.include( part->texture[i] );
      }
    }
    foreach( part, alphaParts.citer() ) {
      for( int i = 0; i < 3; ++i ) {
        textures.include( part->texture[i] );
      }
    }

    if( embedTextures ) {
      stream->writeInt( ~textures.length() );

      for( int i = 1; i < textures.length(); ++i ) {
        uint id = context.loadTexture( textures[i] );

        int nMipmaps, size;
        context.getTextureSize( id, &nMipmaps, &size );
        context.writeTexture( id, nMipmaps, stream );

        glDeleteTextures( 1, &id );
      }
    }
    else {
      stream->writeInt( textures.length() );
      foreach( texture, textures.citer() ) {
        hard_assert( texture->equals( "" ) || translator.textureIndex( *texture ) != -1 );

        stream->writeString( *texture );
      }
    }

    stream->writeInt( solidParts.length() );
    foreach( part, solidParts.citer() ) {
      stream->writeQuat( part->diffuse );
      stream->writeQuat( part->specular );

      stream->writeInt( textures.index( part->texture[0] ) );
      stream->writeInt( textures.index( part->texture[1] ) );
      stream->writeInt( textures.index( part->texture[2] ) );

      stream->writeInt( part->mode );

      stream->writeShort( part->minIndex );
      stream->writeShort( part->maxIndex );
      stream->writeShort( part->nIndices );
      stream->writeShort( part->firstIndex );
    }

    stream->writeInt( alphaParts.length() );
    foreach( part, alphaParts.citer() ) {
      stream->writeQuat( part->diffuse );
      stream->writeQuat( part->specular );

      stream->writeInt( textures.index( part->texture[0] ) );
      stream->writeInt( textures.index( part->texture[1] ) );
      stream->writeInt( textures.index( part->texture[2] ) );

      stream->writeInt( part->mode );

      stream->writeShort( part->minIndex );
      stream->writeShort( part->maxIndex );
      stream->writeShort( part->nIndices );
      stream->writeShort( part->firstIndex );
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
