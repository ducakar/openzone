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

namespace oz
{
namespace client
{

#ifdef OZ_BUMPMAP
  Vertex::Vertex( const Point3& pos_, const TexCoord& texCoord_, const Vec3& normal_,
                  const Vec3& tangent_, const Vec3& binormal_ ) :
      pos( pos_ ), texCoord( texCoord_ ), normal( normal_ ),
      tangent( tangent_ ), binormal( binormal_ )
  {}

  bool Vertex::operator == ( const Vertex& v ) const
  {
    return pos == v.pos && texCoord == v.texCoord && normal == v.normal &&
        tangent == v.tangent && binormal == v.binormal;
  }
#else
  Vertex::Vertex( const Point3& pos_, const TexCoord& texCoord_, const Vec3& normal_,
                  const Vec3&, const Vec3& ) :
      pos( pos_ ), texCoord( texCoord_ ), normal( normal_ )
  {}

  bool Vertex::operator == ( const Vertex& v ) const
  {
    return pos == v.pos && texCoord == v.texCoord && normal == v.normal;
  }
#endif

  void Vertex::read( InputStream* stream )
  {
    pos = stream->readPoint3();

    texCoord[0] = stream->readFloat();
    texCoord[1] = stream->readFloat();

    normal   = stream->readVec3();
#ifdef OZ_BUMPMAP
    tangent  = stream->readVec3();
    binormal = stream->readVec3();
#endif
  }

  void Vertex::write( OutputStream* stream ) const
  {
    stream->writePoint3( pos );

    stream->writeFloat( texCoord[0] );
    stream->writeFloat( texCoord[1] );

    stream->writeVec3( normal );
#ifdef OZ_BUMPMAP
    stream->writeVec3( tangent );
    stream->writeVec3( binormal );
#endif
  }

  void Vertex::setFormat()
  {
    glEnableVertexAttribArray( Attrib::POSITION );
    glVertexAttribPointer( Attrib::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                           reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, pos ) );

    glEnableVertexAttribArray( Attrib::TEXCOORD );
    glVertexAttribPointer( Attrib::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                           reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, texCoord ) );

    glEnableVertexAttribArray( Attrib::NORMAL );
    glVertexAttribPointer( Attrib::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                           reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, normal ) );

#ifdef OZ_BUMPMAP
    glEnableVertexAttribArray( Attrib::TANGENT );
    glVertexAttribPointer( Attrib::TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                           reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, tangent ) );

    glEnableVertexAttribArray( Attrib::BINORMAL );
    glVertexAttribPointer( Attrib::BINORMAL, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                           reinterpret_cast<const char*>( 0 ) + offsetof( Vertex, binormal ) );
#endif
  }

  Mesh::Mesh() : vao( 0 )
  {}

  Mesh::~Mesh()
  {
    hard_assert( vao == 0 );
  }

  void Mesh::load( InputStream* stream, uint usage )
  {
    DArray<uint> textures;

    flags = 0;

    int nVertices = stream->readInt();
    int nIndices  = stream->readInt();

#if defined( OZ_BIG_ENDIAN_STREAM ) == defined( OZ_BIG_ENDIAN_ARCH )
    const char* vertices = stream->prepareRead( nVertices * int( sizeof( Vertex ) ) );
    const char* indices  = stream->prepareRead( nIndices *  int( sizeof( ushort ) ) );
#else
    Vertex* vertices = new Vertex[nVertices];
    for( int i = 0; i < nVertices; ++i ) {
      vertices[i].read( stream );
    }

    ushort* indices = new ushort[nIndices];
    for( int i = 0; i < nIndices; ++i ) {
      indices[i] = stream->readShort();
    }
#endif

    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, nVertices * int( sizeof( Vertex ) ), vertices, usage );

    glGenBuffers( 1, &ibo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, nIndices * int( sizeof( ushort ) ), indices, GL_STATIC_DRAW );

    Vertex::setFormat();

    glBindVertexArray( 0 );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

#if defined( OZ_BIG_ENDIAN_STREAM ) != defined( OZ_BIG_ENDIAN_ARCH )
    delete[] indices;
    delete[] vertices;
#endif

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

    int nParts = stream->readInt();
    int firstAlphaPart = stream->readInt();

    hard_assert( ( firstAlphaPart & FIRST_ALPHA_PART_MASK ) == firstAlphaPart );

    flags |= firstAlphaPart;

    if( firstAlphaPart != 0 ) {
      flags |= SOLID_BIT;
    }
    if( firstAlphaPart != parts.length() ) {
      flags |= ALPHA_BIT;
    }

    parts.alloc( nParts );

    for( int i = 0; i < nParts; ++i ) {
      parts[i].texture    = textures[ stream->readInt() ];
      parts[i].alpha      = stream->readFloat();
      parts[i].specular   = stream->readFloat();

      parts[i].mode       = uint( stream->readInt() );

      parts[i].nIndices   = stream->readInt();
      parts[i].firstIndex = stream->readInt();
    }

    textures.dealloc();

    hard_assert( glGetError() == GL_NO_ERROR );
  }

  void Mesh::unload()
  {
    if( vao != 0 ) {
      if( flags & EMBEDED_TEX_BIT ) {
        foreach( part, parts.citer() ) {
          glDeleteTextures( 1, &part->texture );
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

  void Mesh::upload( const Vertex* vertices, int nVertices, uint usage ) const
  {
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, nVertices * int( sizeof( Vertex ) ), vertices, usage );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
  }

  Vertex* Mesh::map( uint access ) const
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

    int firstAlphaPart = flags & FIRST_ALPHA_PART_MASK;

#ifdef OZ_MESA_COMPATIBLE
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    Vertex::setFormat();
#else
    glBindVertexArray( vao );
#endif

    glUniform4fv( param.oz_Colour, 1, shader.colour );

    if( mask & SOLID_BIT ) {
      for( int i = 0; i < firstAlphaPart; ++i ) {
        glBindTexture( GL_TEXTURE_2D, parts[i].texture );
        glUniform1f( param.oz_Specular, parts[i].specular );
        glDrawElements( parts[i].mode, parts[i].nIndices, GL_UNSIGNED_SHORT,
                        reinterpret_cast<const ushort*>( 0 ) + parts[i].firstIndex );
      }
    }
    if( mask & ALPHA_BIT ) {
      glEnable( GL_BLEND );

      for( int i = firstAlphaPart; i < parts.length(); ++i ) {
        glBindTexture( GL_TEXTURE_2D, parts[i].texture );
        glUniform4f( param.oz_Colour, shader.colour.x, shader.colour.y, shader.colour.z,
                     parts[i].alpha );
        glUniform1f( param.oz_Specular, parts[i].specular );
        glDrawElements( parts[i].mode, parts[i].nIndices, GL_UNSIGNED_SHORT,
                        reinterpret_cast<const ushort*>( 0 ) + parts[i].firstIndex );
      }

      glUniform4fv( param.oz_Colour, 1, shader.colour );
      glDisable( GL_BLEND );
    }
  }

#ifdef OZ_SDK
  void MeshData::write( OutputStream* stream, bool embedTextures ) const
  {
    hard_assert( solidParts.length() > 0 || alphaParts.length() > 0 );
    hard_assert( indices.length() > 0 );
    hard_assert( vertices.length() > 0 );

    log.println( "Compiling mesh {" );
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

    foreach( part, solidParts.citer() ) {
      textures.include( part->texture );
    }
    foreach( part, alphaParts.citer() ) {
      textures.include( part->texture );
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

    stream->writeInt( solidParts.length() + alphaParts.length() );
    stream->writeInt( solidParts.length() );

    foreach( part, solidParts.citer() ) {
      stream->writeInt( textures.index( part->texture ) );
      stream->writeFloat( part->alpha );
      stream->writeFloat( part->specular );

      stream->writeInt( int( part->mode ) );

      stream->writeInt( part->nIndices );
      stream->writeInt( part->firstIndex );
    }
    foreach( part, alphaParts.citer() ) {
      stream->writeInt( textures.index( part->texture ) );
      stream->writeFloat( part->alpha );
      stream->writeFloat( part->specular );

      stream->writeInt( int( part->mode ) );

      stream->writeInt( part->nIndices );
      stream->writeInt( part->firstIndex );
    }

    log.unindent();
    log.println( "}" );
  }
#endif

}
}
