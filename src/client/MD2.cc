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
 * @file client/MD2.cc
 */

#include "stable.hh"

#include "client/MD2.hh"

#include "client/Context.hh"
#include "client/OpenGL.hh"

namespace oz
{
namespace client
{

const MD2::AnimInfo MD2::ANIM_LIST[] =
{
  // first, last, fps, nextAnim
  {   0,  39,  9.0f, ANIM_STAND         }, // STAND
  {  40,  45,  7.0f, ANIM_WALK          }, // WALK
  {  40,  45, 10.0f, ANIM_RUN           }, // RUN
  {  46,  53, 16.0f, ANIM_NONE          }, // ATTACK
  {  54,  57,  7.0f, ANIM_STAND         }, // PAIN_A
  {  58,  61,  7.0f, ANIM_STAND         }, // PAIN_B
  {  62,  65,  7.0f, ANIM_STAND         }, // PAIN_C
  {  67,  67,  9.0f, ANIM_NONE          }, // JUMP
  {  72,  83,  7.0f, ANIM_STAND         }, // FLIP
  {  84,  94,  7.0f, ANIM_STAND         }, // SALUTE
  {  95, 111, 10.0f, ANIM_STAND         }, // WAVE
  { 112, 122,  7.0f, ANIM_STAND         }, // FALLBACK
  { 123, 134,  6.0f, ANIM_STAND         }, // POINT
  { 135, 153, 10.0f, ANIM_CROUCH_STAND  }, // CROUCH_STAND
  { 154, 159,  7.0f, ANIM_CROUCH_WALK   }, // CROUCH_WALK
  { 160, 168, 18.0f, ANIM_CROUCH_ATTACK }, // CROUCH_ATTACK
  { 169, 172,  7.0f, ANIM_CROUCH_STAND  }, // CROUCH_PAIN
  { 173, 177,  5.0f, ANIM_NONE          }, // CROUCH_DEATH
  { 178, 183,  7.0f, ANIM_NONE          }, // DEATH_FALLBACK
  { 184, 189,  7.0f, ANIM_NONE          }, // DEATH_FALLFORWARD
  { 190, 197,  7.0f, ANIM_NONE          }  // DEATH_FALLBACKSLOW
};

Vertex MD2::animBuffer[MAX_VERTS];

void MD2::AnimState::set( Anim newType )
{
  bool isWalkRunToggle = ( type == ANIM_WALK && newType == ANIM_RUN ) ||
                         ( type == ANIM_RUN && newType == ANIM_WALK );

  type       = newType;
  nextType   = MD2::ANIM_LIST[type].nextType;
  firstFrame = MD2::ANIM_LIST[type].firstFrame;
  lastFrame  = MD2::ANIM_LIST[type].lastFrame;
  fps        = MD2::ANIM_LIST[type].fps;
  frameTime  = 1.0f / fps;

  if( !isWalkRunToggle ) {
    nextFrame = min( firstFrame + 1, lastFrame );
    currTime  = 0.0f;
  }
}

void MD2::AnimState::advance( const Bot* bot )
{
  if( ( type == ANIM_WALK || type == ANIM_RUN || type == ANIM_CROUCH_WALK ) && nextType == type ) {
    int   nFrames = lastFrame - firstFrame + 1;
    float time    = bot->step * float( nFrames ) * frameTime;

    nextFrame = firstFrame + int( bot->step * float( nFrames ) + 1.0f ) % nFrames;

    if( time >= frameTime || ( firstFrame <= currFrame && lastFrame <= currFrame ) ) {
      currFrame = firstFrame + int( bot->step * float( nFrames ) ) % nFrames;
    }

    currTime = Math::fmod( time, frameTime );
  }
  else {
    currTime += timer.frameTime;

    while( currTime > frameTime ) {
      currTime -= frameTime;
      currFrame = nextFrame;

      if( nextType == ANIM_NONE ) {
        nextFrame = min( nextFrame + 1, lastFrame );
      }
      else if( nextType == type ) {
        nextFrame = nextFrame == lastFrame ? firstFrame : nextFrame + 1;
      }
      else {
        set( nextType );
      }
    }
  }
}

MD2::MD2( int id_ ) :
  id( id_ ), vertices( null ), positions( null ), normals( 0 ),
  isPreloaded( false ), isLoaded( false )
{}

MD2::~MD2()
{
  if( shader.hasVertexTexture ) {
    glDeleteTextures( 1, &vertexTexId );
    glDeleteTextures( 1, &normalTexId );
  }
  else {
    delete[] vertices;
    delete[] positions;
    delete[] normals;
  }

  mesh.unload();
}

void MD2::preload()
{
  file.setPath( library.models[id].path );

  if( !file.map() ) {
    throw Exception( "MD2 model file '%s' mmap failed", file.path().cstr() );
  }

  isPreloaded = true;
}

void MD2::load()
{
  InputStream is  = file.inputStream();

  shaderId        = library.shaderIndex( is.readString() );

  nFrames         = is.readInt();
  nFrameVertices  = is.readInt();
  nFramePositions = is.readInt();
  weaponTransf    = is.readMat44();

  if( shader.hasVertexTexture ) {
    int vertexBufferSize = nFramePositions * nFrames * int( sizeof( Vec4 ) );
    int normalBufferSize = nFramePositions * nFrames * int( sizeof( Vec4 ) );

    glGenTextures( 1, &vertexTexId );
    glBindTexture( GL_TEXTURE_2D, vertexTexId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, nFramePositions, nFrames, 0, GL_RGBA, GL_FLOAT,
                  is.forward( vertexBufferSize ) );
    glBindTexture( GL_TEXTURE_2D, 0 );

    glGenTextures( 1, &normalTexId );
    glBindTexture( GL_TEXTURE_2D, normalTexId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, nFramePositions, nFrames, 0, GL_RGBA, GL_FLOAT,
                  is.forward( normalBufferSize ) );
    glBindTexture( GL_TEXTURE_2D, 0 );

    mesh.load( &is, GL_STATIC_DRAW, file.path() );
  }
  else {
    positions = new Point[nFramePositions * nFrames];
    normals   = new Vec3[nFramePositions * nFrames];

    for( int i = 0; i < nFramePositions * nFrames; ++i ) {
      positions[i] = Point::ORIGIN + is.readVec4();
    }

    for( int i = 0; i < nFramePositions * nFrames; ++i ) {
      normals[i] = is.readVec4();
    }

    const char* meshStart = is.getPos();

    is.readInt();
    is.readInt();

    vertices = new Vertex[nFrameVertices];
    for( int i = 0; i < nFrameVertices; ++i ) {
      vertices[i].read( &is );
    }

    is.setPos( meshStart );

    mesh.load( &is, GL_STREAM_DRAW, file.path() );
  }

  file.clear();

  isLoaded = true;
}

void MD2::drawFrame( int frame ) const
{
  shader.use( shaderId );

  if( shader.hasVertexTexture ) {
    glActiveTexture( GL_TEXTURE3 );
    glBindTexture( GL_TEXTURE_2D, vertexTexId );
    glActiveTexture( GL_TEXTURE4 );
    glBindTexture( GL_TEXTURE_2D, normalTexId );
  }
  else {
    const Point* framePositions = &positions[frame * nFramePositions];
    const Vec3*  frameNormals   = &normals[frame * nFramePositions];

    for( int i = 0; i < nFrameVertices; ++i ) {
      int j = int( vertices[i].pos[0] * float( nFramePositions - 1 ) + 0.5f );

      Point pos    = framePositions[j];
      Vec3  normal = frameNormals[j];

      animBuffer[i].pos[0] = pos.x;
      animBuffer[i].pos[1] = pos.y;
      animBuffer[i].pos[2] = pos.z;

      animBuffer[i].texCoord[0] = vertices[i].texCoord[0];
      animBuffer[i].texCoord[1] = vertices[i].texCoord[1];

      animBuffer[i].normal[0] = normal.x;
      animBuffer[i].normal[1] = normal.y;
      animBuffer[i].normal[2] = normal.z;
    }

    mesh.upload( animBuffer, nFrameVertices, GL_STREAM_DRAW );
  }

  glUniform3f( param.oz_MD2Anim, float( frame ) / float( nFrames ), 0.0f, 0.0f );
  tf.apply();

  mesh.draw( Mesh::SOLID_BIT );
}

void MD2::draw( const AnimState* anim ) const
{
  shader.use( shaderId );
  tf.apply();

  if( shader.hasVertexTexture ) {
    glActiveTexture( GL_TEXTURE3 );
    glBindTexture( GL_TEXTURE_2D, vertexTexId );
    glActiveTexture( GL_TEXTURE4 );
    glBindTexture( GL_TEXTURE_2D, normalTexId );

    glUniform3f( param.oz_MD2Anim,
                 float( anim->currFrame ) / float( nFrames ),
                 float( anim->nextFrame ) / float( nFrames ),
                 anim->currTime * anim->fps );
  }
  else {
    const Point* currFramePositions = &positions[anim->currFrame * nFramePositions];
    const Point* nextFramePositions = &positions[anim->nextFrame * nFramePositions];
    const Vec3*  currFrameNormals   = &normals[anim->currFrame * nFramePositions];
    const Vec3*  nextFrameNormals   = &normals[anim->nextFrame * nFramePositions];

    float t = anim->currTime * anim->fps;

    for( int i = 0; i < nFrameVertices; ++i ) {
      int j = int( vertices[i].pos[0] * float( nFramePositions - 1 ) + 0.5f );

      Point pos    = Math::mix( currFramePositions[j], nextFramePositions[j], t );
      Vec3  normal = Math::mix( currFrameNormals[j],   nextFrameNormals[j],   t );

      animBuffer[i].pos[0] = pos.x;
      animBuffer[i].pos[1] = pos.y;
      animBuffer[i].pos[2] = pos.z;

      animBuffer[i].texCoord[0] = vertices[i].texCoord[0];
      animBuffer[i].texCoord[1] = vertices[i].texCoord[1];

      animBuffer[i].normal[0] = normal.x;
      animBuffer[i].normal[1] = normal.y;
      animBuffer[i].normal[2] = normal.z;
    }

    mesh.upload( animBuffer, nFrameVertices, GL_STREAM_DRAW );
  }

  mesh.draw( Mesh::SOLID_BIT );
}

}
}
