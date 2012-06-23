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
  // first, last, frequency (FPS), nextAnim
  {   0,  39,  9.0f, ANIM_STAND         }, // STAND
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

MD2::AnimType MD2::AnimState::extractAnim() const
{
  const Weapon* weapon = bot->weapon < 0 ?
                         null : static_cast<const Weapon*>( orbis.objects[bot->weapon] );

  if( bot->state & Bot::DEAD_BIT ) {
    if( ANIM_DEATH_FALLBACK <= currType && currType <= ANIM_DEATH_FALLBACKSLOW ) {
      return nextType;
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
  else if( bot->cargo < 0 ) {
    if( weapon != null && weapon->shotTime != 0.0f ) {
      return bot->state & Bot::CROUCHING_BIT ? ANIM_CROUCH_ATTACK : ANIM_ATTACK;
    }
    else if( bot->state & Bot::CROUCHING_BIT ) {
      return ANIM_CROUCH_STAND;
    }
    else if( bot->state & Bot::GESTURE_MASK ) {
      if( bot->state & Bot::GESTURE_POINT_BIT ) {
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
      else {
        return ANIM_FLIP;
      }
    }
  }

  if( ANIM_FLIP <= currType && currType <= ANIM_POINT ) {
    return nextType;
  }
  else {
    return bot->state & Bot::CROUCHING_BIT ? ANIM_CROUCH_STAND : ANIM_STAND;
  }
}

MD2::AnimState::AnimState( const Bot* bot_ ) :
  bot( bot_ ), nextType( ANIM_STAND ), frameRatio( 0.0f )
{
  setAnim();

  currFrame = lastFrame;
  nextFrame = lastFrame;
}

void MD2::AnimState::setAnim()
{
  currType   = nextType;
  firstFrame = ANIM_LIST[nextType].firstFrame;
  lastFrame  = ANIM_LIST[nextType].lastFrame;
  frameFreq  = ANIM_LIST[nextType].frameFreq;
}

void MD2::AnimState::advance()
{
  AnimType inferredType = extractAnim();

  if( inferredType == ANIM_RUN || inferredType == ANIM_CROUCH_WALK ) {
    const BotClass* clazz = static_cast<const BotClass*>( bot->clazz );

    float nFrames = float( lastFrame - firstFrame + 1 );
    float stepInc = clazz->stepRunInc;

    if( ( bot->state & ( Bot::CROUCHING_BIT | Bot::WALKING_BIT ) ) || bot->cargo >= 0 ) {
      stepInc = clazz->stepWalkInc;
    }

    frameFreq = nFrames * stepInc / Timer::TICK_TIME;
  }

  frameRatio += timer.frameTime * frameFreq;

  if( frameRatio >= 1.0f ) {
    currType  = nextType;
    nextType  = inferredType;
    currFrame = nextFrame;

    do {
      frameRatio -= 1.0f;

      if( inferredType != currType ) {
        nextType = inferredType;
        setAnim();
        nextFrame = firstFrame;
      }
      else if( firstFrame <= nextFrame && nextFrame < lastFrame ) {
        ++nextFrame;
      }
      else {
        AnimType newType = ANIM_LIST[currType].nextType;

        if( newType == currType ) {
          nextFrame = firstFrame;
        }
        else if( newType != ANIM_NONE ) {
          nextType = newType;
          setAnim();
          nextFrame = firstFrame;
        }
      }
    }
    while( frameRatio >= 1.0f );
  }

  hard_assert( 0.0f <= frameRatio && frameRatio < 1.0f );
}

MD2::MD2( int id_ ) :
  id( id_ ), vertices( null ), positions( null ), normals( null ),
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
    is.readChars( reinterpret_cast<char*>( vertices ), nFrameVertices * int( sizeof( Vertex ) ) );
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

      animBuffer[i].normal[0] = quantifyToByte( normal.x );
      animBuffer[i].normal[1] = quantifyToByte( normal.y );
      animBuffer[i].normal[2] = quantifyToByte( normal.z );
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
                 anim->frameRatio );
  }
  else {
    const Point* currFramePositions = &positions[anim->currFrame * nFramePositions];
    const Point* nextFramePositions = &positions[anim->nextFrame * nFramePositions];
    const Vec3*  currFrameNormals   = &normals[anim->currFrame * nFramePositions];
    const Vec3*  nextFrameNormals   = &normals[anim->nextFrame * nFramePositions];

    for( int i = 0; i < nFrameVertices; ++i ) {
      int j = int( vertices[i].pos[0] * float( nFramePositions - 1 ) + 0.5f );

      Point pos    = Math::mix( currFramePositions[j], nextFramePositions[j], anim->frameRatio );
      Vec3  normal = Math::mix( currFrameNormals[j],   nextFrameNormals[j],   anim->frameRatio );

      animBuffer[i].pos[0] = pos.x;
      animBuffer[i].pos[1] = pos.y;
      animBuffer[i].pos[2] = pos.z;

      animBuffer[i].texCoord[0] = vertices[i].texCoord[0];
      animBuffer[i].texCoord[1] = vertices[i].texCoord[1];

      animBuffer[i].normal[0] = quantifyToByte( normal.x );
      animBuffer[i].normal[1] = quantifyToByte( normal.y );
      animBuffer[i].normal[2] = quantifyToByte( normal.z );
    }

    mesh.upload( animBuffer, nFrameVertices, GL_STREAM_DRAW );
  }

  mesh.draw( Mesh::SOLID_BIT );
}

}
}
