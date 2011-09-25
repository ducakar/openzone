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

#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{

#ifndef OZ_BUMPMAP

  Vertex::Vertex( const Point3& pos_, const TexCoord& texCoord_, const Vec3& normal_ )
  {
    pos[0] = pos_.x;
    pos[1] = pos_.y;
    pos[2] = pos_.z;

    texCoord[0] = texCoord_.u;
    texCoord[1] = texCoord_.v;

    normal[0] = normal_.x;
    normal[1] = normal_.y;
    normal[2] = normal_.z;
  }

  bool Vertex::operator == ( const Vertex& v ) const
  {
    return pos[0] == v.pos[0] && pos[1] == v.pos[1] && pos[2] == v.pos[2] &&
        texCoord[0] == v.texCoord[0] && texCoord[1] == v.texCoord[1] &&
        normal[0] == v.normal[0] && normal[1] == v.normal[1] && normal[2] == v.normal[2];
  }

#else

  Vertex::Vertex( const Point3& pos_, const TexCoord& texCoord_, const Vec3& normal_,
                  const Vec3& tangent_, const Vec3& binormal_ )
  {
    pos[0] = pos_.x;
    pos[1] = pos_.y;
    pos[2] = pos_.z;

    texCoord[0] = texCoord_.u;
    texCoord[1] = texCoord_.v;

    normal[0] = normal_.x;
    normal[1] = normal_.y;
    normal[2] = normal_.z;

    tangent[0] = tangent_.x;
    tangent[1] = tangent_.y;
    tangent[2] = tangent_.z;

    binormal[0] = binormal_.x;
    binormal[1] = binormal_.y;
    binormal[2] = binormal_.z;
  }

  bool Vertex::operator == ( const Vertex& v ) const
  {
    return pos[0] == v.pos[0] && pos[1] == v.pos[1] && pos[2] == v.pos[2] &&
        texCoord[0] == v.texCoord[0] && texCoord[1] == v.texCoord[1] &&
        normal[0] == v.normal[0] && normal[1] == v.normal[1] && normal[2] == v.normal[2] &&
        tangent[0] == v.tangent[0] && tangent[1] == v.tangent[1] && tangent[2] == v.tangent[2] &&
        binormal[0] == v.binormal[0] && binormal[1] == v.binormal[1] && binormal[2] == v.binormal[2];
  }

#endif

  void Vertex::read( InputStream* stream )
  {
    pos[0] = stream->readFloat();
    pos[1] = stream->readFloat();
    pos[2] = stream->readFloat();

    texCoord[0] = stream->readFloat();
    texCoord[1] = stream->readFloat();

    normal[0] = stream->readFloat();
    normal[1] = stream->readFloat();
    normal[2] = stream->readFloat();

#ifdef OZ_BUMPMAP
    tangent[0] = stream->readFloat();
    tangent[1] = stream->readFloat();
    tangent[2] = stream->readFloat();

    binormal[0] = stream->readFloat();
    binormal[1] = stream->readFloat();
    binormal[2] = stream->readFloat();
#endif
  }

  void Vertex::write( OutputStream* stream ) const
  {
    stream->writeFloat( pos[0] );
    stream->writeFloat( pos[1] );
    stream->writeFloat( pos[2] );

    stream->writeFloat( texCoord[0] );
    stream->writeFloat( texCoord[1] );

    stream->writeFloat( normal[0] );
    stream->writeFloat( normal[1] );
    stream->writeFloat( normal[2] );

#ifdef OZ_BUMPMAP
    stream->writeFloat( tangent[0] );
    stream->writeFloat( tangent[1] );
    stream->writeFloat( tangent[2] );

    stream->writeFloat( binormal[0] );
    stream->writeFloat( binormal[1] );
    stream->writeFloat( binormal[2] );
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

#ifndef OZ_TOOLS

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

# ifdef OZ_GL_COMPATIBLE
    vao = 1;
# else
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );
# endif

    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, nVertices * int( sizeof( Vertex ) ), 0, usage );

    Vertex* vertices = reinterpret_cast<Vertex*>( glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY ) );

    for( int i = 0; i < nVertices; ++i ) {
      vertices[i].read( stream );
    }

    glUnmapBuffer( GL_ARRAY_BUFFER );

    glGenBuffers( 1, &ibo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, nIndices * int( sizeof( ushort ) ), 0, GL_STATIC_DRAW );

    ushort* indices =
        reinterpret_cast<ushort*>( glMapBuffer( GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY ) );

    for( int i = 0; i < nIndices; ++i ) {
      indices[i] = ushort( stream->readShort() );
    }

    glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );

# ifndef OZ_GL_COMPATIBLE
    Vertex::setFormat();

    glBindVertexArray( 0 );
# endif

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    int nTextures = stream->readInt();

    if( nTextures < 0 ) {
      nTextures = ~nTextures;
      textures.alloc( nTextures );

      flags |= EMBEDED_TEX_BIT;
      textures[0] = 0;

      for( int i = 1; i < nTextures; ++i ) {
        textures[i] = context.readTexture( stream );
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
          texIds[i] = library.textureIndex( name );
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

    OZ_GL_CHECK_ERROR();
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
# ifndef OZ_GL_COMPATIBLE
      glDeleteVertexArrays( 1, &vao );
# endif

      vao = 0;
    }

    OZ_GL_CHECK_ERROR();
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

# ifdef OZ_GL_COMPATIBLE
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    Vertex::setFormat();
# else
    glBindVertexArray( vao );
# endif

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

#else // OZ_TOOLS

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

#endif // OZ_TOOLS

}
}