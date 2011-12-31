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
 * @file build/Compiler.cc
 */

#include "stable.hh"

#include "build/Compiler.hh"

#include "client/Context.hh"
#include "client/OpenGL.hh"

namespace oz
{
namespace build
{

Compiler compiler;

void Compiler::enable( int cap )
{
  caps |= cap;
}

void Compiler::disable( int cap )
{
  caps &= ~cap;
}

void Compiler::beginMesh()
{
  hard_assert( !( flags & MESH_BIT ) );
  hard_assert( !( flags & PART_BIT ) );

  flags |= MESH_BIT;
  caps = 0;

  vertices.clear();
  parts.clear();

  part.component      = 0;
  part.texture        = "";
  part.masks          = "";
  part.alpha          = 1.0f;
  part.specular       = 0.0f;

  vert.pos[0]         = 0.0f;
  vert.pos[1]         = 0.0f;
  vert.pos[2]         = 0.0f;

  vert.texCoord[0]    = 0.0f;
  vert.texCoord[1]    = 0.0f;

  vert.detailCoord[0] = 0.0f;
  vert.detailCoord[1] = 0.0f;

  vert.normal[0]      = 0.0f;
  vert.normal[1]      = 0.0f;
  vert.normal[2]      = 0.0f;

  vert.tangent[0]     = 0.0f;
  vert.tangent[1]     = 0.0f;
  vert.tangent[2]     = 0.0f;

  vert.binormal[0]    = 0.0f;
  vert.binormal[1]    = 0.0f;
  vert.binormal[2]    = 0.0f;
}

void Compiler::endMesh()
{
  hard_assert( flags & MESH_BIT );
  hard_assert( !( flags & PART_BIT ) );

  flags &= ~MESH_BIT;
}

void Compiler::component( int id )
{
  hard_assert( flags & MESH_BIT );
  hard_assert( !( flags & PART_BIT ) );

  componentId = id;
}

void Compiler::material( int target, float param )
{
  hard_assert( flags & MESH_BIT );
  hard_assert( !( flags & PART_BIT ) );

  switch( target ) {
    case GL_DIFFUSE: {
      part.alpha = param;
      break;
    }
    case GL_SPECULAR: {
      part.specular = param;
      break;
    }
    default: {
      hard_assert( false );
      break;
    }
  }
}

void Compiler::texture( const char* texture )
{
  hard_assert( flags & MESH_BIT );
  hard_assert( !( flags & PART_BIT ) );

  part.texture = texture;
}

void Compiler::masks( const char* masks )
{
  hard_assert( flags & MESH_BIT );
  hard_assert( !( flags & PART_BIT ) );

  part.masks = masks;
}

void Compiler::begin( uint mode_ )
{
  hard_assert( flags & MESH_BIT );
  hard_assert( !( flags & PART_BIT ) );

  flags |= PART_BIT;

  mode = mode_;
  vertNum = 0;

  part.component = componentId;

  switch( mode ) {
    case GL_QUADS: {
      part.mode = GL_TRIANGLE_STRIP;
      break;
    }
    case GL_POLYGON: {
      part.mode = GL_TRIANGLES;
      break;
    }
    default: {
      part.mode = mode;
      break;
    }
  }
}

void Compiler::end()
{
  hard_assert( flags & MESH_BIT );
  hard_assert( flags & PART_BIT );

  flags &= ~PART_BIT;

  if( caps & CAP_CW ) {
    aReverse<int>( part.indices, part.indices.length() );
  }

  switch( mode ) {
    case GL_POINTS: {
      hard_assert( vertNum >= 1 );
      break;
    }
    case GL_TRIANGLE_STRIP: {
      hard_assert( vertNum >= 3 );
      break;
    }
    case GL_TRIANGLE_FAN: {
      hard_assert( vertNum >= 3 );
      break;
    }
    case GL_TRIANGLES: {
      hard_assert( vertNum >= 3 && vertNum % 3 == 0 );
      break;
    }
    case GL_QUADS: {
      hard_assert( vertNum >= 4 && vertNum % 4 == 0 );

      for( int i = 0; i < vertNum; i += 4 ) {
        swap( part.indices[i + 2], part.indices[i + 3] );
      }
      break;
    }
    case GL_POLYGON: {
      hard_assert( vertNum >= 3 );

      Vector<int> polyIndices = static_cast< Vector<int>&& >( part.indices );
      part.indices.clear();

      int last[2] = { 0, 1 };

      for( int i = 0; i < vertNum / 2; ++i ) {
        int j = ( i + 3 ) / 2;

        if( i & 1 ) {
          part.indices.add( polyIndices[ last[0] ] );
          part.indices.add( polyIndices[ last[1] ] );
          part.indices.add( polyIndices[j] );

          last[1] = j;
        }
        else {
          part.indices.add( polyIndices[ last[0] ] );
          part.indices.add( polyIndices[ last[1] ] );
          part.indices.add( polyIndices[vertNum - j] );

          last[0] = vertNum - j;
        }
      }
      break;
    }
  }

  int partIndex = parts.index( part );

  if( partIndex == -1 ) {
    parts.add( part );
  }
  else {
    if( part.mode == GL_TRIANGLE_STRIP ) {
      // reset triangle strip
      parts[partIndex].indices.add( parts[partIndex].indices.last() );
      parts[partIndex].indices.add( part.indices.first() );
    }
    parts[partIndex].indices.addAll( part.indices, part.indices.length() );
  }

  part.indices.clear();
}

void Compiler::texCoord( float u, float v )
{
  hard_assert( flags & MESH_BIT );

  vert.texCoord[0] = u;
  vert.texCoord[1] = v;
}

void Compiler::texCoord( const float* v )
{
  texCoord( v[0], v[1] );
}

void Compiler::detailCoord( float u, float v )
{
  hard_assert( flags & MESH_BIT );

  vert.detailCoord[0] = u;
  vert.detailCoord[1] = v;
}

void Compiler::detailCoord( const float* v )
{
  detailCoord( v[0], v[1] );
}

void Compiler::normal( float nx, float ny, float nz )
{
  hard_assert( flags & MESH_BIT );

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

  vert.pos[0] = x;
  vert.pos[1] = y;
  vert.pos[2] = z;

  if( !( flags & PART_BIT ) ) {
    vertices.add( vert );
  }
  else {
    bool doRestart = false;

    if( mode == GL_QUADS && vertNum != 0 && vertNum % 4 == 0 ) {
      doRestart = true;
    }

    if( doRestart ) {
      part.indices.add( part.indices.last() );
    }

    int index;

    if( caps & CAP_UNIQUE ) {
      index = vertices.include( vert );
      part.indices.add( index );
    }
    else {
      index = vertices.length();

      vertices.add( vert );
      part.indices.add( index );
    }

    if( doRestart ) {
      part.indices.add( index );
    }

    ++vertNum;
  }
}

void Compiler::vertex( const float* v )
{
  vertex( v[0], v[1], v[2] );
}

void Compiler::animVertex( int i )
{
  vertex( float( i ), 0.0f, 0.0f );
}

void Compiler::getMeshData( MeshData* mesh ) const
{
  hard_assert( !( flags & MESH_BIT ) );
  hard_assert( !( flags & PART_BIT ) );

  int nIndices = 0;

  for( int i = 0; i < parts.length(); ++i ) {
    mesh->parts.add();

    mesh->parts[i].component  = parts[i].component;
    mesh->parts[i].mode       = parts[i].mode;

    mesh->parts[i].texture    = parts[i].texture;
    mesh->parts[i].masks      = parts[i].masks;
    mesh->parts[i].specular   = parts[i].specular;
    mesh->parts[i].alpha      = parts[i].alpha;

    mesh->parts[i].nIndices   = parts[i].indices.length();
    mesh->parts[i].firstIndex = nIndices;

    nIndices += parts[i].indices.length();
  }

  mesh->indices.alloc( nIndices );
  ushort* currIndex = mesh->indices;

  foreach( part, parts.citer() ) {
    foreach( i, part->indices.citer() ) {
      *currIndex = ushort( *i );
      ++currIndex;
    }
  }

  mesh->vertices.alloc( vertices.length() );
  aCopy<Vertex>( mesh->vertices, vertices, vertices.length() );
}

void Compiler::free()
{
  vertices.clear();
  vertices.dealloc();

  parts.clear();
  parts.dealloc();

  part.texture = "";
  part.masks = "";
  part.indices.clear();
  part.indices.dealloc();
}

}
}
