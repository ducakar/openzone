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
 * @file matrix/Bot.cc
 */

#include "stable.hh"

#include "matrix/Bot.hh"

#include "matrix/Weapon.hh"
#include "matrix/NamePool.hh"
#include "matrix/Collider.hh"
#include "matrix/Physics.hh"
#include "matrix/Synapse.hh"

namespace oz
{
namespace matrix
{

constexpr float Bot::AIR_FRICTION;
constexpr float Bot::HIT_HARD_THRESHOLD;

constexpr float Bot::WOUNDED_THRESHOLD;
constexpr float Bot::CORPSE_FADE_FACTOR;

constexpr float Bot::INSTRUMENT_DIST;
constexpr float Bot::INSTRUMENT_DOT_MIN;

constexpr float Bot::GRAB_EPSILON;
constexpr float Bot::GRAB_STRING_RATIO;
constexpr float Bot::GRAB_HANDLE_TOL;
constexpr float Bot::GRAB_MOM_RATIO;
constexpr float Bot::GRAB_MOM_MAX;
constexpr float Bot::GRAB_MOM_MAX_SQ;

constexpr float Bot::STEP_MOVE_AHEAD;
constexpr float Bot::CLIMB_MOVE_AHEAD;

Pool<Bot, 1024> Bot::pool;

void Bot::onDestroy()
{
  // only play death sound when an alive bot is destroyed but not when a body is destroyed
  if( !( state & DEAD_BIT ) ) {
    addEvent( EVENT_DEATH, 1.0f );
  }

  Dynamic::onDestroy();
}

void Bot::onHit( const Hit* hit, float hitMomentum )
{
  if( state & DEAD_BIT ) {
    return;
  }

  if( hit->normal.z >= Physics::FLOOR_NORMAL_Z ) {
    hard_assert( hitMomentum <= 0.0f );

    addEvent( EVENT_LAND, 1.0f );
  }
  else if( hitMomentum < HIT_HARD_THRESHOLD ) {
    addEvent( EVENT_HIT_HARD, 1.0f );
  }
}

void Bot::onUpdate()
{
  const BotClass* clazz = static_cast<const BotClass*>( this->clazz );

  Dynamic* cargoObj  = cargo  == -1 ? null : static_cast<Dynamic*>( orbis.objects[cargo] );
  Weapon*  weaponObj = weapon == -1 ? null : static_cast<Weapon*>( orbis.objects[weapon] );

  if( weaponObj == null ) {
    weapon = -1;

    if( cargoObj == null ) {
      cargo = -1;
    }
  }
  else {
    cargo = -1;
  }

  hard_assert( cargoObj  != static_cast<const Dynamic*>( this ) );
  hard_assert( weaponObj != static_cast<const Dynamic*>( this ) );

  if( life < clazz->life / 2.0f ) {
    if( life > 0.0f ) {
      if( !( state & DEAD_BIT ) ) {
        kill();
      }
      else {
        if( dim != clazz->corpseDim && !collider.overlaps( AABB( p, clazz->corpseDim ), this ) ) {
          dim = clazz->corpseDim;
        }

        life -= clazz->life * CORPSE_FADE_FACTOR;
        // we don't want Object::destroy() to be called when body dissolves (destroy() causes
        // sounds and frags to fly around), that's why we just remove the object
        if( life <= 0.0f ) {
          synapse.remove( this );
        }
      }
    }
    return;
  }

  if( actions & ~oldActions & ACTION_SUICIDE ) {
    kill();
    return;
  }

  hard_assert( 0.0f <= h && h < Math::TAU );
  hard_assert( 0.0f <= v && v <= Math::TAU / 2.0f );

  life    = min( life + clazz->regeneration, clazz->life );
  stamina = min( stamina + clazz->staminaGain, clazz->stamina );

  if( parent != -1 ) {
    Object* vehicle = orbis.objects[parent];

    if( vehicle == null ) {
      exit();
    }
  }
  else {
    /*
     * STATE
     */
    state &= ~( GROUNDED_BIT | ON_STAIRS_BIT | CLIMBING_BIT | SWIMMING_BIT | SUBMERGED_BIT |
        CARGO_BIT | ATTACKING_BIT );

    state |= lower != -1 || ( flags & ON_FLOOR_BIT ) ? GROUNDED_BIT  : 0;
    state |= ( flags & ON_LADDER_BIT )               ? CLIMBING_BIT  : 0;
    state |= depth > dim.z                           ? SWIMMING_BIT  : 0;
    state |= depth > dim.z + camZ                    ? SUBMERGED_BIT : 0;
    state |= cargo != -1                             ? CARGO_BIT     : 0;

    flags |= CLIMBER_BIT;

    if( state & SUBMERGED_BIT ) {
      stamina -= clazz->staminaWaterDrain;

      if( stamina < 0.0f ) {
        life += stamina;
        stamina = 0.0f;
      }
    }

    stepRate -= velocity.x*velocity.x + velocity.y*velocity.y;
    stepRate *= clazz->stepRateSupp;

    /*
     * JUMP, CROUCH
     */

    // We want the player to press the key for jump each time, so logical consequence would be to
    // jump when jump key becomes pressed. But then a jump may be missed if we are in air for just
    // a brief period of time, e.g. when swimming or running down the hill (at those occasions the
    // bot is not in water/on floor all the time, but may fly for a few frames in the mean time).
    // So, if we press the jump key, we schedule for a jump, and when jump conditions are met,
    // the jump will be commited if we still hold down the jump key.
    if( actions & ACTION_JUMP ) {
      if( !( oldActions & ACTION_JUMP ) ) {
        state |= JUMP_SCHED_BIT;
      }
      if( ( state & JUMP_SCHED_BIT ) && ( state & ( GROUNDED_BIT | SWIMMING_BIT ) ) &&
          cargo == -1 && stamina >= clazz->staminaJumpDrain )
      {
        flags &= ~( DISABLED_BIT | ON_FLOOR_BIT | ON_SLICK_BIT );
        lower = -1;
        state &= ~GROUNDED_BIT;
        stamina -= clazz->staminaJumpDrain;

        momentum.z = clazz->jumpMomentum;
        addEvent( EVENT_JUMP, 1.0f );

        state &= ~JUMP_SCHED_BIT;
      }
    }
    else {
      state &= ~JUMP_SCHED_BIT;
    }

    if( actions & ~oldActions & ACTION_CROUCH ) {
      if( state & CROUCHING_BIT ) {
        float oldZ = p.z;

        p.z = oldZ + clazz->dim.z - clazz->crouchDim.z;
        dim = clazz->dim;

        if( !collider.overlaps( this, this ) ) {
          camZ  = clazz->camZ;
          state &= ~CROUCHING_BIT;
        }
        else {
          p.z = oldZ - clazz->dim.z + clazz->crouchDim.z;

          if( !collider.overlaps( this, this ) ) {
            camZ  = clazz->camZ;
            state &= ~CROUCHING_BIT;
          }
          else {
            dim = clazz->crouchDim;
            p.z = oldZ;
          }
        }
      }
      else {
        flags &= ~DISABLED_BIT;
        flags &= ~ON_FLOOR_BIT;
        lower =  -1;

        p.z    += dim.z - clazz->crouchDim.z;
        dim.z  = clazz->crouchDim.z;
        camZ   = clazz->crouchCamZ;
        state  |= CROUCHING_BIT;
      }
    }
    if( stamina < clazz->staminaRunDrain || life < WOUNDED_THRESHOLD * clazz->life ) {
      state &= ~RUNNING_BIT;
    }

    /*
     * MOVE
     */

    // { hsine, hcosine, vsine, vcosine, vsine * hsine, vsine * hcosine }
    float hvsc[6];

    Math::sincos( h, &hvsc[0], &hvsc[1] );
    Math::sincos( v, &hvsc[2], &hvsc[3] );

    hvsc[4] = hvsc[2] * hvsc[0];
    hvsc[5] = hvsc[2] * hvsc[1];

    if( !( state & ( GROUNDED_BIT | SWIMMING_BIT ) ) ) {
      momentum.x *= 1.0f - AIR_FRICTION;
      momentum.y *= 1.0f - AIR_FRICTION;
    }

    Vec3 move = Vec3::ZERO;
    state &= ~MOVING_BIT;

    if( actions & ACTION_FORWARD ) {
      // for now CLIMBING_BIT is equivalent to ON_LADDER_BIT in flags since ledge climbing has not
      // been processed yet
      if( state & ( CLIMBING_BIT | SWIMMING_BIT ) ) {
        move.x -= hvsc[4];
        move.y += hvsc[5];
        move.z -= hvsc[3];
      }
      else {
        move.x -= hvsc[0];
        move.y += hvsc[1];
      }
    }
    if( actions & ACTION_BACKWARD ) {
      if( state & ( CLIMBING_BIT | SWIMMING_BIT ) ) {
        move.x += hvsc[4];
        move.y -= hvsc[5];
        move.z += hvsc[3];
      }
      else {
        move.x += hvsc[0];
        move.y -= hvsc[1];
      }
    }
    if( actions & ACTION_RIGHT ) {
      move.x += hvsc[1];
      move.y += hvsc[0];
    }
    if( actions & ACTION_LEFT ) {
      move.x -= hvsc[1];
      move.y -= hvsc[0];
    }

    if( move != Vec3::ZERO ) {
      flags &= ~DISABLED_BIT;
      state |= MOVING_BIT;

      move = ~move;

      /*
       * Ledge climbing
       *
       * First, check if bot's going to hit an obstacle soon. If it does, check whether it would
       * have moved further if we raised it (over the obstacle). We check different heights
       * (those are specified in configuration file: climbInc and climbMax). To prevent climbing
       * on high slopes, we must check that we step over an edge. In other words:
       *
       *      .                                  Start and end position must be on different sides
       *  end  .     end of a failed attempt     of the obstacle side plane we collided to.
       *     \  .   /
       *      o  . x
       * ----------     collision point
       *           \   |
       *            \  |         start
       *             \ |        /
       *              \x<------o
       *               \----------
       *
       * If this succeeds, check also that the "ledge" actually exists. We move the bot down and if
       * it hits a relatively horizontal surface (Physics::FLOOR_NORMAL_Z), proceed with climbing.
       */
      if( ( actions & ( ACTION_FORWARD | ACTION_JUMP ) ) == ( ACTION_FORWARD | ACTION_JUMP ) &&
          !( state & CLIMBING_BIT ) && stamina > clazz->staminaClimbDrain )
      {
        // check if bot's gonna hit a wall soon
        Vec3 desiredMove = CLIMB_MOVE_AHEAD * Vec3( move.x, move.y, 0.0f );

        collider.translate( this, desiredMove );

        if( collider.hit.ratio != 1.0f && collider.hit.normal.z < Physics::FLOOR_NORMAL_Z ) {
          // check how far upwards we can raise
          Vec3  normal    = collider.hit.normal;
          float startDist = 4.0f * EPSILON - ( desiredMove * collider.hit.ratio ) * normal;
          float originalZ = p.z;

          collider.translate( this, Vec3( 0.0f, 0.0f, clazz->climbMax ) );

          float maxRaise = collider.hit.ratio * clazz->climbMax;

          // for each height check if we can move forwards for desiredMove
          for( float raise = clazz->stepMax; raise <= maxRaise; raise += clazz->climbInc ) {
            p.z += clazz->climbInc;

            collider.translate( this, desiredMove );

            Vec3 move = desiredMove * collider.hit.ratio;
            float endDist = startDist + Vec3( move.x, move.y, move.z + raise ) * normal;

            if( endDist < 0.0f ) {
              // check if ledge has a normal.z >= FLOOR_NORMAL_Z
              Point3 raisedPos = p;
              p += move;

              collider.translate( this, Vec3( 0.0f, 0.0f, -raise ) );

              p = raisedPos;

              if( collider.hit.ratio != 1.0f && collider.hit.normal.z >= Physics::FLOOR_NORMAL_Z ) {
                momentum.x    *= 1.0f - Physics::LADDER_FRICTION;
                momentum.y    *= 1.0f - Physics::LADDER_FRICTION;
                momentum.z    = max( momentum.z, clazz->climbMomentum );

                instrument    = -1;
                cargo         = -1;
                state         |= CLIMBING_BIT;
                state         &= ~JUMP_SCHED_BIT;
                stamina       -= clazz->staminaClimbDrain;
                break;
              }
            }
          }

          p.z = originalZ;
        }
      }

      /*
       * STEPPING OVER OBSTACLES
       *
       * First, check if bot's going to hit an obstacle in the next frame. If it does, check whether
       * it would have moved further if we raised it a bit (over the obstacle). We check different
       * heights (those are specified in configuration file: stepInc and stepMax). To prevent that
       * stepping would result in "climbing" high slopes, we must check that we step over an edge.
       * In other words:
       *
       *      .                                  Start and end position must be on different sides
       *  end  .     end of a failed attempt     of the obstacle side plane we collided to.
       *     \  .   /
       *      o  . x
       * ----------     collision point
       *           \   |
       *            \  |         start
       *             \ |        /
       *              \x<------o
       *               \----------
       */
      if( !( state & CLIMBING_BIT ) && stepRate <= clazz->stepRateLimit ) {
        // check if bot's gonna hit a stair in the next frame
        Vec3 desiredMove = STEP_MOVE_AHEAD * move;

        collider.translate( this, desiredMove );

        if( collider.hit.ratio != 1.0f && collider.hit.normal.z < Physics::FLOOR_NORMAL_Z ) {
          Vec3  normal    = collider.hit.normal;
          float startDist = 2.0f * EPSILON - ( desiredMove * collider.hit.ratio ) * normal;
          float originalZ = p.z;

          collider.translate( this, Vec3( 0.0f, 0.0f, clazz->stepMax + 2.0f * EPSILON ) );

          float maxRaise = collider.hit.ratio * clazz->stepMax;

          for( float raise = clazz->stepInc; raise <= maxRaise; raise += clazz->stepInc ) {
            p.z += clazz->stepInc;
            collider.translate( this, desiredMove );

            Vec3 move = desiredMove * collider.hit.ratio;
            move.z += raise;
            float endDist = startDist + move * normal;

            if( endDist < 0.0f ) {
              stepRate += raise*raise * clazz->stepRateCoeff;
              goto stepSucceeded;
            }
          }
          p.z = originalZ;

        stepSucceeded:;
        }
      }

      Vec3 desiredMomentum = move;

      if( state & CROUCHING_BIT ) {
        desiredMomentum *= clazz->crouchMomentum;
      }
      else if( ( state & RUNNING_BIT ) && cargo == -1 ) {
        desiredMomentum *= clazz->runMomentum;
      }
      else {
        desiredMomentum *= clazz->walkMomentum;
      }

      if( flags & ON_SLICK_BIT ) {
        desiredMomentum *= clazz->slickControl;
      }
      else if( state & CLIMBING_BIT ) {
        if( actions & ACTION_JUMP ) {
          desiredMomentum = Vec3::ZERO;
        }
        else {
          desiredMomentum *= clazz->climbControl;
        }
      }
      else if( state & SWIMMING_BIT ) {
        // not on static ground
        if( !( flags & ON_FLOOR_BIT ) &&
            !( lower != -1 && ( orbis.objects[lower]->flags & Object::DISABLED_BIT ) ) )
        {
          desiredMomentum *= clazz->waterControl;
        }
      }
      else if( !( state & GROUNDED_BIT ) ) {
        desiredMomentum *= clazz->airControl;
      }

      if( ( flags & ( ON_FLOOR_BIT | IN_WATER_BIT ) ) == ON_FLOOR_BIT && floor.z != 1.0f ) {
        float dot = desiredMomentum * floor;

        if( dot > 0.0f ) {
          desiredMomentum -= dot * floor;
        }
      }

      momentum += desiredMomentum;

      if( ( state & RUNNING_BIT ) && ( state & ( GROUNDED_BIT | SWIMMING_BIT | CLIMBING_BIT ) ) ) {
        stamina -= clazz->staminaRunDrain;
      }
    }

    /*
     * ATTACK & GESTURES
     */

    if( !( state & MOVING_BIT ) && cargo == -1 &&
        ( !( actions & ACTION_JUMP ) || ( state & ( Bot::GROUNDED_BIT | Bot::CLIMBING_BIT ) ) ) )
    {
      if( actions & ACTION_ATTACK ) {
        if( weaponObj != null ) {
          state |= ATTACKING_BIT;
          weaponObj->trigger( this );
        }
      }
      else if( !( state & CROUCHING_BIT ) ) {
        if( actions & ACTION_GESTURE0 ) {
          if( !( state & GESTURE0_BIT ) ) {
            state &= ~( GESTURE0_BIT | GESTURE1_BIT | GESTURE2_BIT | GESTURE3_BIT | GESTURE4_BIT );
            state |= GESTURE0_BIT;
          }
        }
        else if( actions & ACTION_GESTURE1 ) {
          if( !( state & GESTURE1_BIT ) ) {
            state &= ~( GESTURE0_BIT | GESTURE1_BIT | GESTURE2_BIT | GESTURE3_BIT | GESTURE4_BIT );
            state |= GESTURE1_BIT;
          }
        }
        else if( actions & ACTION_GESTURE2 ) {
          if( !( state & GESTURE2_BIT ) ) {
            state &= ~( GESTURE0_BIT | GESTURE1_BIT | GESTURE2_BIT | GESTURE3_BIT | GESTURE4_BIT );
            state |= GESTURE2_BIT;
          }
        }
        else if( actions & ACTION_GESTURE3 ) {
          if( !( state & GESTURE3_BIT ) ) {
            state &= ~( GESTURE0_BIT | GESTURE1_BIT | GESTURE2_BIT | GESTURE3_BIT | GESTURE4_BIT );
            state |= GESTURE3_BIT;
          }
        }
        else if( actions & ACTION_GESTURE4 ) {
          if( !( state & GESTURE4_BIT ) ) {
            state &= ~( GESTURE0_BIT | GESTURE1_BIT | GESTURE2_BIT | GESTURE3_BIT | GESTURE4_BIT );
            state |= GESTURE4_BIT;

            addEvent( EVENT_FLIP, 1.0f );
          }
        }
        else {
          state &= ~( GESTURE0_BIT | GESTURE1_BIT | GESTURE2_BIT | GESTURE3_BIT | GESTURE4_BIT );
        }
      }
    }

    /*
     * GRAB MOVEMENT
     */

    if( cargo != -1 ) {
      const Bot* cargoBot = static_cast<const Bot*>( cargoObj );

      if( cargoObj == null || cargoObj->cell == null || ( cargoObj->flags & BELOW_BIT ) ||
          ( state & SWIMMING_BIT ) || ( actions & ACTION_JUMP ) ||
          ( ( cargoBot->flags & BOT_BIT ) &&
            ( ( cargoBot->actions & ACTION_JUMP ) || ( cargoBot->cargo != -1 ) ) ) )
      {
        cargo = -1;
      }
      else {
        // keep constant length of xy projection of handle
        Vec3 handle = Vec3( -hvsc[0], hvsc[1], -hvsc[3] ) * grabHandle;
        // bottom of the object cannot be raised over the player AABB, neither can be lowered
        // under the player (in the latter case one can lift himself with the lower object)
        handle.z    = min( handle.z, dim.z - camZ );
        Vec3 string = p + Vec3( 0.0f, 0.0f, camZ ) + handle - cargoObj->p;

        if( string.sqL() > GRAB_HANDLE_TOL * grabHandle*grabHandle ) {
          cargo = -1;
        }
        else {
          Vec3 desiredMom    = string * GRAB_STRING_RATIO;
          Vec3 momDiff       = ( desiredMom - cargoObj->momentum ) * GRAB_MOM_RATIO;

          float momDiffSqL   = momDiff.sqL();
          momDiff.z          += Physics::G_ACCEL * Timer::TICK_TIME;
          if( momDiffSqL > GRAB_MOM_MAX_SQ ) {
            momDiff *= GRAB_MOM_MAX / Math::sqrt( momDiffSqL );
          }
          momDiff.z          -= Physics::G_ACCEL * Timer::TICK_TIME;

          flags              &= ~CLIMBER_BIT;

          cargoObj->momentum += momDiff;
          cargoObj->flags    &= ~DISABLED_BIT;
        }
      }
    }
  } // parent == -1

  /*
   * ACTIONS ON ITEMS
   */

  if( actions & ~oldActions & ACTION_USE ) {
    Object* obj = orbis.objects[instrument];

    if( obj != null ) {
      synapse.use( this, obj );
    }
  }
  else if( actions & ~oldActions & ACTION_INV_TAKE ) {
    Dynamic* item   = static_cast<Dynamic*>( orbis.objects[instrument] );
    Object*  source = orbis.objects[container];

    if( item != null && items.length() != clazz->nItems ) {
      hard_assert( source->items.contains( instrument ) );
      hard_assert( item != null && ( item->flags & DYNAMIC_BIT ) && ( item->flags & ITEM_BIT ) );

      item->parent = index;
      items.add( instrument );
      source->items.exclude( instrument );
    }
  }
  else if( actions & ~oldActions & ACTION_INV_GIVE ) {
    Dynamic* item   = static_cast<Dynamic*>( orbis.objects[instrument] );
    Object*  target = orbis.objects[container];

    if( item != null && target->items.length() != target->clazz->nItems ) {
      hard_assert( items.contains( instrument ) );
      hard_assert( item != null && ( item->flags & DYNAMIC_BIT ) && ( item->flags & ITEM_BIT ) );

      if( instrument == weapon ) {
        weapon = -1;
      }

      item->parent = container;
      target->items.add( instrument );
      items.exclude( instrument );
    }
  }
  else if( parent == -1 ) { // not applicable in vehicles
    if( actions & ~oldActions & ACTION_TAKE ) {
      Dynamic* item = static_cast<Dynamic*>( orbis.objects[instrument] );

      if( item != null && items.length() != clazz->nItems &&
          ( item->flags & ( ITEM_BIT | SOLID_BIT ) ) == ( ITEM_BIT | SOLID_BIT ) )
      {
        cargo = -1;

        item->flags &= ~( Object::TICK_CLEAR_MASK | Object::MOVE_CLEAR_MASK );
        item->lower = -1;
        item->velocity = Vec3::ZERO;
        item->momentum = Vec3::ZERO;

        item->parent = index;
        items.add( item->index );
        synapse.cut( item );
      }
    }
    else if( actions & ~oldActions & ACTION_ROTATE ) {
      if( cargoObj != null ) {
        int heading = cargoObj->flags & Object::HEADING_MASK;

        swap( cargoObj->dim.x, cargoObj->dim.y );

        if( collider.overlaps( cargoObj, cargoObj ) ) {
          swap( cargoObj->dim.x, cargoObj->dim.y );
        }
        else {
          cargoObj->flags &= ~Object::HEADING_MASK;
          cargoObj->flags |= ( heading + 1 ) % 4;
        }
      }
    }
    else if( actions & ~oldActions & ACTION_THROW ) {
      if( cargoObj != null && stamina >= clazz->staminaThrowDrain ) {
        hard_assert( cargoObj->flags & DYNAMIC_BIT );

        // { hsine, hcosine, vsine, vcosine, vsine * hsine, vsine * hcosine }
        float hvsc[6];

        Math::sincos( h, &hvsc[0], &hvsc[1] );
        Math::sincos( v, &hvsc[2], &hvsc[3] );

        Vec3 handle = Vec3( -hvsc[0], hvsc[1], -hvsc[3] );

        stamina -= clazz->staminaThrowDrain;
        cargoObj->momentum = handle * clazz->throwMomentum;

        cargo = -1;
      }
    }
    else if( actions & ~oldActions & ACTION_GRAB ) {
      if( instrument == -1 || weapon != -1 || ( state & ( CLIMBING_BIT | SWIMMING_BIT ) ) ) {
        cargo = -1;
      }
      else {
        Bot* dyn = static_cast<Bot*>( orbis.objects[instrument] );

        if( dyn != null && ( dyn->flags & DYNAMIC_BIT ) && dyn->mass <= clazz->grabMass &&
            ( !( dyn->flags & BOT_BIT ) || dyn->cargo == -1 ) )
        {
          float dimX = dim.x + dyn->dim.x;
          float dimY = dim.y + dyn->dim.y;
          float dist = Math::sqrt( dimX*dimX + dimY*dimY ) + GRAB_EPSILON;

          if( dist <= clazz->reachDist ) {
            cargo = instrument;
            grabHandle = dist;

            dyn->flags &= ~BELOW_BIT;
          }
        }
      }
    }
    else if( actions & ~oldActions & ( ACTION_INV_GRAB | ACTION_INV_DROP ) ) {
      Dynamic* item = static_cast<Dynamic*>( orbis.objects[instrument] );

      if( item != null && cargo == -1 ) {
        hard_assert( items.contains( instrument ) );
        hard_assert( ( item->flags & DYNAMIC_BIT ) && ( item->flags & ITEM_BIT ) );

        // { hsine, hcosine, vsine, vcosine, vsine * hsine, vsine * hcosine }
        float hvsc[6];

        Math::sincos( h, &hvsc[0], &hvsc[1] );
        Math::sincos( v, &hvsc[2], &hvsc[3] );

        float dimX = dim.x + item->dim.x;
        float dimY = dim.y + item->dim.y;
        float dist = Math::sqrt( dimX*dimX + dimY*dimY ) + GRAB_EPSILON;

        // keep constant length of xy projection of handle
        Vec3 handle = Vec3( -hvsc[0], hvsc[1], -hvsc[3] ) * dist;
        // bottom of the object cannot be raised over the player aabb
        handle.z    = clamp( handle.z, -dim.z - camZ, dim.z - camZ );
        item->p     = p + Vec3( 0.0f, 0.0f, camZ ) + handle;

        if( instrument == weapon ) {
          weapon = -1;
        }

        if( !collider.overlaps( item ) ) {
          item->parent = -1;
          synapse.put( item );
          items.exclude( instrument );

          if( ( actions & ~oldActions & ACTION_INV_GRAB ) &&
              !( state & ( CLIMBING_BIT | SWIMMING_BIT ) ) && weapon == -1 )
          {
            cargo      = instrument;
            grabHandle = dist;

            item->flags &= ~BELOW_BIT;
          }
        }
      }
    }
  }

  oldActions = actions;
  oldState   = state;
  instrument = -1;
  container  = -1;
}

inline Object* Bot::getTagged( const Vec3& at, int mask ) const
{
  const BotClass* clazz = static_cast<const BotClass*>( this->clazz );

  Point3 eye   = p + Vec3( 0.0f, 0.0f, camZ );
  Vec3   reach = at * clazz->reachDist;

  collider.mask = mask;
  collider.translate( eye, reach, this );
  collider.mask = SOLID_BIT;

  return collider.hit.obj;
}

Object* Bot::getTagged( int mask ) const
{
  // { hsine, hcosine, vsine, vcosine, vsine * hsine, vsine * hcosine }
  float hvsc[6];

  Math::sincos( h, &hvsc[0], &hvsc[1] );
  Math::sincos( v, &hvsc[2], &hvsc[3] );

  hvsc[4] = hvsc[2] * hvsc[0];
  hvsc[5] = hvsc[2] * hvsc[1];

  return getTagged( Vec3( -hvsc[4], hvsc[5], -hvsc[3] ), mask );
}

void Bot::heal()
{
  const BotClass* clazz = static_cast<const BotClass*>( this->clazz );

  life    = clazz->life;
  stamina = clazz->stamina;
}

void Bot::rearm()
{
  for( int i = 0; i < items.length(); ++i ) {
    if( items[i] != -1 ) {
      Weapon* weaponObj = static_cast<Weapon*>( orbis.objects[ items[i] ] );

      if( weaponObj != null && ( weaponObj->flags & Object::WEAPON_BIT ) ) {
        const WeaponClass* weaponClazz = static_cast<const WeaponClass*>( weaponObj->clazz );

        weaponObj->nRounds = weaponClazz->nRounds;
      }
    }
  }
}

void Bot::kill()
{
  if( !Math::isinf( life ) ) {
    const BotClass* clazz = static_cast<const BotClass*>( this->clazz );

    p.z        -= dim.z - clazz->corpseDim.z - EPSILON;
    dim.z      = clazz->corpseDim.z;
    flags      |= WIDE_CULL_BIT;
    flags      &= ~SOLID_BIT;
    life       = clazz->life / 2.0f - EPSILON;
    resistance = Math::INF;

    actions    = 0;
    instrument = -1;
    container  = -1;

    state      |= DEAD_BIT;
    cargo      = -1;

    if( clazz->nItems != 0 ) {
      flags |= BROWSABLE_BIT;
    }

    addEvent( EVENT_DEATH, 1.0f );
  }
}

void Bot::enter( int vehicle_ )
{
  hard_assert( cell != null && vehicle_ != -1 );

  const BotClass* clazz = static_cast<const BotClass*>( this->clazz );

  flags      &= ~( Object::TICK_CLEAR_MASK | Object::MOVE_CLEAR_MASK );
  lower      = -1;
  parent     = vehicle_;

  dim        = clazz->dim;

  actions    = 0;
  instrument = -1;
  container  = vehicle_;

  camZ       = clazz->camZ;
  state      &= ~CROUCHING_BIT;
  cargo      = -1;

  synapse.cut( this );
}

void Bot::exit()
{
  hard_assert( cell == null && parent != -1 );

  parent     = -1;
  actions    = 0;
  instrument = -1;
  container  = -1;
  cargo      = -1;

  synapse.put( this );
}

Bot::Bot(  const BotClass* clazz_, int index, const Point3& p_, Heading heading ) :
    Dynamic( clazz_, index, p_, heading )
{
  h          = float( heading ) * Math::TAU / 4.0f;
  v          = Math::TAU / 4.0f;
  actions    = 0;
  oldActions = 0;
  instrument = -1;
  container  = -1;

  state      = clazz_->state;
  oldState   = clazz_->state;
  stamina    = clazz_->stamina;
  stepRate   = 0.0f;
  cargo      = -1;
  weapon     = -1;
  grabHandle = 0.0f;
  camZ       = clazz_->camZ;

  name       = namePool.genName( clazz_->nameList );
  mindFunc   = clazz_->mindFunc;
}

Bot::Bot( const BotClass* clazz_, InputStream* istream ) :
    Dynamic( clazz_, istream )
{
  dim        = istream->readVec3();

  h          = istream->readFloat();
  v          = istream->readFloat();
  actions    = istream->readInt();
  oldActions = istream->readInt();
  instrument = istream->readInt();
  container  = istream->readInt();

  state      = istream->readInt();
  oldState   = istream->readInt();
  stamina    = istream->readFloat();
  stepRate   = istream->readFloat();
  cargo      = istream->readInt();
  weapon     = istream->readInt();
  grabHandle = istream->readFloat();
  camZ       = state & Bot::CROUCHING_BIT ? clazz_->crouchCamZ : clazz_->camZ;

  name       = istream->readString();
  mindFunc   = istream->readString();

  if( state & DEAD_BIT ) {
    resistance = Math::INF;
  }
}

void Bot::write( BufferStream* ostream ) const
{
  Dynamic::write( ostream );

  ostream->writeVec3( dim );

  ostream->writeFloat( h );
  ostream->writeFloat( v );
  ostream->writeInt( actions );
  ostream->writeInt( oldActions );
  ostream->writeInt( instrument );
  ostream->writeInt( container );

  ostream->writeInt( state );
  ostream->writeInt( oldState );
  ostream->writeFloat( stamina );
  ostream->writeFloat( stepRate );
  ostream->writeInt( cargo );
  ostream->writeInt( weapon );
  ostream->writeFloat( grabHandle );

  ostream->writeString( name );
  ostream->writeString( mindFunc );
}

void Bot::readUpdate( InputStream* )
{}

void Bot::writeUpdate( BufferStream* ) const
{}

}
}
