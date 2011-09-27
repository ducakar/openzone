/*
 *  Bot.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Bot.hpp"

#include "matrix/Synapse.hpp"
#include "matrix/Collider.hpp"
#include "matrix/Physics.hpp"
#include "matrix/Weapon.hpp"
#include "matrix/BotClass.hpp"
#include "matrix/Vehicle.hpp"

namespace oz
{

  const float Bot::HIT_HARD_THRESHOLD  = -8.00f;
  const float Bot::WOUNDED_THRESHOLD   = 0.70f;
  const float Bot::INSTRUMENT_DIST_MAX = 2.00f;
  const float Bot::INSTRUMENT_DOT_MIN  = 0.80f;
  const float Bot::GRAB_EPSILON        = 0.20f;
  const float Bot::GRAB_STRING_RATIO   = 10.0f;
  const float Bot::GRAB_HANDLE_TOL     = 1.60f;
  const float Bot::GRAB_MOM_RATIO      = 0.3f;
  // should be smaller than abs( Physics::HIT_THRESHOLD )
  const float Bot::GRAB_MOM_MAX        = 1.0f;
  const float Bot::GRAB_MOM_MAX_SQ     = 1.0f;
  const float Bot::DEAD_BODY_LIFT      = 100.0f;
  // 100 s
  const float Bot::BODY_FADE_FACTOR    = 0.5f / 100.0f * Timer::TICK_TIME;

  Pool<Bot, 1024> Bot::pool;

  const Object* Bot::getTagged( float* hvsc, int mask ) const
  {
    const BotClass* clazz = static_cast<const BotClass*>( this->clazz );

    Point3 eye  = p + Vec3( 0.0f, 0.0f, camZ );
    Vec3   look = Vec3( -hvsc[4], hvsc[5], -hvsc[3] ) * clazz->grabDistance;

    collider.mask = mask;
    collider.translate( eye, look, this );
    collider.mask = SOLID_BIT;

    return collider.hit.obj;
  }

  void Bot::onDestroy()
  {
    // only play death sound when an alive bot is destroyed but not when a body is destroyed
    if( !( state & DEATH_BIT ) ) {
      state |= DEATH_BIT;
      addEvent( EVENT_DEATH, 1.0f );
    }
    Object::onDestroy();
  }

  void Bot::onHit( const Hit* hit, float hitMomentum )
  {
    if( hit->normal.z >= Physics::FLOOR_NORMAL_Z ) {
      hard_assert( hitMomentum <= 0.0f );

      addEvent( EVENT_LAND, hitMomentum * MOMENTUM_INTENSITY_COEF );
    }
    else if( hitMomentum < HIT_HARD_THRESHOLD ) {
      addEvent( EVENT_HIT_HARD, hitMomentum * MOMENTUM_INTENSITY_COEF );
    }
  }

  void Bot::onUpdate()
  {
    const BotClass* clazz = static_cast<const BotClass*>( this->clazz );

    Object*  instrumentObj = null;
    Weapon*  weaponObj     = null;

    hard_assert( instrument != -1 || !( state & GRAB_BIT ) );

    if( instrument != -1 ) {
      instrumentObj = orbis.objects[instrument];

      if( instrumentObj == null ) {
        instrument = -1;
      }
    }

    if( weapon != -1 ) {
      state &= ~GRAB_BIT;

      weaponObj = static_cast<Weapon*>( orbis.objects[weapon] );

      if( weaponObj == null || weaponObj->parent != index ) {
        weapon = -1;
      }
    }

    if( life <= clazz->life / 2.0f ) {
      if( life > 0.0f ) {
        if( !( state & DEATH_BIT ) ) {
          flags |= WIDE_CULL_BIT;
          flags &= ~SOLID_BIT;
          life  = clazz->life / 2.0f - EPSILON;
          state |= DEATH_BIT;
          state &= ~GRAB_BIT;
          anim  = Anim::Type( Anim::DEATH_FALLBACK + Math::rand( 3 ) );

          instrument = -1;
          taggedItem = -1;

          if( clazz->nItems != 0 ) {
            flags |= BROWSABLE_BIT;
          }

          addEvent( EVENT_DEATH, 1.0f );
        }
        else {
          life -= clazz->life * BODY_FADE_FACTOR;
          // we don't want Object::destroy() to be called when body dissolves (destroy() causes
          // sounds and particles to fly around), that's why we just remove the object
          if( life <= 0.0f ) {
            parent != -1 ? synapse.removeCut( this ) : synapse.remove( this );
          }
        }
      }
      return;
    }

    h = Math::mod( h + Math::TAU, Math::TAU );
    v = clamp( v, 0.0f, Math::TAU / 2.0f );

    stamina = min( stamina + clazz->staminaGain, clazz->stamina );

    if( actions & ~oldActions & ACTION_SUICIDE ) {
      life = clazz->life / 2.0f - EPSILON;
    }
    // we implement inventory actions here so they can be used inside vehicles
    else if( actions & ~oldActions & ACTION_INV_USE ) {
      if( taggedItem != -1 && taggedItem < items.length() ) {
        Dynamic* item = static_cast<Dynamic*>( orbis.objects[ items[taggedItem] ] );

        hard_assert( item != null && ( item->flags & DYNAMIC_BIT ) && ( item->flags & ITEM_BIT ) );

        synapse.use( this, item );
        // the object may have removed itself after use
        if( item->index == -1 ) {
          items.remove( taggedItem );
        }
      }
    }
    else if( actions & ~oldActions & ACTION_INV_TAKE ) {
      if( instrumentObj != null && taggedItem != -1 && taggedItem < instrumentObj->items.length() &&
          items.length() < clazz->nItems )
      {
        Dynamic* item = static_cast<Dynamic*>( orbis.objects[ instrumentObj->items[taggedItem] ] );

        hard_assert( item != null && ( item->flags & DYNAMIC_BIT ) && ( item->flags & ITEM_BIT ) );

        items.add( item->index );
        instrumentObj->items.remove( taggedItem );
        item->parent = index;
      }
    }
    else if( actions & ~oldActions & ACTION_INV_GIVE ) {
      if( instrumentObj != null && taggedItem != -1 && taggedItem < items.length() &&
          instrumentObj->items.length() < instrumentObj->clazz->nItems )
      {
        Dynamic* item = static_cast<Dynamic*>( orbis.objects[ items[taggedItem] ] );

        hard_assert( item != null && ( item->flags & DYNAMIC_BIT ) && ( item->flags & ITEM_BIT ) );

        instrumentObj->items.add( item->index );
        items.remove( taggedItem );
        item->parent = instrument;
      }
    }

    if( parent != -1 ) {
      hard_assert( cell == null && instrument == parent );

      taggedItem = -1;
      return;
    }

    /*
     * STATE
     */

    // { hsine, hcosine, vsine, vcosine, vsine * hsine, vsine * hcosine }
    float hvsc[6];

    Math::sincos( h, &hvsc[0], &hvsc[1] );
    Math::sincos( v, &hvsc[2], &hvsc[3] );

    hvsc[4] = hvsc[2] * hvsc[0];
    hvsc[5] = hvsc[2] * hvsc[1];

    state &= ~( GROUNDED_BIT | CLIMBING_BIT | SWIMMING_BIT | SUBMERGED_BIT );

    state |= lower != -1 || ( flags & ON_FLOOR_BIT ) ? GROUNDED_BIT  : 0;
    state |= ( flags & ON_LADDER_BIT )               ? CLIMBING_BIT  : 0;
    state |= depth > dim.z                           ? SWIMMING_BIT  : 0;
    state |= depth > dim.z + camZ                    ? SUBMERGED_BIT : 0;

    flags |= CLIMBER_BIT;

    if( state & SUBMERGED_BIT ) {
      stamina -= clazz->staminaWaterDrain;

      if( stamina < 0.0f ) {
        life += stamina;
        stamina = 0.0f;
      }
    }

    stepRate -= ( velocity.x*velocity.x + velocity.y*velocity.y );
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
          !( state & GRAB_BIT ) && stamina >= clazz->staminaJumpDrain )
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

        p.z = oldZ + clazz->dim.z - clazz->dimCrouch.z;
        dim = clazz->dim;

        if( !collider.overlaps( this, this ) ) {
          camZ  = clazz->camZ;
          state &= ~CROUCHING_BIT;
        }
        else {
          p.z = oldZ - clazz->dim.z + clazz->dimCrouch.z;

          if( !collider.overlaps( this, this ) ) {
            camZ  = clazz->camZ;
            state &= ~CROUCHING_BIT;
          }
          else {
            dim = clazz->dimCrouch;
            p.z = oldZ;
          }
        }
      }
      else {
        flags &= ~DISABLED_BIT;
        flags &= ~ON_FLOOR_BIT;
        lower =  -1;

        p.z    += dim.z - clazz->dimCrouch.z;
        dim.z  = clazz->dimCrouch.z;
        camZ   = clazz->crouchCamZ;
        state  |= CROUCHING_BIT;
      }
    }
    if( stamina < clazz->staminaRunDrain || life < WOUNDED_THRESHOLD * clazz->life ) {
      state &= ~RUNNING_BIT;
    }

    /*
     * ANIMATION
     */

    if( ( actions & ACTION_JUMP ) && !( state & GROUNDED_BIT ) ) {
      anim = Anim::JUMP;
    }
    else if( actions & ( ACTION_FORWARD | ACTION_BACKWARD | ACTION_LEFT | ACTION_RIGHT ) ) {
      anim = ( state & CROUCHING_BIT ) ? Anim::CROUCH_WALK : Anim::RUN;
    }
    else if( actions & ACTION_ATTACK ) {
      if( weaponObj != null && weaponObj->shotTime == 0.0f ) {
        anim = ( state & CROUCHING_BIT ) ? Anim::CROUCH_ATTACK : Anim::ATTACK;
        weaponObj->trigger( this );
      }
    }
    else if( state & CROUCHING_BIT ) {
      anim = Anim::CROUCH_STAND;
    }
    else if( state & GESTURE0_BIT ) {
      anim = Anim::POINT;
    }
    else if( state & GESTURE1_BIT ) {
      anim = Anim::FALLBACK;
    }
    else if( state & GESTURE2_BIT ) {
      anim = Anim::SALUTE;
    }
    else if( state & GESTURE3_BIT ) {
      anim = Anim::WAVE;
    }
    else if( state & GESTURE4_BIT ) {
      anim = Anim::FLIP;
      if( !( oldState & GESTURE4_BIT ) ) {
        addEvent( EVENT_FLIP, 1.0f );
      }
    }
    else if( state & GESTURE_ALL_BIT ) {
      anim = Anim::MAX;
    }
    else {
      anim = Anim::STAND;
    }

    /*
     * MOVE
     */

    Vec3 move = Vec3::ZERO;
    state &= ~MOVING_BIT;

    if( actions & ACTION_FORWARD ) {
      flags &= ~DISABLED_BIT;
      state |= MOVING_BIT;

      if( state & SWIMMING_BIT ) {
        move.x -= hvsc[4];
        move.y += hvsc[5];
        move.z -= hvsc[3];
      }
      else if( state & CLIMBING_BIT ) {
        move.x -= hvsc[4];
        move.y += hvsc[5];
        move.z += v < Math::TAU / 4.0f ? -1.0f : 1.0f;
      }
      else {
        move.x -= hvsc[0];
        move.y += hvsc[1];
      }
    }
    if( actions & ACTION_BACKWARD ) {
      flags &= ~DISABLED_BIT;
      state |= MOVING_BIT;

      if( state & ( SWIMMING_BIT | CLIMBING_BIT ) ) {
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
      flags &= ~DISABLED_BIT;
      state |= MOVING_BIT;

      move.x += hvsc[1];
      move.y += hvsc[0];
    }
    if( actions & ACTION_LEFT ) {
      flags &= ~DISABLED_BIT;
      state |= MOVING_BIT;

      move.x -= hvsc[1];
      move.y -= hvsc[0];
    }

    if( move != Vec3::ZERO ) {
      move = ~move;

      Vec3 desiredMomentum = move;

      if( state & CROUCHING_BIT ) {
        desiredMomentum *= clazz->crouchMomentum;
      }
      else if( ( state & ( RUNNING_BIT | GRAB_BIT ) ) == RUNNING_BIT ) {
        desiredMomentum *= clazz->runMomentum;
      }
      else {
        desiredMomentum *= clazz->walkMomentum;
      }

      if( flags & ON_SLICK_BIT ) {
        desiredMomentum *= clazz->slickControl;
      }
      else if( state & CLIMBING_BIT ) {
        desiredMomentum *= clazz->climbControl;
      }
      else if( state & SWIMMING_BIT ) {
        bool isOnStaticGround = ( flags & ON_FLOOR_BIT ) ||
          ( lower != -1 && ( orbis.objects[lower]->flags & Object::DISABLED_BIT ) );

        if( !isOnStaticGround ) {
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

      // First, check if bot's gonna hit an obstacle in the next frame. If it does, check whether it
      // would have moved further if we raised it a bit (over the obstacle). We check different
      // heights (those are specified in configuration file: stepInc and stepMax).
      // To prevent that stepping would result in "climbing" high slopes, we must check that we
      // step over an edge. In other words:
      //
      //      .                                  Start and end position must be on different sides
      //  end  .     end of a failed attempt     of a obstacle side plane we collided to.
      //     \  .   /
      //      o  . x
      // ----------     collision point
      //           \   |
      //            \  |         start
      //             \ |        /
      //              \x<------o
      //               \----------
      //
      //
      if( ( state & ( STEPPING_BIT | CLIMBING_BIT ) ) == STEPPING_BIT &&
          stepRate <= clazz->stepRateLimit )
      {
        // check if bot's gonna hit a stair in next frame
        Vec3 desiredMove = momentum * Timer::TICK_TIME;

        collider.mask = flags & SOLID_BIT;
        collider.translate( this, desiredMove );

        if( collider.hit.ratio != 1.0f && collider.hit.normal.z < Physics::FLOOR_NORMAL_Z ) {
          float originalZ = p.z;
          Vec3  normal = collider.hit.normal;
          float negStartDist = ( desiredMove * collider.hit.ratio ) * normal - EPSILON;

          for( float raise = clazz->stepInc; raise <= clazz->stepMax; raise += clazz->stepInc ) {
            collider.translate( this, Vec3( 0.0f, 0.0f, clazz->stepInc ) );

            if( collider.hit.ratio != 1.0f ) {
              break;
            }
            p.z += clazz->stepInc;
            collider.translate( this, desiredMove );

            Vec3 move = desiredMove * collider.hit.ratio;
            move.z += raise;
            float endDist = move * normal - negStartDist;

            if( endDist < 0.0f ) {
              stepRate += raise*raise * clazz->stepRateCoeff;
              goto stepSucceeded;
            }
          }
          p.z = originalZ;
          stepSucceeded:;
        }

        collider.mask = SOLID_BIT;
      }
    }

    /*
     * GRAB MOVEMENT
     */

    if( state & GRAB_BIT ) {
      Bot* obj = static_cast<Bot*>( instrumentObj );

      hard_assert( obj->flags & DYNAMIC_BIT );
      hard_assert( !( obj->flags & ON_LADDER_BIT ) );

      if( obj == null || obj->cell == null || obj->p.z + obj->dim.z < p.z - dim.z ||
          ( obj->flags & UPPER_BIT ) || ( state & SWIMMING_BIT ) || ( actions & ACTION_JUMP ) ||
          ( ( obj->flags & BOT_BIT ) && ( ( obj->actions & ACTION_JUMP ) || ( obj->state & GRAB_BIT ) ) ) )
      {
        state &= ~GRAB_BIT;
        instrument = -1;
        instrumentObj = null;
      }
      else {
        // keep constant length of xy projection of handle
        Vec3 handle = Vec3( -hvsc[0], hvsc[1], -hvsc[3] ) * grabHandle;
        // bottom of the object cannot be raised over the player AABB, neither can be lowered
        // under the player (in the latter case one can lift himself with the lower object)
        handle.z    = clamp( handle.z, -dim.z - camZ, dim.z - camZ );
        Vec3 string = p + Vec3( 0.0f, 0.0f, camZ ) + handle - obj->p;

        if( string.sqL() > GRAB_HANDLE_TOL * grabHandle*grabHandle ) {
          state &= ~GRAB_BIT;
          instrument = -1;
          instrumentObj = null;
        }
        else {
          Vec3 desiredMom   = string * GRAB_STRING_RATIO;
          Vec3 momDiff      = ( desiredMom - obj->momentum ) * GRAB_MOM_RATIO;

          float momDiffSqL  = momDiff.sqL();
          momDiff.z         += Physics::G_ACCEL * Timer::TICK_TIME;
          if( momDiffSqL > GRAB_MOM_MAX_SQ ) {
            momDiff *= GRAB_MOM_MAX / Math::sqrt( momDiffSqL );
          }
          momDiff.z         -= Physics::G_ACCEL * Timer::TICK_TIME;

          obj->momentum += momDiff;
          obj->flags    &= ~DISABLED_BIT;
          flags         &= ~CLIMBER_BIT;
        }
      }
    }

    /*
     * INSTRUMENT PERSISTENCE
     */

    if( instrumentObj != null && !( state & GRAB_BIT ) && getTagged( hvsc, ~0 ) != instrumentObj ) {
      instrument = -1;
      instrumentObj = null;
    }

    /*
     * USE, TAKE, THROW, GRAB, INVENTORY USE AND INVENTORY GRAB ACTIONS
     */

    if( actions & ~oldActions & ACTION_USE ) {
      const Object* obj = instrumentObj != null ? instrumentObj : getTagged( hvsc, ~0 );

      if( obj != null ) {
        synapse.use( this, orbis.objects[obj->index] );
      }
    }
    else if( actions & ~oldActions & ACTION_TAKE ) {
      Dynamic* obj = static_cast<Dynamic*>( instrumentObj );

      if( obj == null ) {
        const Object* tagged = getTagged( hvsc, ~0 );

        if( tagged != null ) {
          obj = static_cast<Dynamic*>( orbis.objects[tagged->index] );
        }
      }

      if( obj != null ) {
        if( obj->flags & BROWSABLE_BIT ) {
          instrument = obj->index;
        }
        else if( ( obj->flags & ( ITEM_BIT | SOLID_BIT ) ) == ( ITEM_BIT | SOLID_BIT ) &&
            items.length() < clazz->nItems )
        {
          hard_assert( obj->flags & DYNAMIC_BIT );

          state &= ~GRAB_BIT;
          instrument = -1;

          obj->flags &= ~( Object::TICK_CLEAR_MASK | Object::MOVE_CLEAR_MASK );
          obj->lower = -1;
          obj->velocity = Vec3::ZERO;
          obj->momentum = Vec3::ZERO;

          items.add( obj->index );
          obj->parent = index;
          synapse.cut( obj );
        }
      }
    }
    else if( actions & ~oldActions & ACTION_THROW ) {
      if( ( state & GRAB_BIT ) && stamina >= clazz->staminaThrowDrain ) {
        Dynamic* dyn = static_cast<Dynamic*>( instrumentObj );

        hard_assert( dyn->flags & DYNAMIC_BIT );

        Vec3 handle = Vec3( -hvsc[0], hvsc[1], -hvsc[3] );

        stamina -= clazz->staminaThrowDrain;
        dyn->momentum = handle * clazz->throwMomentum;

        state &= ~GRAB_BIT;
        instrument = -1;
      }
    }
    else if( actions & ~oldActions & ACTION_GRAB ) {
      if( ( state & GRAB_BIT ) || weapon != -1 || ( state & ( CLIMBING_BIT | SWIMMING_BIT ) ) ) {
        state &= ~GRAB_BIT;
      }
      else {
        const Bot* obj = static_cast<const Bot*>( getTagged( hvsc ) );

        if( obj != null && ( obj->flags & DYNAMIC_BIT ) && obj->mass <= clazz->grabMass &&
            !( ( obj->flags & BOT_BIT ) && ( obj->state & GRAB_BIT ) ) )
        {
          float dimX = dim.x + obj->dim.x;
          float dimY = dim.y + obj->dim.y;
          float dist = Math::sqrt( dimX*dimX + dimY*dimY ) + GRAB_EPSILON;

          if( dist <= clazz->grabDistance ) {
            flags      &= ~ON_LADDER_BIT;
            state      |= GRAB_BIT;
            instrument = obj->index;
            grabHandle = dist;
          }
        }
      }
    }
    else if( actions & ~oldActions & ( ACTION_INV_GRAB | ACTION_INV_DROP ) ) {
      if( !( state & GRAB_BIT ) && taggedItem != -1 && taggedItem < items.length() ) {
        Dynamic* item = static_cast<Dynamic*>( orbis.objects[items[taggedItem]] );

        hard_assert( item != null && ( item->flags & DYNAMIC_BIT ) && ( item->flags & ITEM_BIT ) );

        float dimX = dim.x + item->dim.x;
        float dimY = dim.y + item->dim.y;
        float dist = Math::sqrt( dimX*dimX + dimY*dimY ) + GRAB_EPSILON;

        // keep constant length of xy projection of handle
        Vec3 handle = Vec3( -hvsc[0], hvsc[1], -hvsc[3] ) * dist;
        // bottom of the object cannot be raised over the player aabb
        handle.z    = clamp( handle.z, -dim.z - camZ, dim.z - camZ );
        item->p     = p + Vec3( 0.0f, 0.0f, camZ ) + handle;

        if( !collider.overlaps( item ) ) {
          item->parent = -1;
          synapse.put( item );
          items.remove( taggedItem );

          if( actions & ~oldActions & ACTION_INV_GRAB ) {
            flags      &= ~ON_LADDER_BIT;
            state      |= GRAB_BIT;
            instrument = item->index;
            grabHandle = dist;
          }
        }
      }
    }

    hard_assert( instrument != -1 || !( state & GRAB_BIT ) );

    taggedItem = -1;

    oldState   = state;
    oldActions = actions;
  }

  Bot::Bot() : actions( 0 ), oldActions( 0 ), stepRate( 0.0f ),
      instrument( -1 ), weapon( -1 ), anim( Anim::STAND )
  {}

  void Bot::heal()
  {
    life = clazz->life;
  }

  void Bot::rearm()
  {
    foreach( item, items.iter() ) {
      if( *item != -1 ) {
        Weapon* weaponObj = static_cast<Weapon*>( orbis.objects[*item] );

        if( weaponObj != null && ( weaponObj->flags & Object::WEAPON_BIT ) ) {
          const WeaponClass* weaponClazz = static_cast<const WeaponClass*>( weaponObj->clazz );

          weaponObj->nShots = weaponClazz->nShots;
        }
      }
    }
  }

  void Bot::enter( int vehicle_ )
  {
    hard_assert( cell != null && vehicle_ != -1 );

    flags      &= ~( Object::TICK_CLEAR_MASK | Object::MOVE_CLEAR_MASK );
    lower      = -1;

    parent     = vehicle_;
    actions    = 0;
    instrument = vehicle_;
    state      &= ~GRAB_BIT;
    anim       = Anim::STAND;

    synapse.cut( this );
  }

  void Bot::exit()
  {
    hard_assert( cell == null && parent != -1 );

    parent     = -1;
    actions    = 0;
    instrument = -1;

    synapse.put( this );
  }

  void Bot::kill()
  {
    life = clazz->life / 2.0f - EPSILON;
  }

  void Bot::readFull( InputStream* istream )
  {
    Dynamic::readFull( istream );

    h          = istream->readFloat();
    v          = istream->readFloat();
    state      = istream->readInt();
    oldState   = istream->readInt();
    actions    = istream->readInt();
    oldActions = istream->readInt();
    stamina    = istream->readFloat();

    instrument = istream->readInt();
    grabHandle = istream->readFloat();

    stepRate   = istream->readFloat();

    int nItems = istream->readInt();
    for( int i = 0; i < nItems; ++i ) {
      items.add( istream->readInt() );
    }
    weapon     = istream->readInt();

    anim       = Anim::Type( istream->readInt() );
    name       = istream->readString();

    const BotClass* clazz = static_cast<const BotClass*>( this->clazz );
    dim = ( state & CROUCHING_BIT ) ? clazz->dimCrouch : clazz->dim;
  }

  void Bot::writeFull( OutputStream* ostream ) const
  {
    Dynamic::writeFull( ostream );

    ostream->writeFloat( h );
    ostream->writeFloat( v );
    ostream->writeInt( state );
    ostream->writeInt( oldState );
    ostream->writeInt( actions );
    ostream->writeInt( oldActions );
    ostream->writeFloat( stamina );

    ostream->writeInt( instrument );
    ostream->writeFloat( grabHandle );

    ostream->writeFloat( stepRate );

    ostream->writeInt( items.length() );
    foreach( item, items.citer() ) {
      ostream->writeInt( *item );
    }
    ostream->writeInt( weapon );

    ostream->writeInt( int( anim ) );
    ostream->writeString( name );
  }

  void Bot::readUpdate( InputStream* istream )
  {
    Dynamic::readUpdate( istream );

    h          = istream->readFloat();
    v          = istream->readFloat();
    state      = istream->readInt();
    instrument = istream->readInt();
    anim       = Anim::Type( istream->readInt() );
  }

  void Bot::writeUpdate( OutputStream* ostream ) const
  {
    Dynamic::writeUpdate( ostream );

    ostream->writeFloat( h );
    ostream->writeFloat( v );
    ostream->writeInt( state );
    ostream->writeInt( instrument );
    ostream->writeInt( int( anim ) );
  }

}
