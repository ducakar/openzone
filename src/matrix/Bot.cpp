/*
 *  Bot.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
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
  const float Bot::GRAB_EPSILON        = 0.20f;
  const float Bot::GRAB_STRING_RATIO   = 10.0f;
  const float Bot::GRAB_MOM_RATIO      = 0.3f;
  // should be smaller than abs( Physics::HIT_THRESHOLD )
  const float Bot::GRAB_MOM_MAX        = 1.0f;
  const float Bot::GRAB_MOM_MAX_SQ     = 1.0f;
  const float Bot::DEAD_BODY_LIFT      = 100.0f;
  const float Bot::BODY_FADEOUT_FACTOR = 0.0005f;

  Pool<Bot, 1024> Bot::pool;

  void Bot::onDestroy()
  {
    // only play death sound when an alive bot is destroyed but not when a body is destroyed
    if( !( state & DEATH_BIT ) ) {
      addEvent( EVENT_DEATH, 1.0f );
    }
    Object::onDestroy();
  }

  void Bot::onHit( const Hit* hit, float hitMomentum )
  {
    if( hit->normal.z >= Physics::FLOOR_NORMAL_Z ) {
      hard_assert( hitMomentum <= 0.0f );

      addEvent( EVENT_LAND, hitMomentum * Object::MOMENTUM_INTENSITY_COEF );
    }
    else if( hitMomentum < HIT_HARD_THRESHOLD ) {
      addEvent( EVENT_HIT_HARD, hitMomentum * Object::MOMENTUM_INTENSITY_COEF );
    }
  }

  void Bot::onUpdate()
  {
    const BotClass* clazz = static_cast<const BotClass*>( this->clazz );

    // clear invalid references from inventory
    for( int i = 0; i < items.length(); ) {
      if( orbis.objects[ items[i] ] == null ) {
        items.remove( i );
      }
      else {
        ++i;
      }
    }

    if( life <= clazz->life / 2.0f ) {
      if( !( state & DEATH_BIT ) && life > 0.0f ) {
        flags |= WIDE_CULL_BIT;
        flags &= ~SOLID_BIT;
        addEvent( EVENT_DEATH, 1.0f );
        life = clazz->life / 2.0f - EPSILON;
        anim = Anim::DEATH_FALLBACK;
      }
      state |= DEATH_BIT;
    }
    if( state & DEATH_BIT ) {
      // if body gets destroyed during fade out, skip this, or Object::destroy() won't be called
      if( life > 0.0f ) {
        life -= clazz->life * BODY_FADEOUT_FACTOR;
        // we don't want Object::destroy() to be called when body dissolves (destroy() causes sounds
        // and particles to fly around), that's why we remove the object
        if( life <= 0.0f ) {
          foreach( i, items.citer() ) {
            synapse.removeCut( static_cast<Dynamic*>( orbis.objects[*i] ) );
          }
          life = EPSILON;
          synapse.remove( this );
        }
      }
      return;
    }

    h = Math::mod( h + Math::TAU, Math::TAU );
    v = bound( v, 0.0f, Math::TAU / 2.0f );

    // { hsine, hcosine, vsine, vcosine, vsine * hsine, vsine * hcosine }
    float hvsc[6];

    Math::sincos( h, &hvsc[0], &hvsc[1] );
    Math::sincos( v, &hvsc[2], &hvsc[3] );

    hvsc[4] = hvsc[2] * hvsc[0];
    hvsc[5] = hvsc[2] * hvsc[1];

    if( parent != -1 ) {
      grabObj = -1;
      taggedItem = -1;
      weaponItem = -1;

      if( orbis.objects[parent] == null ) {
        exit();
      }
      else {
        hard_assert( orbis.objects[parent]->flags & VEHICLE_BIT );
        return;
      }
    }

    /*
     * STATE
     */

    state &= ~( GROUNDED_BIT | CLIMBING_BIT | SWIMMING_BIT | SUBMERGED_BIT );

    state |= lower != -1 || ( flags & ON_FLOOR_BIT )    ? GROUNDED_BIT  : 0;
    state |= ( flags & ON_LADDER_BIT ) && grabObj == -1 ? CLIMBING_BIT  : 0;
    state |= depth > dim.z && ( ~state & GROUNDED_BIT ) ? SWIMMING_BIT  : 0;
    state |= depth > dim.z + camZ                       ? SUBMERGED_BIT : 0;

    flags |= CLIMBER_BIT;

    stepRate -= ( velocity.x*velocity.x + velocity.y*velocity.y );
    stepRate *= clazz->stepRateSupp;

    stamina += clazz->staminaGain;
    stamina = bound( stamina, 0.0f, clazz->stamina );

    if( state & SUBMERGED_BIT ) {
      stamina -= clazz->staminaWaterDrain;

      if( stamina < 0.0f ) {
        life += stamina;
        stamina = 0.0f;
      }
    }

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
          grabObj == -1 && stamina >= clazz->staminaJumpDrain )
      {
        flags &= ~DISABLED_BIT;
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

        p.z += clazz->dim.z - clazz->dimCrouch.z;
        dim = clazz->dim;

        if( !collider.overlaps( *this, this ) ) {
          camZ   = clazz->camZ;
          state  &= ~CROUCHING_BIT;
        }
        else {
          dim = clazz->dimCrouch;
          p.z = oldZ;
        }
      }
      else {
        flags &= ~DISABLED_BIT;
        flags &= ~Object::ON_FLOOR_BIT;
        lower =  -1;

        p.z    += dim.z - clazz->dimCrouch.z;
        dim.z  = clazz->dimCrouch.z;
        camZ   = clazz->crouchCamZ;
        state  |= CROUCHING_BIT;
      }
    }
    if( stamina < clazz->staminaRunDrain ) {
      state &= ~RUNNING_BIT;
    }

    /*
     * ANIMATION
     */

    if( ( actions & ACTION_JUMP ) && ( ~state & GROUNDED_BIT ) ) {
      anim = Anim::JUMP;
    }
    else if( actions & ( ACTION_FORWARD | ACTION_BACKWARD | ACTION_LEFT | ACTION_RIGHT ) ) {
      anim = ( state & CROUCHING_BIT ) ? Anim::CROUCH_WALK : Anim::RUN;
    }
    else if( ( actions & ACTION_ATTACK ) && weaponItem != -1 && grabObj == -1 ) {
      Weapon* weapon = static_cast<Weapon*>( orbis.objects[weaponItem] );

      if( weapon != null && weapon->shotTime == 0.0f ) {
        anim = ( state & CROUCHING_BIT ) ? Anim::CROUCH_ATTACK : Anim::ATTACK;
        weapon->trigger( this );
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
    if( actions & ACTION_SUICIDE ) {
      life = clazz->life / 2.0f - EPSILON;
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
      else if( ( state & RUNNING_BIT ) && grabObj == -1 ) {
        desiredMomentum *= clazz->runMomentum;
      }
      else {
        desiredMomentum *= clazz->walkMomentum;
      }

      if( flags & ON_SLICK_BIT ) {
        desiredMomentum *= clazz->slickControl;
      }
      else if( ~state & GROUNDED_BIT ) {
        if( state & CLIMBING_BIT ) {
          desiredMomentum *= clazz->climbControl;
        }
        else if( state & SWIMMING_BIT ) {
          desiredMomentum *= clazz->waterControl;
        }
        else {
          desiredMomentum *= clazz->airControl;
        }
      }

      if( ( flags & Object::ON_FLOOR_BIT ) && floor.z != 1.0f ) {
        float dot = desiredMomentum * floor;

        if( dot > 0.0f ) {
          desiredMomentum -= dot * floor;
        }
      }
      momentum += desiredMomentum;

      if( state & ( RUNNING_BIT | GROUNDED_BIT | SWIMMING_BIT | CLIMBING_BIT ) ) {
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
      if( ( state & STEPPING_BIT ) && ( ~state & CLIMBING_BIT ) &&
          stepRate <= clazz->stepRateLimit )
      {
        // check if bot's gonna hit a stair in next frame
        Vec3 desiredMove = momentum * Timer::TICK_TIME;

        collider.mask = flags & SOLID_BIT;
        collider.translate( *this, desiredMove, this );

        if( collider.hit.ratio != 1.0f && collider.hit.normal.z < Physics::FLOOR_NORMAL_Z ) {
          float originalZ = p.z;
          Vec3  normal = collider.hit.normal;
          float negStartDist = ( desiredMove * collider.hit.ratio ) * normal - EPSILON;

          for( float raise = clazz->stepInc; raise <= clazz->stepMax; raise += clazz->stepInc ) {
            collider.translate( *this, Vec3( 0.0f, 0.0f, clazz->stepInc ) );

            if( collider.hit.ratio != 1.0f ) {
              break;
            }
            p.z += clazz->stepInc;
            collider.translate( *this, desiredMove, this );

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

    Dynamic* obj = null;
    if( grabObj != -1 ) {
      obj = static_cast<Dynamic*>( orbis.objects[grabObj] );

      if( obj == null || obj->cell == null || weaponItem != -1 ) {
        grabObj = -1;
        obj = null;
      }
    }

    if( grabObj != -1 ) {
      if( lower == grabObj || ( state & SWIMMING_BIT ) || ( obj->flags & Object::UPPER_BIT ) ) {
        grabObj = -1;
      }
      else {
        // keep constant length of xy projection of handle
        Vec3 handle = Vec3( -hvsc[0], hvsc[1], -hvsc[3] ) * grabHandle;
        // bottom of the object cannot be raised over the player aabb
        handle.z    = min( handle.z, dim.z - camZ + obj->dim.z );
        Vec3 string = p + Vec3( 0.0f, 0.0f, camZ ) + handle - obj->p;

        if( string.sqL() > grabHandle*grabHandle ) {
          grabObj = -1;
        }
        else {
          Vec3 desiredMom   = string * GRAB_STRING_RATIO;
          Vec3 momDiff      = ( desiredMom - obj->momentum ) * GRAB_MOM_RATIO;

          float momDiffSqL  = momDiff.sqL();
          momDiff.z         += Physics::G_MOMENTUM;
          if( momDiffSqL > GRAB_MOM_MAX_SQ ) {
            momDiff *= GRAB_MOM_MAX / Math::sqrt( momDiffSqL );
          }
          momDiff.z         -= Physics::G_MOMENTUM;

          obj->momentum += momDiff;
          obj->flags    &= ~Object::DISABLED_BIT;
          flags         &= ~CLIMBER_BIT;
        }
      }
    }

    /*
     * USE, TAKE, THROW, GRAB, INVENTORY USE AND INVENTORY GRAB ACTIONS
     */

    if( actions & ~oldActions & ACTION_USE ) {
      if( grabObj != -1 ) {
        synapse.use( this, obj );
      }
      else {
        Point3 eye  = p + Vec3( 0.0f, 0.0f, camZ );
        Vec3   look = Vec3( -hvsc[4], hvsc[5], -hvsc[3] ) * clazz->grabDistance;

        collider.translate( eye, look, this );

        if( collider.hit.obj != null ) {
          synapse.use( this, const_cast<Object*>( collider.hit.obj ) );
        }
      }
    }
    else if( actions & ~oldActions & ACTION_TAKE ) {
      if( grabObj != -1 ) {
        if( obj->flags & ITEM_BIT ) {
          take( obj );
        }
      }
      else {
        Point3 eye  = p + Vec3( 0.0f, 0.0f, camZ );
        Vec3   look = Vec3( -hvsc[4], hvsc[5], -hvsc[3] ) * clazz->grabDistance;

        collider.translate( eye, look, this );

        const Dynamic* obj = static_cast<const Dynamic*>( collider.hit.obj );
        if( obj != null && ( obj->flags & ITEM_BIT ) ) {
          hard_assert( obj->flags & DYNAMIC_BIT );

          take( const_cast<Dynamic*>( obj ) );
        }
      }
    }
    else if( actions & ~oldActions & ACTION_THROW ) {
      if( grabObj != -1 && stamina >= clazz->staminaThrowDrain ) {
        Vec3 handle = Vec3( -hvsc[0], hvsc[1], -hvsc[3] );

        stamina -= clazz->staminaThrowDrain;
        obj->momentum = handle * clazz->throwMomentum;
        grabObj = -1;
      }
    }
    else if( actions & ~oldActions & ACTION_GRAB ) {
      if( grabObj != -1 || ( state & SWIMMING_BIT ) || weaponItem != -1 ) {
        grabObj = -1;
      }
      else {
        Point3 eye  = p + Vec3( 0.0f, 0.0f, camZ );
        Vec3   look = Vec3( -hvsc[4], hvsc[5], -hvsc[3] ) * clazz->grabDistance;

        collider.translate( eye, look, this );

        const Dynamic* obj = static_cast<const Dynamic*>( collider.hit.obj );
        const Bot* bot = static_cast<const Bot*>( collider.hit.obj );

        if( obj != null && ( obj->flags & DYNAMIC_BIT ) && obj->mass <= clazz->grabMass &&
            lower != obj->index && ( ( ~obj->flags & BOT_BIT ) || bot->grabObj == -1 ) )
        {
          float dimX = dim.x + obj->dim.x;
          float dimY = dim.y + obj->dim.y;
          float dist = Math::sqrt( dimX*dimX + dimY*dimY ) + GRAB_EPSILON;

          if( dist <= clazz->grabDistance ) {
            grabObj   = obj->index;
            grabHandle = dist;
            flags      &= ~ON_LADDER_BIT;
          }
        }
      }
    }
    else if( actions & ~oldActions & ACTION_INV_USE ) {
      if( taggedItem != -1 && taggedItem < items.length() && items[taggedItem] != -1 ) {
        Dynamic* item = static_cast<Dynamic*>( orbis.objects[items[taggedItem]] );

        hard_assert( item != null && ( item->flags & DYNAMIC_BIT ) && ( item->flags & ITEM_BIT ) );

        synapse.use( this, item );
        // the object may have removed itself after use
        if( item->index == -1 ) {
          items.remove( taggedItem );
        }
      }
    }
    else if( actions & ~oldActions & ACTION_INV_GRAB ) {
      if( grabObj == -1 && taggedItem != -1 && taggedItem < items.length() ) {
        Dynamic* item = static_cast<Dynamic*>( orbis.objects[items[taggedItem]] );

        hard_assert( item != null && ( item->flags & DYNAMIC_BIT ) && ( item->flags & ITEM_BIT ) );

        float dimX = dim.x + item->dim.x;
        float dimY = dim.y + item->dim.y;
        float dist = Math::sqrt( dimX*dimX + dimY*dimY ) + GRAB_EPSILON;

        // keep constant length of xy projection of handle
        Vec3 handle = Vec3( -hvsc[0], hvsc[1], -hvsc[3] ) * dist;
        // bottom of the object cannot be raised over the player aabb
        handle.z    = min( handle.z, dim.z - camZ + item->dim.z );
        item->p     = p + Vec3( 0.0f, 0.0f, camZ ) + handle;

        if( !collider.overlaps( *item ) ) {
          item->parent = -1;
          synapse.put( item );
          items.remove( taggedItem );

          grabObj   = item->index;
          grabHandle = dist;
          flags      &= ~ON_LADDER_BIT;

          if( item->index == weaponItem ) {
            weaponItem = -1;
          }
        }
      }
    }

    /*
     * WEAPON
     */
    if( weaponItem != -1 ) {
      Dynamic* weapon = static_cast<Dynamic*>( orbis.objects[weaponItem] );

      hard_assert( ( weapon->flags & DYNAMIC_BIT ) && ( weapon->flags & ITEM_BIT ) &&
              ( weapon->flags & WEAPON_BIT ) );

      if( weapon == null ) {
        weaponItem = -1;
      }
    }

    oldState   = state;
    oldActions = actions;
  }

  void Bot::onAct()
  {}

  Bot::Bot() : actions( 0 ), oldActions( 0 ), stepRate( 0.0f ),
      grabObj( -1 ), weaponItem( -1 ), anim( Anim::STAND )
  {}

  void Bot::take( Dynamic* item )
  {
    hard_assert( index != -1 && ( item->flags & Object::ITEM_BIT ) );

    items.add( item->index );
    item->parent = index;
    synapse.cut( item );
  }

  void Bot::enter( int vehicle_ )
  {
    hard_assert( cell != null );

    parent   = vehicle_;
    grabObj  = -1;
    anim     = Anim::STAND;
    synapse.cut( this );
  }

  void Bot::exit()
  {
    hard_assert( cell == null && parent != -1 );

    parent = -1;
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

    grabObj    = istream->readInt();
    grabHandle = istream->readFloat();

    stepRate     = istream->readFloat();

    int nItems = istream->readInt();
    for( int i = 0; i < nItems; ++i ) {
      items.add( istream->readInt() );
    }
    weaponItem = istream->readInt();

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

    ostream->writeInt( grabObj );
    ostream->writeFloat( grabHandle );

    ostream->writeFloat( stepRate );

    ostream->writeInt( items.length() );
    foreach( item, items.citer() ) {
      ostream->writeInt( *item );
    }
    ostream->writeInt( weaponItem );

    ostream->writeInt( int( anim ) );
    ostream->writeString( name );
  }

  void Bot::readUpdate( InputStream* istream )
  {
    Dynamic::readUpdate( istream );

    h            = istream->readFloat();
    v            = istream->readFloat();
    state        = istream->readInt();
    grabObj      = istream->readInt();
    anim         = Anim::Type( istream->readInt() );
  }

  void Bot::writeUpdate( OutputStream* ostream ) const
  {
    Dynamic::writeUpdate( ostream );

    ostream->writeFloat( h );
    ostream->writeFloat( v );
    ostream->writeInt( state );
    ostream->writeInt( grabObj );
    ostream->writeInt( int( anim ) );
  }

}
