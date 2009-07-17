/*
 *  Bot.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Bot.h"

#include "BotClass.h"
#include "Physics.h"

namespace oz
{

  void Bot::onUpdate()
  {
    BotClass &clazz = *(BotClass*) type;

    h = Math::mod( h + 360.0f, 360.0f );
    v = bound( v, -90.0f, 90.0f );

    // { hsine, hcosine, vsine, vcosine, vcosine * hsine, vcosine * hcosine }
    float hvsc[6];

    Math::sincos( Math::rad( h ), &hvsc[0], &hvsc[1] );
    Math::sincos( Math::rad( v ), &hvsc[2], &hvsc[3] );

    hvsc[4] = hvsc[3] * hvsc[0];
    hvsc[5] = hvsc[3] * hvsc[1];

    bool isSwimming = ( flags & UNDER_WATER_BIT ) || ( oldFlags & UNDER_WATER_BIT );
    bool isClimbing = flags & ON_LADDER_BIT;
    bool isGrounded = ( lower >= 0 || ( flags & ON_FLOOR_BIT ) ) && !isSwimming;

    if( ( keys & KEY_FREELOOK ) && !( oldKeys & KEY_FREELOOK ) ) {
      state ^= FREELOOK_BIT;
    }

    if( ( keys & KEY_RUN ) && !( oldKeys & KEY_RUN ) ) {
      state ^= RUNNING_BIT;
    }

    if( ( keys & KEY_STEP ) && !( oldKeys & KEY_STEP ) ) {
      state ^= STEPPING_BIT;
    }

    if( keys & KEY_GESTURE0 ) {
      state |= GESTURE0_BIT;
    }
    else {
      state &= ~GESTURE0_BIT;
    }
    if( keys & KEY_GESTURE1 ) {
      state |= GESTURE1_BIT;
    }
    else {
      state &= ~GESTURE1_BIT;
    }

    if( ( keys & KEY_JUMP ) && !( oldKeys & KEY_JUMP ) &&
        ( isGrounded || ( flags & Object::UNDER_WATER_BIT ) ) )
    {
      flags &= ~DISABLED_BIT;
      isGrounded = false;

      momentum.z = clazz.jumpVelocity;
      addEvent( SND_JUMP );
    }
    if( ( keys & KEY_CROUCH ) && !( oldKeys & KEY_CROUCH ) ) {
      if( state & CROUCHING_BIT ) {
        float oldZ = p.z;

        p.z += clazz.dim.z - clazz.dimCrouch.z;
        dim = clazz.dim;

        if( collider.test( *this, this ) ) {
          camPos = clazz.camPos;
          state  &= ~CROUCHING_BIT;
        }
        else {
          dim = clazz.dimCrouch;
          p.z = oldZ;
        }
      }
      else {
        flags &= ~DISABLED_BIT;
        flags &= ~Object::ON_FLOOR_BIT;
        lower =  -1;

        p.z    += dim.z - clazz.dimCrouch.z;
        dim.z  = clazz.dimCrouch.z;
        camPos = clazz.camPosCrouch;
        state |= CROUCHING_BIT;
      }
    }

    float velocity = ( state & CROUCHING_BIT ) ? clazz.crouchVelocity :
        ( state & RUNNING_BIT ) ? clazz.runVelocity : clazz.walkVelocity;

    if( ( !isGrounded && !isClimbing ) || ( flags & ON_SLICK_BIT ) ) {
      velocity *= clazz.airControl;
    }

    Vec3 move = Vec3::zero();
    state &= ~MOVING_BIT;

    if( keys & KEY_FORWARD ) {
      flags &= ~DISABLED_BIT;
      state |= MOVING_BIT;

      if( isSwimming || isClimbing ) {
        move.x -= hvsc[4];
        move.y += hvsc[5];
        move.z += hvsc[2];
      }
      else {
        move.x -= hvsc[0];
        move.y += hvsc[1];
      }
    }
    if( keys & KEY_BACKWARD ) {
      flags &= ~DISABLED_BIT;
      state |= MOVING_BIT;

      if( isSwimming || isClimbing ) {
        move.x += hvsc[4];
        move.y -= hvsc[5];
        move.z -= hvsc[2];
      }
      else {
        move.x += hvsc[0];
        move.y -= hvsc[1];
      }
    }
    if( keys & KEY_RIGHT ) {
      flags &= ~DISABLED_BIT;
      state |= MOVING_BIT;

      move.x += hvsc[1];
      move.y += hvsc[0];
    }
    if( keys & KEY_LEFT ) {
      flags &= ~DISABLED_BIT;
      state |= MOVING_BIT;

      move.x -= hvsc[1];
      move.y -= hvsc[0];
    }
    if( !move.isZero() && isGrounded ) {
      if( state & CROUCHING_BIT ) {
        anim = ANIM_CROUCH_WALK;
      }
      else {
        anim = ANIM_RUN;
      }
    }
    else {
      if( state & CROUCHING_BIT ) {
        anim = ANIM_CROUCH_STAND;
      }
      else {
        anim = ANIM_STAND;
      }
    }

    if( !move.isZero() ) {
      Vec3 desiredVelocity = velocity * ~move;

      if( ( flags & Object::ON_FLOOR_BIT ) && floor.z != 1.0f ) {
        float dot = desiredVelocity * floor;

        if( dot > 0.0f ) {
          desiredVelocity -= dot * floor;
        }
      }
      momentum += desiredVelocity;
    }

    // TODO: better stepping algoriths (stepping per time unit limit + vertial surface should not
    // be required. Should try step up-forward-down. Should hit a floor surface then.)
    if( ( state & STEPPING_BIT ) && !isClimbing ) {
      Vec3 desiredMove = momentum * timer.frameTime;

      collider.translate( *this, desiredMove, this );

      float orgRatio = collider.hit.ratio;

      if( orgRatio != 1.0f && collider.hit.normal.z == 0.0f ) {
        float orgZ = p.z;

        for( float raise = clazz.stepInc; raise < clazz.stepMax; raise += clazz.stepInc ) {
          p.z += clazz.stepInc;

          if( !collider.test( *this, this ) ) {
            break;
          }
          else {
            collider.translate( *this, desiredMove, this );

            if( collider.hit.ratio > orgRatio + EPSILON ) {
              goto stepSucceeded;
            }
          }
        }
        p.z = orgZ;
        stepSucceeded:;
      }
    }

    // BOT SPECIFIC UPDATE END
    if( ( keys & KEY_USE ) && !( oldKeys & KEY_USE ) ) {
      Vec3 look( -hvsc[4], hvsc[5], hvsc[2] );

      collider.translate( p + camPos, look * clazz.grabDistance );

      if( collider.hit.obj != null ) {
        collider.hit.obj->destroy();
      }
      throw Exception( 0, "drek" );
    }

    oldKeys = keys;
    keys = 0;
  }

  void Bot::onHit( const Hit *hit, float hitVelocity )
  {
    if( hitVelocity >= 30.0f ) {
      damage += hitVelocity;
    }

    if( hit->normal.z >= Physics::FLOOR_NORMAL_Z && hitVelocity < 8.0f ) {
      addEvent( SND_LAND );
    }
  }

  void Bot::onDestroy()
  {}

  Bot::Bot() : anim( ANIM_STAND ), keys( 0 ), oldKeys( 0 ), h( 0.0f ), v( 0.0f ), bob( 0.0f )
  {}

  void Bot::readUpdates( InputStream *istream )
  {
    p         = istream->readVec3();
    flags     = istream->readInt();
    oldFlags  = istream->readInt();
    damage    = istream->readFloat();

    velocity  = istream->readVec3();
    momentum  = istream->readVec3();

    state     = istream->readInt();
    anim      = (AnimEnum) istream->readByte();
    h         = istream->readFloat();

    int nEvents = istream->readByte();
    for( int i = 0; i < nEvents; i++ ) {
      addEvent( istream->readInt() );
    }
    int nEffects = istream->readByte();
    for( int i = 0; i < nEffects; i++ ) {
      addEffect( istream->readInt() );
    }
  }

  void Bot::writeUpdates( OutputStream *ostream )
  {
    ostream->writeVec3( p );
    ostream->writeInt( flags );
    ostream->writeInt( oldFlags );
    ostream->writeFloat( damage );

    ostream->writeVec3( velocity );
    ostream->writeVec3( momentum );

    ostream->writeInt( state );
    ostream->writeByte( anim );
    ostream->writeFloat( h );

    ostream->writeByte( events.length() );
    foreach( event, events.iterator() ) {
      ostream->writeInt( event->id );
    }
    ostream->writeByte( effects.length() );
    foreach( effect, effects.iterator() ) {
      ostream->writeInt( effect->id );
    }
  }

  void Bot::readFull( InputStream *istream )
  {
    p         = istream->readVec3();
    flags     = istream->readInt();
    oldFlags  = istream->readInt();
    damage    = istream->readFloat();

    velocity  = istream->readVec3();
    momentum  = istream->readVec3();

    state     = istream->readInt();
    anim      = (AnimEnum) istream->readByte();
    h         = istream->readFloat();

    int nEvents = istream->readByte();
    for( int i = 0; i < nEvents; i++ ) {
      addEvent( istream->readInt() );
    }
    int nEffects = istream->readByte();
    for( int i = 0; i < nEffects; i++ ) {
      addEffect( istream->readInt() );
    }
  }

  void Bot::writeFull( OutputStream *ostream )
  {
    ostream->writeVec3( p );
    ostream->writeInt( flags );
    ostream->writeInt( oldFlags );
    ostream->writeFloat( damage );

    ostream->writeVec3( velocity );
    ostream->writeVec3( momentum );

    ostream->writeInt( state );
    ostream->writeByte( anim );
    ostream->writeFloat( h );

    ostream->writeByte( events.length() );
    foreach( event, events.iterator() ) {
      ostream->writeInt( event->id );
    }
    ostream->writeByte( effects.length() );
    foreach( effect, effects.iterator() ) {
      ostream->writeInt( effect->id );
    }
  }

}
