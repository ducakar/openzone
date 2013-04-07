/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#include <stable.hh>
#include <client/MD2.hh>

#include <common/Timer.hh>
#include <client/Context.hh>
#include <client/OpenGL.hh>

namespace oz
{
namespace client
{

const MD2::AnimInfo MD2::ANIM_LIST[] =
{
  // first, last, frequency (FPS), nextAnim
  {   0,  39,  9.0f, ANIM_STAND         }, // STAND
  {  40,  45, 10.0f, ANIM_RUN           }, // RUN
  {  46,  53, 16.0f, ANIM_ATTACK        }, // ATTACK
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

const float MD2::AnimState::MIN_SHOT_INTERVAL_SYNC = 0.2f;

MD2::AnimType MD2::AnimState::extractAnim()
{
  const Weapon* weapon = bot->weapon < 0 ?
                         nullptr : static_cast<const Weapon*>( orbis.objects[bot->weapon] );

  if( bot->state & Bot::DEAD_BIT ) {
    if( ANIM_DEATH_FALLBACK <= currType && nextType <= ANIM_DEATH_FALLBACKSLOW ) {
      return nextType;
    }
    else {
      return AnimType( ANIM_DEATH_FALLBACK + Math::rand( 3 ) );
    }
  }
  else if( bot->cell == nullptr ) {
    return ANIM_CROUCH_STAND;
  }
  else if( ( bot->actions & Bot::ACTION_JUMP ) &&
           !( bot->state & ( Bot::GROUNDED_BIT | Bot::LADDER_BIT | Bot::LEDGE_BIT ) ) )
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
    if( weapon != nullptr && ( prevAttack || weapon->shotTime != 0.0f ) ) {
      prevAttack = weapon->shotTime != 0.0f;

      return bot->state & Bot::CROUCHING_BIT ? ANIM_CROUCH_ATTACK : ANIM_ATTACK;
    }
    else if( prevAttack || bot->meleeTime != 0.0f ) {
      prevAttack = bot->meleeTime != 0.0f;

      return bot->state & Bot::CROUCHING_BIT ? ANIM_CROUCH_ATTACK : ANIM_ATTACK;
    }
    else if( bot->state & Bot::CROUCHING_BIT ) {
      return ANIM_CROUCH_STAND;
    }
    else if( bot->state & Bot::GESTURE_MASK ) {
      if( bot->state & Bot::GESTURE_POINT_BIT ) {
        return ANIM_POINT;
      }
      else if( bot->state & Bot::GESTURE_FALL_BACK_BIT ) {
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
  bot( bot_ ), currType( ANIM_STAND ), nextType( ANIM_STAND ),
  frameRatio( 0.0f ), prevAttack( false )
{
  if( bot->state & Bot::DEAD_BIT ) {
    currType = AnimType( ANIM_DEATH_FALLBACK + Math::rand( 3 ) );
    nextType = currType;
  }

  nextType = extractAnim();

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

  frameFreq = ANIM_LIST[inferredType].frameFreq;

  if( inferredType == ANIM_RUN || inferredType == ANIM_CROUCH_WALK ) {
    const BotClass* clazz = static_cast<const BotClass*>( bot->clazz );

    int   nFrames = lastFrame - firstFrame + 1;
    float stepInc = clazz->stepRunInc;

    if( ( bot->state & ( Bot::CROUCHING_BIT | Bot::WALKING_BIT ) ) || bot->cargo >= 0 ) {
      stepInc = clazz->stepWalkInc;
    }

    frameFreq = float( nFrames ) * stepInc / Timer::TICK_TIME;
  }
  else if( ( inferredType == ANIM_ATTACK || inferredType == ANIM_CROUCH_ATTACK ) ) {
    float shotInterval;
    if( bot->weapon < 0 || orbis.objects[bot->weapon] == nullptr ) {
      const BotClass* clazz = static_cast<const BotClass*>( bot->clazz );

      shotInterval = clazz->meleeInterval;
    }
    else {
      const Weapon*      weapon = static_cast<const Weapon*>( orbis.objects[bot->weapon] );
      const WeaponClass* clazz  = static_cast<const WeaponClass*>( weapon->clazz );

      shotInterval = clazz->shotInterval;
    }

    if( shotInterval >= MIN_SHOT_INTERVAL_SYNC ) {
      int nFrames = lastFrame - firstFrame + 1;

      frameFreq = float( nFrames ) / shotInterval;
    }
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
  id( id_ ), isPreloaded( false ), isLoaded( false )
{}

void MD2::preload()
{
  File file( liber.models[id].path );

  buffer = file.read();
  if( buffer.isEmpty() ) {
    OZ_ERROR( "MD2 model file '%s' read failed", file.path().cstr() );
  }

  isPreloaded = true;
}

void MD2::load()
{
  InputStream is = buffer.inputStream();

  weaponTransf = is.readMat44();
  mesh.load( &is, shader.hasVertexTexture ? GL_STATIC_DRAW : GL_STREAM_DRAW );

  hard_assert( !is.isAvailable() );

  buffer.deallocate();

  isLoaded = true;
}

}
}
