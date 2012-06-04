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
  // first, last, frameTime (1.0 / fps), nextAnim
  {   0,  39, 1.0f /  9.0f, ANIM_STAND         }, // STAND
  {  40,  45, 1.0f / 10.0f, ANIM_RUN           }, // RUN
  {  46,  53, 1.0f / 16.0f, ANIM_NONE          }, // ATTACK
  {  54,  57, 1.0f /  7.0f, ANIM_STAND         }, // PAIN_A
  {  58,  61, 1.0f /  7.0f, ANIM_STAND         }, // PAIN_B
  {  62,  65, 1.0f /  7.0f, ANIM_STAND         }, // PAIN_C
  {  67,  67, 1.0f /  9.0f, ANIM_NONE          }, // JUMP
  {  72,  83, 1.0f /  7.0f, ANIM_STAND         }, // FLIP
  {  84,  94, 1.0f /  7.0f, ANIM_STAND         }, // SALUTE
  {  95, 111, 1.0f / 10.0f, ANIM_STAND         }, // WAVE
  { 112, 122, 1.0f /  7.0f, ANIM_STAND         }, // FALLBACK
  { 123, 134, 1.0f /  6.0f, ANIM_STAND         }, // POINT
  { 135, 153, 1.0f / 10.0f, ANIM_CROUCH_STAND  }, // CROUCH_STAND
  { 154, 159, 1.0f /  7.0f, ANIM_CROUCH_WALK   }, // CROUCH_WALK
  { 160, 168, 1.0f / 18.0f, ANIM_CROUCH_ATTACK }, // CROUCH_ATTACK
  { 169, 172, 1.0f /  7.0f, ANIM_CROUCH_STAND  }, // CROUCH_PAIN
  { 173, 177, 1.0f /  5.0f, ANIM_NONE          }, // CROUCH_DEATH
  { 178, 183, 1.0f /  7.0f, ANIM_NONE          }, // DEATH_FALLBACK
  { 184, 189, 1.0f /  7.0f, ANIM_NONE          }, // DEATH_FALLFORWARD
  { 190, 197, 1.0f /  7.0f, ANIM_NONE          }  // DEATH_FALLBACKSLOW
};

Vertex MD2::animBuffer[MAX_VERTS];

MD2::AnimType MD2::AnimState::extractAnim() const
{
  const Weapon* weapon = bot->weapon == -1 ?
                         null : static_cast<const Weapon*>( orbis.objects[bot->weapon] );

  if( bot->state & Bot::DEAD_BIT ) {
    if( type == ANIM_DEATH_FALLBACK || type == ANIM_DEATH_FALLBACKSLOW ||
        type == ANIM_DEATH_FALLFORWARD )
    {
      return type;
    }
    else {
      return AnimType( ANIM_DEATH_FALLBACK + Math::rand( 3 ) );
    }
  }
  else if( bot->cell == null ) {
    return ANIM_CROUCH_STAND;
  }
  else if( ( bot->actions & Bot::ACTION_JUMP ) &&
    !( bot->state & ( Bot::GROUNDED_BIT | Bot::CLIMBING_BIT ) ) )
  {
    return ANIM_JUMP;
  }
  else if( bot->state & Bot::MOVING_BIT ) {
    if( bot->state & Bot::CROUCHING_BIT ) {
      return ANIM_CROUCH_WALK;
    }
    else {
      return ANIM_RUN;
    }
  }
  else if( bot->cargo == -1 ) {
    if( weapon != null && weapon->shotTime != 0.0f ) {
      return bot->state & Bot::CROUCHING_BIT ? ANIM_CROUCH_ATTACK : ANIM_ATTACK;
    }
    else if( bot->state & Bot::CROUCHING_BIT ) {
      return ANIM_CROUCH_STAND;
    }
    else if( bot->state & Bot::GESTURE_POINT_BIT ) {
      return ANIM_POINT;
    }
    else if( bot->state & Bot::GESTURE_BACK_BIT ) {
      return ANIM_FALLBACK;
    }
    else if( bot->state & Bot::GESTURE_SALUTE_BIT ) {
      return ANIM_SALUTE;
    }
    else if( bot->state & Bot::GESTURE_WAVE_BIT ) {
      return ANIM_WAVE;
    }
    else if( bot->state & Bot::GESTURE_FLIP_BIT ) {
      return ANIM_FLIP;
    }
  }
  return bot->state & Bot::CROUCHING_BIT ? ANIM_CROUCH_STAND : ANIM_STAND;
}

MD2::AnimState::AnimState( const Bot* bot_ ) :
  bot( bot_ ), type( ANIM_STAND ), nextType( ANIM_STAND )
{
  set( ANIM_STAND );

  currFrame  = lastFrame;
  nextFrame  = lastFrame;
}

void MD2::AnimState::set( AnimType newType )
{
  type       = newType;
  nextType   = ANIM_LIST[type].nextType;
  firstFrame = ANIM_LIST[type].firstFrame;
  lastFrame  = ANIM_LIST[type].lastFrame;
  nextFrame  = min( firstFrame + 1, lastFrame );
  frameTime  = ANIM_LIST[type].frameTime;
  currTime   = 0.0f;
}

void MD2::AnimState::advance()
{
  nextType = extractAnim();

  currTime += timer.frameTime;

  while( currTime > frameTime ) {
    currTime -= frameTime;
    currFrame = nextFrame;

    if( ( type == ANIM_RUN || type == ANIM_CROUCH_WALK ) &&
        ( nextType == ANIM_RUN || nextType == ANIM_CROUCH_WALK ) )
    {
      const BotClass* clazz = static_cast<const BotClass*>( bot->clazz );

      int   nFrames = lastFrame - firstFrame + 1;
      float frame   = bot->step * float( nFrames );
      float stepInc = ( bot->state & ( Bot::RUNNING_BIT | Bot::CROUCHING_BIT ) ) == Bot::RUNNING_BIT ?
                      clazz->stepRunInc : clazz->stepWalkInc;

      nextFrame = firstFrame + int( frame + 1.0f ) % nFrames;
      frameTime = Timer::TICK_TIME / ( float( nFrames ) * stepInc );
    }
    else if( ( type == ANIM_ATTACK && nextType == ANIM_ATTACK ) ||
             ( type == ANIM_CROUCH_ATTACK && nextType == ANIM_CROUCH_ATTACK ) )
    {
      if( bot->weapon == -1 ) {
        goto normalAnim;
      }

      const Weapon* weapon = static_cast<const Weapon*>( orbis.objects[bot->weapon] );
      if( weapon == null ) {
        goto normalAnim;
      }

      const WeaponClass* clazz = static_cast<const WeaponClass*>( weapon->clazz );

      int   nFrames = lastFrame - firstFrame + 1;
      float frame   = weapon->shotTime / clazz->shotInterval * float( nFrames );

      nextFrame = firstFrame + int( frame + 1.0f ) % nFrames;
      frameTime = clazz->shotInterval / float( nFrames );
    }
    else if( nextType == ANIM_NONE ) {
      nextFrame = min( nextFrame + 1, lastFrame );
    }
    else if( nextType == type ) {
    normalAnim:
      nextFrame = nextFrame == lastFrame ? firstFrame : nextFrame + 1;
    }
    else {
      set( nextType );
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

  file.setPath( "" );

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
                 anim->currTime / anim->frameTime );
  }
  else {
    const Point* currFramePositions = &positions[anim->currFrame * nFramePositions];
    const Point* nextFramePositions = &positions[anim->nextFrame * nFramePositions];
    const Vec3*  currFrameNormals   = &normals[anim->currFrame * nFramePositions];
    const Vec3*  nextFrameNormals   = &normals[anim->nextFrame * nFramePositions];

    float t = anim->currTime / anim->frameTime;

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
