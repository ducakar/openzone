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

#include "matrix/Library.hh"
#include "matrix/NamePool.hh"
#include "matrix/Physics.hh"
#include "matrix/Synapse.hh"

namespace oz
{
namespace matrix
{

const float Bot::AIR_FRICTION         =  0.01f;
const float Bot::LADDER_SLIP_MOMENTUM =  16.0f;

const float Bot::WOUNDED_THRESHOLD    =  0.70f;
const float Bot::CORPSE_FADE_FACTOR   =  0.50f / 100.0f;

const float Bot::INSTRUMENT_DIST      =  2.00f;
const float Bot::INSTRUMENT_DOT_MIN   =  0.80f;

const float Bot::GRAB_EPSILON         =  0.20f;
const float Bot::GRAB_STRING_RATIO    =  10.0f;
const float Bot::GRAB_HANDLE_TOL      =  1.60f;
const float Bot::GRAB_MOM_RATIO       =  0.30f;
const float Bot::GRAB_MOM_MAX         =  1.00f; // must be < abs( Physics::HIT_THRESHOLD )
const float Bot::GRAB_MOM_MAX_SQ      =  1.00f;

const float Bot::STEP_MOVE_AHEAD      =  0.20f;
const float Bot::CLIMB_MOVE_AHEAD     =  0.40f;

Pool<Bot, 1024> Bot::pool;

bool Bot::hasAttribute( int attribute ) const
{
  if( clazz->attributes & attribute ) {
    return true;
  }

  if( parent >= 0 ) {
    const Object* vehicle = orbis.objects[parent];

    if( vehicle != null && ( vehicle->clazz->attributes & attribute ) ) {
      return true;
    }
  }

  for( int i = 0; i < items.length(); ++i ) {
    const Object* item = orbis.objects[ items[i] ];

    if( item != null && ( item->clazz->attributes & attribute ) ) {
      return true;
    }
  }

  return false;
}

bool Bot::canReach( const Entity* ent ) const
{
  const BotClass* clazz = static_cast<const BotClass*>( this->clazz );

  Point eye   = Point( p.x, p.y, p.z + camZ );
  Vec3  reach = Vec3( clazz->reachDist, clazz->reachDist, clazz->reachDist );

  return collider.overlapsEntity( AABB( eye, reach ), ent );
}

bool Bot::canReach( const Object* obj ) const
{
  const BotClass* clazz = static_cast<const BotClass*>( this->clazz );

  Point eye   = Point( p.x, p.y, p.z + camZ );
  Vec3  reach = Vec3( clazz->reachDist, clazz->reachDist, clazz->reachDist );

  return AABB( eye, reach ).overlaps( *obj );
}

bool Bot::trigger( const Entity* entity )
{
  hard_assert( entity != null );

  if( entity->key >= 0 && entity->model->target >= 0 && canReach( entity ) ) {
    actions   &= ~INSTRUMENT_ACTIONS;
    actions   |= ACTION_TRIGGER;
    instrument = entity->str->index * Struct::MAX_ENTITIES + int( entity - entity->str->entities );
    container  = -1;

    return true;
  }
  return false;
}

bool Bot::lock( const Entity* entity )
{
  hard_assert( entity != null );

  if( entity->key != 0 && canReach( entity ) ) {
    actions   &= ~INSTRUMENT_ACTIONS;
    actions   |= ACTION_LOCK;
    instrument = entity->str->index * Struct::MAX_ENTITIES + int( entity - entity->str->entities );
    container  = -1;

    return true;
  }
  return false;
}

bool Bot::use( const Object* object )
{
  hard_assert( object != null );

  if( ( object->flags & USE_FUNC_BIT ) && canReach( object ) ) {
    actions   &= ~INSTRUMENT_ACTIONS;
    actions   |= ACTION_USE;
    instrument = object->index;
    container  = -1;

    return true;
  }
  return false;
}

bool Bot::take( const Dynamic* item )
{
  hard_assert( item != null && ( item->flags & DYNAMIC_BIT ) );

  if( ( item->flags & ITEM_BIT ) && canReach( item ) ) {
    actions   &= ~INSTRUMENT_ACTIONS;
    actions   |= ACTION_TAKE;
    instrument = item->index;
    container  = -1;

    return true;
  }
  return false;
}

bool Bot::grab( const Dynamic* dynamic )
{
  hard_assert( dynamic == null || ( dynamic->flags & DYNAMIC_BIT ) );

  if( dynamic == null || canReach( dynamic ) ) {
    actions   &= ~INSTRUMENT_ACTIONS;
    actions   |= ACTION_GRAB;
    instrument = dynamic == null ? -1 : dynamic->index;
    container  = -1;
    cargo      = -1;

    return true;
  }
  return false;
}

bool Bot::rotateCargo()
{
  if( cargo >= 0 ) {
    actions   &= ~INSTRUMENT_ACTIONS;
    actions   |= ACTION_ROTATE;
    instrument = -1;
    container  = -1;

    return true;
  }
  return false;
}

bool Bot::throwCargo()
{
  if( cargo >= 0 ) {
    actions   &= ~INSTRUMENT_ACTIONS;
    actions   |= ACTION_THROW;
    instrument = -1;
    container  = -1;

    return true;
  }
  return false;
}

bool Bot::invUse( const Dynamic* item, const Object* source )
{
  hard_assert( item != null && source != null );

  if( ( item->flags & USE_FUNC_BIT ) && source->items.contains( item->index ) &&
    canReach( source ) )
  {
    actions   &= ~INSTRUMENT_ACTIONS;
    actions   |= ACTION_INV_USE;
    instrument = item->index;
    container  = source->index;

    return true;
  }
  return false;
}

bool Bot::invTake( const Dynamic* item, const Object* source )
{
  hard_assert( item != null && source != null );

  if( source->items.contains( item->index ) && canReach( source ) ) {
    actions   &= ~INSTRUMENT_ACTIONS;
    actions   |= ACTION_INV_TAKE;
    instrument = item->index;
    container  = source->index;

    return true;
  }
  return false;
}

bool Bot::invGive( const Dynamic* item, const Object* target )
{
  hard_assert( item != null && target != null );

  if( items.contains( item->index ) && canReach( target ) ) {
    actions   &= ~INSTRUMENT_ACTIONS;
    actions   |= ACTION_INV_GIVE;
    instrument = item->index;
    container  = target->index;

    return true;
  }
  return false;
}

bool Bot::invDrop( const Dynamic* item )
{
  hard_assert( item != null );

  if( items.contains( item->index ) ) {
    actions   &= ~INSTRUMENT_ACTIONS;
    actions   |= ACTION_INV_DROP;
    instrument = item->index;
    container  = -1;

    return true;
  }
  return false;
}

bool Bot::invGrab( const Dynamic* item )
{
  hard_assert( item != null );

  if( items.contains( item->index ) ) {
    actions   &= ~INSTRUMENT_ACTIONS;
    actions   |= ACTION_INV_GRAB;
    instrument = item->index;
    container  = -1;
    cargo      = -1;

    return true;
  }
  return false;
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
    if( items[i] >= 0 ) {
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
  const BotClass* clazz = static_cast<const BotClass*>( this->clazz );

  p.z       -= dim.z - clazz->corpseDim.z - EPSILON;
  dim.z      = clazz->corpseDim.z;
  flags     |= WIDE_CULL_BIT;
  flags     &= ~SOLID_BIT;
  life       = clazz->life / 2.0f - EPSILON;
  resistance = Math::INF;

  actions    = 0;
  instrument = -1;
  container  = -1;

  state     |= DEAD_BIT;
  cargo      = -1;

  if( clazz->nItems != 0 ) {
    flags |= BROWSABLE_BIT;
  }

  addEvent( EVENT_DEATH, 1.0f );
}

void Bot::enter( int vehicle_ )
{
  hard_assert( cell != null && vehicle_ >= 0 );

  const BotClass* clazz = static_cast<const BotClass*>( this->clazz );

  parent     = vehicle_;

  dim        = clazz->dim;

  actions    = 0;
  instrument = -1;
  container  = vehicle_;

  camZ       = clazz->camZ;
  state     &= ~CROUCHING_BIT;
  step       = 0.0f;
  cargo      = -1;

  synapse.cut( this );
}

void Bot::exit()
{
  hard_assert( cell == null && parent >= 0 );
  hard_assert( cargo == -1 );

  parent     = -1;
  actions    = 0;
  instrument = -1;
  container  = -1;

  synapse.put( this );
}

void Bot::onDestroy()
{
  // only play death sound when an alive bot is destroyed but not when a body is destroyed
  if( !( state & DEAD_BIT ) ) {
    addEvent( EVENT_DEATH, 1.0f );
  }

  Dynamic::onDestroy();
}

void Bot::onUpdate()
{
  const BotClass* clazz = static_cast<const BotClass*>( this->clazz );

  Dynamic* cargoObj  = cargo  < 0 ? null : static_cast<Dynamic*>( orbis.objects[cargo] );
  Weapon*  weaponObj = weapon < 0 ? null : static_cast<Weapon*>( orbis.objects[weapon] );

  if( weaponObj == null ) {
    weapon = -1;
  }
  if( cargoObj == null ) {
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

        life -= clazz->life * CORPSE_FADE_FACTOR * Timer::TICK_TIME;
        // we don't want Object::destroy() to be called when body dissolves (destroy() causes
        // sounds and frags to fly around), that's why we just remove the object
        if( life <= 0.0f ) {
          flags |= DESTROYED_BIT;
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

  if( parent >= 0 ) {
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
                ATTACKING_BIT );

    if( cargo >= 0 || velocity.sqN() > LADDER_SLIP_MOMENTUM ) {
      flags &= ~ON_LADDER_BIT;
    }

    state |= lower >= 0 || ( flags & ON_FLOOR_BIT ) ? GROUNDED_BIT  : 0;
    state |= ( flags & ON_LADDER_BIT )              ? CLIMBING_BIT  : 0;
    state |= depth > dim.z                          ? SWIMMING_BIT  : 0;
    state |= depth > dim.z + camZ                   ? SUBMERGED_BIT : 0;

    if( state & SUBMERGED_BIT ) {
      stamina -= clazz->staminaWaterDrain;

      if( stamina < 0.0f ) {
        float oldLife = life;

        life += stamina;
        stamina = 0.0f;

        if( int( life ) / 8 != int( oldLife ) / 8 ) {
          addEvent( EVENT_DAMAGE, 1.0f );
        }
      }
    }

    stairRate *= clazz->stairRateSupp;

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
          cargo < 0 && stamina >= clazz->staminaJumpDrain )
      {
        flags     &= ~( DISABLED_BIT | ON_FLOOR_BIT );
        lower      = -1;
        state     &= ~( JUMP_SCHED_BIT | GROUNDED_BIT );
        momentum.z = clazz->jumpMomentum;
        stamina   -= clazz->staminaJumpDrain;
        addEvent( EVENT_JUMP, 1.0f );
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
        flags |= ENABLE_BIT;

        p.z   += dim.z - clazz->crouchDim.z;
        dim.z  = clazz->crouchDim.z;
        camZ   = clazz->crouchCamZ;
        state |= CROUCHING_BIT;
      }
    }
    if( actions & ~oldActions & ACTION_WALK ) {
      state ^= WALKING_BIT;
    }

    if( stamina < clazz->staminaRunDrain || life < WOUNDED_THRESHOLD * clazz->life ) {
      state |= WALKING_BIT;
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

    if( state & CLIMBING_BIT ) {
      move.z *= 2.0f;
    }

    if( move == Vec3::ZERO ) {
      step = 0.0f;
    }
    else {
      flags &= ~DISABLED_BIT;
      state |= MOVING_BIT;
      move   = ~move;

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
          for( float raise = clazz->stairMax; raise <= maxRaise; raise += clazz->climbInc ) {
            p.z += clazz->climbInc;

            collider.translate( this, desiredMove );

            Vec3 move = desiredMove * collider.hit.ratio;
            float endDist = startDist + Vec3( move.x, move.y, move.z + raise ) * normal;

            if( endDist < 0.0f ) {
              // check if ledge has a normal.z >= FLOOR_NORMAL_Z
              Point raisedPos = p;
              p += move;

              collider.translate( this, Vec3( 0.0f, 0.0f, -raise ) );

              p = raisedPos;

              if( collider.hit.ratio != 1.0f && collider.hit.normal.z >= Physics::FLOOR_NORMAL_Z ) {
                momentum.x *= 1.0f - Physics::LADDER_FRICTION;
                momentum.y *= 1.0f - Physics::LADDER_FRICTION;
                momentum.z  = max( momentum.z, clazz->climbMomentum );

                cargo       = -1;
                state      |= CLIMBING_BIT;
                state      &= ~JUMP_SCHED_BIT;
                stamina    -= clazz->staminaClimbDrain;

                break;
              }
            }
          }

          p.z = originalZ;
        }
      }

      /*
       * STEPPING OVER OBSTACLES (STAIRS)
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
      if( !( state & CLIMBING_BIT ) && stairRate <= clazz->stairRateLimit ) {
        // check if bot's gonna hit a stair in the next frame
        Vec3 desiredMove = STEP_MOVE_AHEAD * move;

        collider.translate( this, desiredMove );

        if( collider.hit.ratio != 1.0f && collider.hit.normal.z < Physics::FLOOR_NORMAL_Z ) {
          Vec3  normal    = collider.hit.normal;
          float startDist = 2.0f * EPSILON - ( desiredMove * collider.hit.ratio ) * normal;
          float originalZ = p.z;

          collider.translate( this, Vec3( 0.0f, 0.0f, clazz->stairMax + 2.0f * EPSILON ) );

          float maxRaise = collider.hit.ratio * clazz->stairMax;

          for( float raise = clazz->stairInc; raise <= maxRaise; raise += clazz->stairInc ) {
            p.z += clazz->stairInc;
            collider.translate( this, desiredMove );

            Vec3 move = desiredMove * collider.hit.ratio;
            move.z += raise;
            float endDist = startDist + move * normal;

            if( endDist < 0.0f ) {
              momentum.z = max( momentum.z, 0.0f );
              stairRate += raise*raise;
              goto stepSucceeded;
            }
          }
          p.z = originalZ;
        }
      }

    stepSucceeded:

      Vec3 desiredMomentum = move;

      if( state & ( CROUCHING_BIT | WALKING_BIT ) || cargo >= 0 ) {
        step            += clazz->stepWalkInc;
        desiredMomentum *= clazz->walkMomentum;
      }
      else {
        stamina         -= clazz->staminaRunDrain;
        step            += clazz->stepRunInc;
        desiredMomentum *= clazz->runMomentum;
      }

      if( flags & ON_SLICK_BIT ) {
        desiredMomentum *= clazz->slickControl;
      }
      else if( state & CLIMBING_BIT ) {
        desiredMomentum *= clazz->climbControl;
      }
      else if( state & SWIMMING_BIT ) {
        // not on static ground
        if( !( flags & ON_FLOOR_BIT ) &&
            !( lower >= 0 && ( orbis.objects[lower]->flags & Object::DISABLED_BIT ) ) )
        {
          desiredMomentum *= clazz->waterControl;
        }
      }
      else if( !( state & GROUNDED_BIT ) ) {
        desiredMomentum *= clazz->airControl;
      }

      if( ( flags & ( ON_FLOOR_BIT | IN_LIQUID_BIT ) ) == ON_FLOOR_BIT && floor.z != 1.0f ) {
        float dot = desiredMomentum * floor;

        if( dot > 0.0f ) {
          desiredMomentum -= dot * floor;
        }
      }

      momentum += desiredMomentum;
      step      = Math::fmod( step, 1.0f );
    }

    /*
     * ATTACK & GESTURES
     */

    if( !( state & MOVING_BIT ) && cargo < 0 &&
        ( !( actions & ACTION_JUMP ) || ( state & ( Bot::GROUNDED_BIT | Bot::CLIMBING_BIT ) ) ) )
    {
      if( actions & ACTION_ATTACK ) {
        if( weaponObj != null ) {
          state |= ATTACKING_BIT;
          weaponObj->trigger( this );
        }
      }
      else if( !( state & CROUCHING_BIT ) ) {
        if( actions & ACTION_GESTURE_MASK ) {
          if( actions & ACTION_POINT ) {
            if( !( state & GESTURE_POINT_BIT ) ) {
              state &= ~GESTURE_MASK;
              state |= GESTURE_POINT_BIT;
            }
          }
          else if( actions & ACTION_BACK ) {
            if( !( state & GESTURE_BACK_BIT ) ) {
              state &= ~GESTURE_MASK;
              state |= GESTURE_BACK_BIT;
            }
          }
          else if( actions & ACTION_SALUTE ) {
            if( !( state & GESTURE_SALUTE_BIT ) ) {
              state &= ~GESTURE_MASK;
              state |= GESTURE_SALUTE_BIT;
            }
          }
          else if( actions & ACTION_WAVE ) {
            if( !( state & GESTURE_WAVE_BIT ) ) {
              state &= ~GESTURE_MASK;
              state |= GESTURE_WAVE_BIT;
            }
          }
          else {
            if( !( state & GESTURE_FLIP_BIT ) ) {
              state &= ~GESTURE_MASK;
              state |= GESTURE_FLIP_BIT;

              addEvent( EVENT_FLIP, 1.0f );
            }
          }
        }
        else {
          state &= ~GESTURE_MASK;
        }
      }
    }

    /*
     * GRAB MOVEMENT
     */

    if( cargo >= 0 ) {
      const Bot* cargoBot = static_cast<const Bot*>( cargoObj );

      if( cargoObj == null || cargoObj->cell == null || ( cargoObj->flags & BELOW_BIT ) ||
          ( state & SWIMMING_BIT ) || ( actions & ACTION_JUMP ) ||
          ( ( cargoBot->flags & BOT_BIT ) &&
            ( ( cargoBot->actions & ACTION_JUMP ) || ( cargoBot->cargo >= 0 ) ) ) )
      {
        cargo    = -1;
        cargoObj = null;
      }
      else {
        // keep constant length of xy projection of handle
        Vec3 handle = Vec3( -hvsc[0], hvsc[1], -hvsc[3] ) * grabHandle;
        // bottom of the object cannot be raised over the player AABB, neither can be lowered
        // under the player (in the latter case one can lift himself with the lower object)
        handle.z    = min( handle.z, dim.z - camZ );
        Vec3 string = p + Vec3( 0.0f, 0.0f, camZ ) + handle - cargoObj->p;

        if( string.sqN() > GRAB_HANDLE_TOL * grabHandle*grabHandle ) {
          cargo    = -1;
          cargoObj = null;
        }
        else {
          Vec3 desiredMom     = string * GRAB_STRING_RATIO;
          Vec3 momDiff        = ( desiredMom - cargoObj->momentum ) * GRAB_MOM_RATIO;

          float momDiffSqN    = momDiff.sqN();
          momDiff.z          += physics.gravity * Timer::TICK_TIME;

          if( momDiffSqN > GRAB_MOM_MAX_SQ ) {
            momDiff *= GRAB_MOM_MAX / Math::sqrt( momDiffSqN );
          }

          momDiff.z          -= physics.gravity * Timer::TICK_TIME;

          cargoObj->flags    &= ~DISABLED_BIT;
          cargoObj->momentum += momDiff;
        }
      }
    }
  } // parent == -1

  /*
   * ACTIONS ON INSTRUMENT
   */

  if( actions & ~oldActions & INSTRUMENT_ACTIONS ) {
    if( actions & ~oldActions & ACTION_INV_USE ) {
      Dynamic* item   = static_cast<Dynamic*>( orbis.objects[instrument] );
      Object*  source = orbis.objects[container];

      if( item != null && source != null &&
          source->items.contains( instrument ) && canReach( source ) )
      {
        hard_assert( ( item->flags & DYNAMIC_BIT ) && ( item->flags & ITEM_BIT ) );

        synapse.use( this, item );
      }
    }
    else if( actions & ~oldActions & ACTION_INV_TAKE ) {
      Dynamic* item   = static_cast<Dynamic*>( orbis.objects[instrument] );
      Object*  source = orbis.objects[container];

      if( item != null && source != null && items.length() != clazz->nItems &&
          source->items.contains( instrument ) && canReach( source ) )
      {
        hard_assert( ( item->flags & DYNAMIC_BIT ) && ( item->flags & ITEM_BIT ) );

        item->parent = index;
        items.add( instrument );
        source->items.exclude( instrument );

        if( source->flags & BOT_BIT ) {
          Bot* bot = static_cast<Bot*>( source );

          if( bot->weapon == item->index ) {
            bot->weapon = -1;
          }
        }
      }
    }
    else if( actions & ~oldActions & ACTION_INV_GIVE ) {
      Dynamic* item   = static_cast<Dynamic*>( orbis.objects[instrument] );
      Object*  target = orbis.objects[container];

      if( item != null && target != null && target->items.length() != target->clazz->nItems &&
          items.contains( instrument ) && canReach( target ) )
      {
        hard_assert( ( item->flags & DYNAMIC_BIT ) && ( item->flags & ITEM_BIT ) );

        item->parent = container;
        target->items.add( instrument );
        items.exclude( instrument );

        if( instrument == weapon ) {
          weapon = -1;
        }
      }
    }
    else if( parent < 0 ) { // not applicable in vehicles
      if( actions & ~oldActions & ( ACTION_TRIGGER | ACTION_LOCK ) ) {
        int strIndex = instrument / Struct::MAX_ENTITIES;
        int entIndex = instrument % Struct::MAX_ENTITIES;

        Struct* str = orbis.structs[strIndex];

        if( str != null ) {
          Entity* ent = &str->entities[entIndex];

          if( canReach( ent ) ) {
            if( actions & ~oldActions & ACTION_TRIGGER ) {
              synapse.trigger( ent );
            }
            else {
              synapse.lock( this, ent );
            }
          }
        }
      }
      else if( actions & ~oldActions & ACTION_USE ) {
        Dynamic* obj = static_cast<Dynamic*>( orbis.objects[instrument] );

        if( obj != null && canReach( obj ) ) {
          synapse.use( this, obj );
        }
      }
      else if( actions & ~oldActions & ACTION_TAKE ) {
        Dynamic* item = static_cast<Dynamic*>( orbis.objects[instrument] );

        if( item != null && items.length() != clazz->nItems && canReach( item ) ) {
          hard_assert( ( item->flags & DYNAMIC_BIT ) && ( item->flags & ITEM_BIT ) );

          cargo = -1;

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
        if( instrument < 0 || weapon >= 0 || ( state & ( CLIMBING_BIT | SWIMMING_BIT ) ) ) {
          cargo = -1;
        }
        else {
          Bot* dyn = static_cast<Bot*>( orbis.objects[instrument] );

          if( dyn != null && dyn->mass <= clazz->grabMass &&
              !( ( dyn->flags & BOT_BIT ) && dyn->cargo >= 0 ) && canReach( dyn ) )
          {
            hard_assert( dyn->flags & DYNAMIC_BIT );

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

        if( item != null && cargo < 0 && items.contains( instrument ) ) {
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

            item->velocity = velocity;
            item->momentum = velocity;

            if( ( actions & ~oldActions & ACTION_INV_GRAB ) &&
                !( state & ( CLIMBING_BIT | SWIMMING_BIT ) ) && weapon < 0 )
            {
              cargo      = instrument;
              grabHandle = dist;

              item->flags &= ~BELOW_BIT;
            }
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

Bot::Bot( const BotClass* clazz_, int index, const Point& p_, Heading heading ) :
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
  step       = 0.0f;
  stairRate  = 0.0f;
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
  step       = istream->readFloat();
  stairRate  = istream->readFloat();
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
  ostream->writeFloat( step );
  ostream->writeFloat( stairRate );
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
