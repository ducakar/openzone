/*
 *  B_Goblin.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.hpp"

#include "B_Goblin.hpp"

#include "Physics.hpp"

namespace oz
{

  const char *B_Goblin::NAME = "B_Goblin";
  const int B_Goblin::TYPE = String::hash( B_Goblin::NAME );

  const Vec3  B_Goblin::CAM_POS            = Vec3( 0.00f, 0.00f, 0.55f );
  const Vec3  B_Goblin::CAM_POS_CROUCH     = Vec3( 0.00f, 0.00f, 0.40f );
  const Vec3  B_Goblin::DIM                = Vec3( 0.24f, 0.24f, 0.64f );
  const Vec3  B_Goblin::DIM_CROUCH         = Vec3( 0.24f, 0.24f, 0.49f );

  const float B_Goblin::BOB_INC            = 0.05f;
  const float B_Goblin::BOB_AMPLITUDE      = 0.05f;

  const float B_Goblin::WALK_VELOCITY      = 0.4f;
  const float B_Goblin::RUN_VELOCITY       = 1.2f;
  const float B_Goblin::CROUCH_VELOCITY    = 0.4f;
  const float B_Goblin::JUMP_VELOCITY      = 4.5f;
  const float B_Goblin::AIR_CONTROL        = 0.15f;
  const float B_Goblin::GRAB_DIST          = 1.0f;

  const float B_Goblin::STEP_INC           = 0.1f;
  const float B_Goblin::STEP_MAX           = 0.5f;

  B_Goblin::B_Goblin( const Vec3 &p_, float h_, float v_, Mind *mind_ )
  {
    p       = p_;
    dim     = DIM;

    flags   = Object::UPDATE_FUNC_BIT | Object::HIT_FUNC_BIT | Object::CLIP_BIT |
        Object::DYNAMIC_BIT | Object::PUSHING_BIT | Object::BOT_BIT;
    type    = TYPE;

    damage  = 2.0f;

    anim    = ANIM_STAND;

    newVelocity.setZero();
    lower   = -1;
    mass    = 50.0f;

    h       = h_;
    v       = v_;

    camPos  = CAM_POS;
    bob     = 0.0f;

    state   = STEPPING_BIT;
    keys    = 0;
    oldKeys = 0;

    mind    = mind_;
  }

  void B_Goblin::onUpdate()
  {
    h = Math::mod( h + 360.0f, 360.0f );
    v = bound( v, -89.0f, 89.0f );

    rotZ = h;

    // { hsine, hcosine, vsine, vcosine, vcosine * hsine, vcosine * hcosine }
    float hvsc[6];

    Math::sincos( Math::rad( h ), &hvsc[0], &hvsc[1] );
    Math::sincos( Math::rad( v ), &hvsc[2], &hvsc[3] );

    hvsc[4] = hvsc[3] * hvsc[0];
    hvsc[5] = hvsc[3] * hvsc[1];

    bool isGrounded = lower >= 0 || ( flags & Object::ON_FLOOR_BIT );
    bool isSwimming = !isGrounded && ( flags & Object::ON_WATER_BIT );

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

    if( ( keys & KEY_JUMP ) && !( oldKeys & KEY_JUMP ) && ( isGrounded || isSwimming ) ) {
      flags &= ~DISABLED_BIT;
      isGrounded = false;

      newVelocity.z = JUMP_VELOCITY;
      addEffect( SND_JUMP );
    }
    if( ( keys & KEY_CROUCH ) && !( oldKeys & KEY_CROUCH ) ) {
      if( state & CROUCHING_BIT ) {
        float oldZ = p.z;

        p.z += DIM.z - DIM_CROUCH.z;
        dim = DIM;

        if( collider.test( *this, this ) ) {
          camPos = CAM_POS;
          state  &= ~CROUCHING_BIT;
        }
        else {
          dim = DIM_CROUCH;
          p.z = oldZ;
        }
      }
      else {
        flags &= ~DISABLED_BIT;

        p.z    += dim.z - DIM_CROUCH.z;
        dim.z  = DIM_CROUCH.z;
        camPos = CAM_POS_CROUCH;

        lower =  -1;
        flags &= ~Object::ON_FLOOR_BIT;
        state |= CROUCHING_BIT;
      }
    }

    float velocity = ( state & CROUCHING_BIT ) ? CROUCH_VELOCITY :
        ( state & RUNNING_BIT ) ? RUN_VELOCITY : WALK_VELOCITY;

    if( !isGrounded || isSwimming ) {
      velocity *= AIR_CONTROL;
    }

    Vec3 move = Vec3::zero();
    state &= ~MOVING_BIT;

    if( keys & KEY_FORWARD ) {
      flags &= ~DISABLED_BIT;
      state |= MOVING_BIT;

      if( isSwimming ) {
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

      if( isSwimming ) {
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

    Vec3 addVelocity = velocity * move;

    if( ( flags & Object::ON_FLOOR_BIT ) && floor.z != 1.0f ) {
      float dot = addVelocity * floor;

      if( dot > 0.0f ) {
        addVelocity -= dot * floor;
      }
    }

    newVelocity += addVelocity;

    if( state & STEPPING_BIT ) {
      Vec3 desiredMove = newVelocity * timer.frameTime;

      collider.translate( *this, desiredMove, this );

      float orgRatio = collider.hit.ratio;

      if( orgRatio != 1.0f && collider.hit.normal.z == 0.0f ) {
        float orgZ = p.z;

        for( float raise = STEP_INC; raise < STEP_MAX; raise += STEP_INC ) {
          p.z += STEP_INC;

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

      collider.translate( p + camPos, look * GRAB_DIST );

      if( collider.hit.sObj != null ) {
        collider.hit.sObj->destroy();
      }
    }

    oldKeys = keys;
    keys = 0;
  }

  void B_Goblin::onHit( const Hit *hit )
  {
    float deltaVel = ( newVelocity - velocity ).sqL();

    if( deltaVel >= 1000.0f ) {
      damage += deltaVel;
    }

    if( hit->normal.z >= Physics::FLOOR_NORMAL_Z && deltaVel > 8.0f ) {
      addEffect( SND_LAND );
    }
  }

  void B_Goblin::onDestroy()
  {
    world.remove( this );
  }

}
