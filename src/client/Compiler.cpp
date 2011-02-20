/*
 *  Mesh.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Compiler.hpp"

#include "client/Colours.hpp"

#define GL_GLEXT_PROTOTYPES
#include <SDL_opengl.h>
#include "Context.hpp"

namespace oz
{
namespace client
{

  Compiler compiler;

  void Compiler::beginMesh()
  {
    hard_assert( ~flags & MESH_BIT );
    hard_assert( ~flags & MATERIAL_BIT );
    hard_assert( ~flags & SURFACE_BIT );

    flags |= MESH_BIT;

    vertices.clear();
    elements.clear();
    parts.clear();

    part.diffuse    = Colours::WHITE;
    part.specular   = Colours::BLACK;
    part.texture[0] = GL_NONE;
    part.texture[1] = GL_NONE;
    part.flags      = 0;

    vert.pos[0] = 0.0f;
    vert.pos[1] = 0.0f;
    vert.pos[2] = 0.0f;

    vert.normal[0] = 0.0f;
    vert.normal[1] = 0.0f;
    vert.normal[2] = 0.0f;

    vert.texCoord[0] = 0.0f;
    vert.texCoord[1] = 0.0f;
  }

  void Compiler::endMesh()
  {
    hard_assert( flags & MESH_BIT );
    hard_assert( ~flags & MATERIAL_BIT );
    hard_assert( ~flags & SURFACE_BIT );

    flags &= ~MESH_BIT;
  }

  void Compiler::beginMaterial()
  {
    hard_assert( flags & MESH_BIT );
    hard_assert( ~flags & MATERIAL_BIT );
    hard_assert( ~flags & SURFACE_BIT );

    flags |= MATERIAL_BIT;

    part.firstElement = elements.length();
  }

  void Compiler::endMaterial()
  {
    hard_assert( flags & MESH_BIT );
    hard_assert( flags & MATERIAL_BIT );
    hard_assert( ~flags & SURFACE_BIT );

    flags &= ~MATERIAL_BIT;

    part.nElements = elements.length() - part.firstElement;
    parts.add( part );
  }

  void Compiler::material( int face, int target, const float* params )
  {
    hard_assert( flags & MESH_BIT );
    hard_assert( flags & MATERIAL_BIT );
    hard_assert( ~flags & SURFACE_BIT );

    hard_assert( face == GL_FRONT );

    switch( target ) {
      case GL_DIFFUSE: {
        part.diffuse.x = params[0];
        part.diffuse.y = params[1];
        part.diffuse.z = params[2];
        part.diffuse.w = params[3];
        break;
      }
      case GL_SPECULAR: {
        part.specular.x = params[0];
        part.specular.y = params[1];
        part.specular.z = params[2];
        part.specular.w = params[3];
        break;
      }
      default: {
        hard_assert( false );
        break;
      }
    }
  }

  void Compiler::setTexture( int unit, int target, const char* texture )
  {
    hard_assert( flags & MESH_BIT );
    hard_assert( flags & MATERIAL_BIT );
    hard_assert( ~flags & SURFACE_BIT );

    hard_assert( target == GL_TEXTURE_2D );

    switch( unit ) {
      case GL_TEXTURE0: {
        part.texture[0] = texture;
        break;
      }
      case GL_TEXTURE1: {
        part.texture[1] = texture;
        break;
      }
      default: {
        hard_assert( false );
        break;
      }
    }
  }

  void Compiler::texCoord( float s, float t )
  {
    hard_assert( flags & MESH_BIT );
    hard_assert( flags & MATERIAL_BIT );
    hard_assert( ~flags & SURFACE_BIT );

    vert.texCoord[0] = s;
    vert.texCoord[1] = t;
  }

  void Compiler::texCoord( const float* v )
  {
    texCoord( v[0], v[1] );
  }

  void Compiler::normal( float nx, float ny, float nz )
  {
    hard_assert( flags & MESH_BIT );
    hard_assert( flags & MATERIAL_BIT );
    hard_assert( ~flags & SURFACE_BIT );

    vert.normal[0] = nx;
    vert.normal[1] = ny;
    vert.normal[2] = nz;
  }

  void Compiler::normal( const float* v )
  {
    normal( v[0], v[1], v[2] );
  }

  void Compiler::vertex( float x, float y, float z )
  {
    hard_assert( flags & MESH_BIT );
    hard_assert( flags & MATERIAL_BIT );
    hard_assert( ~flags & SURFACE_BIT );

    vert.pos[0] = x;
    vert.pos[1] = y;
    vert.pos[2] = z;

    vertices.add( vert );
  }

  void Compiler::vertex( const float* v )
  {
    vertex( v[0], v[1], v[2] );
  }

  void Compiler::begin( int mode_ )
  {
    hard_assert( flags & MESH_BIT );
    hard_assert( flags & MATERIAL_BIT );
    hard_assert( ~flags & SURFACE_BIT );

    flags |= SURFACE_BIT;

    mode = mode_;
    vertNum = 0;

    hard_assert( mode == GL_POINTS || mode == GL_LINES || mode == GL_LINE_STRIP ||
        mode == GL_LINE_LOOP || mode == GL_TRIANGLE_STRIP || mode == GL_TRIANGLES );
  }

  void Compiler::end()
  {
    hard_assert( flags & MESH_BIT );
    hard_assert( flags & MATERIAL_BIT );
    hard_assert( flags & SURFACE_BIT );

    flags &= ~SURFACE_BIT;

    // TODO GL_TRIANGLE_STRIP, GL_QUADS, GL_QUAD_STRIP, GL_POLYGON

    hard_assert( mode != GL_TRIANGLE_STRIP || vertNum >= 3 );
    hard_assert( mode != GL_TRIANGLE_FAN || vertNum >= 3 );
    hard_assert( mode != GL_TRIANGLES || vertNum == 3 );
    hard_assert( mode != GL_QUADS || vertNum == 4 );
  }

  void Compiler::element( ushort index )
  {
    hard_assert( flags & MESH_BIT );
    hard_assert( flags & MATERIAL_BIT );
    hard_assert( ( flags & SURFACE_BIT ) || mode == GL_TRIANGLE_STRIP );

    // add additional vertices to restart triangle strip if neccesary
    if( mode == GL_TRIANGLES && vertNum == 3 ) {
      elements.add( elements.last() );
      vertNum = -1;
    }
    else if( mode == GL_QUADS && vertNum == 4 ) {
      elements.add( elements.last() );
      vertNum = -1;
    }

    ++vertNum;

    elements.add( index );

    // if this is first vertex of not-first primitive, add additional vertex to restart strip
    if( vertNum == 0 ) {
      elements.add( index );
      ++vertNum;
    }
  }

  void Compiler::writeMesh( OutputStream* stream ) const
  {
    hard_assert( ~flags & MESH_BIT );
    hard_assert( ~flags & MATERIAL_BIT );
    hard_assert( ~flags & SURFACE_BIT );

    stream->writeInt( vertices.length() );
    for( int i = 0; i < vertices.length(); ++i ) {
      stream->writeFloat( vertices[i].pos[0] );
      stream->writeFloat( vertices[i].pos[1] );
      stream->writeFloat( vertices[i].pos[2] );

      stream->writeFloat( vertices[i].normal[0] );
      stream->writeFloat( vertices[i].normal[1] );
      stream->writeFloat( vertices[i].normal[2] );

      stream->writeFloat( vertices[i].texCoord[0] );
      stream->writeFloat( vertices[i].texCoord[1] );
    }

    stream->writeInt( elements.length() );
    for( int i = 0; i < elements.length(); ++i ) {
      stream->writeInt( elements[i] );
    }

    stream->writeInt( parts.length() );
    for( int i = 0; i < parts.length(); ++i ) {
      stream->writeQuat( parts[i].diffuse );
      stream->writeQuat( parts[i].specular );

      stream->writeString( parts[i].texture[0] );
      stream->writeString( parts[i].texture[1] );
      stream->writeString( parts[i].texture[2] );

      stream->writeInt( parts[i].flags );

      stream->writeInt( parts[i].firstElement );
      stream->writeInt( parts[i].nElements );
    }
  }

  void Compiler::getMesh( Mesh* mesh ) const
  {
    hard_assert( ~flags & MESH_BIT );
    hard_assert( ~flags & MATERIAL_BIT );
    hard_assert( ~flags & SURFACE_BIT );

    uint buffers[2];

    glGenBuffers( 2, buffers );

    glBindBuffer( GL_ARRAY_BUFFER, buffers[0] );
    glBufferData( GL_ARRAY_BUFFER, vertices.length() * sizeof( Vertex ), vertices,
                  GL_STATIC_DRAW );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffers[1] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, elements.length() * sizeof( uint ), elements,
                  GL_STATIC_DRAW );

    mesh->arrayBuffer = buffers[0];
    mesh->elementBuffer = buffers[1];

    for( int i = 0; i < parts.length(); ++i ) {
      mesh->parts.add();

      mesh->parts[i].diffuse      = parts[i].diffuse;
      mesh->parts[i].specular     = parts[i].specular;
      mesh->parts[i].texture[0]   = context.loadTexture( parts[i].texture[0] );
      mesh->parts[i].texture[1]   = context.loadTexture( parts[i].texture[1] );
      mesh->parts[i].texture[2]   = context.loadTexture( parts[i].texture[2] );

      mesh->parts[i].flags        = parts[i].flags;

      mesh->parts[i].firstElement = parts[i].firstElement;
      mesh->parts[i].nElements    = parts[i].nElements;
    }
  }

}
}
