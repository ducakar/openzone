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
 * @file client/DMesh.cc
 */

#include "stable.hh"

#include "client/DMesh.hh"

#include "client/Shader.hh"
#include "client/OpenGL.hh"

namespace oz
{
namespace client
{

Vertex* DMesh::vertexAnimBuffer       = null;
int     DMesh::vertexAnimBufferLength = 0;

DMesh::DMesh() :
  positionsTexId( 0 ), normalsTexId( 0 ), vertices( null ), positions( null ), normals( null )
{}

void DMesh::load( InputStream* istream, const char* path )
{
  if( shader.hasVertexTexture ) {
    Mesh::load( istream, GL_STATIC_DRAW, path );

    nFrames         = istream->readInt();
    nFramePositions = istream->readInt();
    nFrameVertices  = istream->readInt();

    int vertexBufferSize = nFramePositions * nFrames * int( sizeof( Point ) );
    int normalBufferSize = nFramePositions * nFrames * int( sizeof( Vec3 ) );

    glGenTextures( 1, &positionsTexId );
    glBindTexture( GL_TEXTURE_2D, positionsTexId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB16F, nFramePositions, nFrames, 0, GL_RGB, GL_FLOAT,
                  istream->forward( vertexBufferSize ) );
    glBindTexture( GL_TEXTURE_2D, 0 );

    glGenTextures( 1, &normalsTexId );
    glBindTexture( GL_TEXTURE_2D, normalsTexId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB16F, nFramePositions, nFrames, 0, GL_RGB, GL_FLOAT,
                  istream->forward( normalBufferSize ) );
    glBindTexture( GL_TEXTURE_2D, 0 );
  }
  else {
    const char* meshStart = istream->getPos();

    Mesh::load( istream, GL_STREAM_DRAW, path );

    nFrames         = istream->readInt();
    nFramePositions = istream->readInt();
    nFrameVertices  = istream->readInt();

    vertices  = new Vertex[nFrameVertices];
    positions = new Point[nFramePositions * nFrames];
    normals   = new Vec3[nFramePositions * nFrames];

    for( int i = 0; i < nFramePositions * nFrames; ++i ) {
      positions[i] = istream->readPoint();
    }
    for( int i = 0; i < nFramePositions * nFrames; ++i ) {
      normals[i] = istream->readVec3();
    }

    const char* meshEnd = istream->getPos();

    istream->setPos( meshStart );

    istream->readInt();
    istream->readInt();

    istream->readChars( reinterpret_cast<char*>( vertices ),
                        nFrameVertices * int( sizeof( Vertex ) ) );

    istream->setPos( meshEnd );

    if( nFrameVertices > vertexAnimBufferLength ) {
      delete[] vertexAnimBuffer;

      vertexAnimBuffer = new Vertex[nFrameVertices];
      vertexAnimBufferLength = nFrameVertices;
    }
  }
}

void DMesh::unload()
{
  if( vbo != 0 ) {
    if( shader.hasVertexTexture ) {
      glDeleteTextures( 1, &normalsTexId );
      glDeleteTextures( 1, &positionsTexId );
    }
    else {
      delete[] normals;
      delete[] positions;
      delete[] vertices;
    }

    Mesh::unload();
  }
}

void DMesh::drawFrame( int mask, int frame ) const
{
  if( shader.hasVertexTexture ) {
    glActiveTexture( GL_TEXTURE3 );
    glBindTexture( GL_TEXTURE_2D, positionsTexId );
    glActiveTexture( GL_TEXTURE4 );
    glBindTexture( GL_TEXTURE_2D, normalsTexId );
  }
  else {
    const Point* framePositions = &positions[frame * nFramePositions];
    const Vec3*  frameNormals   = &normals[frame * nFramePositions];

    for( int i = 0; i < nFrameVertices; ++i ) {
      int j = int( vertices[i].pos[0] * float( nFramePositions - 1 ) + 0.5f );

      Point pos    = framePositions[j];
      Vec3  normal = frameNormals[j];

      vertexAnimBuffer[i].pos[0] = pos.x;
      vertexAnimBuffer[i].pos[1] = pos.y;
      vertexAnimBuffer[i].pos[2] = pos.z;

      vertexAnimBuffer[i].texCoord[0] = vertices[i].texCoord[0];
      vertexAnimBuffer[i].texCoord[1] = vertices[i].texCoord[1];

      vertexAnimBuffer[i].normal[0] = quantifyToByte( normal.x );
      vertexAnimBuffer[i].normal[1] = quantifyToByte( normal.y );
      vertexAnimBuffer[i].normal[2] = quantifyToByte( normal.z );
    }

    Mesh::upload( vertexAnimBuffer, nFrameVertices, GL_STREAM_DRAW );
  }

  glUniform3f( param.oz_MeshAnimation, float( frame ) / float( nFrames ), 0.0f, 0.0f );

  Mesh::draw( mask );
}

void DMesh::drawAnim( int mask, int firstFrame, int secondFrame, float interpolation ) const
{
  if( shader.hasVertexTexture ) {
    glActiveTexture( GL_TEXTURE3 );
    glBindTexture( GL_TEXTURE_2D, positionsTexId );
    glActiveTexture( GL_TEXTURE4 );
    glBindTexture( GL_TEXTURE_2D, normalsTexId );

    glUniform3f( param.oz_MeshAnimation,
                 float( firstFrame ) / float( nFrames ),
                 float( secondFrame ) / float( nFrames ),
                 interpolation );
  }
  else {
    const Point* currFramePositions = &positions[firstFrame * nFramePositions];
    const Point* nextFramePositions = &positions[secondFrame * nFramePositions];
    const Vec3*  currFrameNormals   = &normals[firstFrame * nFramePositions];
    const Vec3*  nextFrameNormals   = &normals[secondFrame * nFramePositions];

    for( int i = 0; i < nFrameVertices; ++i ) {
      int j = int( vertices[i].pos[0] * float( nFramePositions - 1 ) + 0.5f );

      Point pos    = Math::mix( currFramePositions[j], nextFramePositions[j], interpolation );
      Vec3  normal = Math::mix( currFrameNormals[j],   nextFrameNormals[j],   interpolation );

      vertexAnimBuffer[i].pos[0] = pos.x;
      vertexAnimBuffer[i].pos[1] = pos.y;
      vertexAnimBuffer[i].pos[2] = pos.z;

      vertexAnimBuffer[i].texCoord[0] = vertices[i].texCoord[0];
      vertexAnimBuffer[i].texCoord[1] = vertices[i].texCoord[1];

      vertexAnimBuffer[i].normal[0] = quantifyToByte( normal.x );
      vertexAnimBuffer[i].normal[1] = quantifyToByte( normal.y );
      vertexAnimBuffer[i].normal[2] = quantifyToByte( normal.z );
    }

    Mesh::upload( vertexAnimBuffer, nFrameVertices, GL_STREAM_DRAW );
  }

  Mesh::draw( mask );
}

void DMesh::free()
{
  delete[] vertexAnimBuffer;
  vertexAnimBufferLength = 0;
}

}
}
