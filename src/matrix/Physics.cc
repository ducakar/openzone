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
 * @file matrix/Physics.cc
 */

#include "stable.hh"

#include "matrix/Physics.hh"

namespace oz
{
namespace matrix
{

Physics physics;

const float Physics::FLOOR_NORMAL_Z          =  0.60f;
const float Physics::MOVE_BOUNCE             =  EPSILON;
const float Physics::ENTITY_BOND_G_RATIO     =  0.10f;
const float Physics::SIDE_PUSH_RATIO         =  0.40f;

const float Physics::MAX_HIT_MASS            =  100.0f;
const float Physics::HIT_THRESHOLD           = -3.0f;
const float Physics::HIT_INTENSITY_COEF      =  0.02f;
const float Physics::HIT_ENERGY_COEF         =  0.01f;
const float Physics::SPLASH_THRESHOLD        = -2.0f;
const float Physics::SPLASH_INTENSITY_COEF   =  0.02f;

const float Physics::WEIGHT_DAMAGE_THRESHOLD =  1000.0f;
const float Physics::WEIGHT_DAMAGE_FACTOR    =  20.0f;
const float Physics::SLIDE_DAMAGE_THRESHOLD  =  50.0f;
const float Physics::SLIDE_DAMAGE_COEF       = -2.5f;

const float Physics::STICK_VELOCITY          =  0.03f;
const float Physics::SLICK_STICK_VELOCITY    =  0.003f;
const float Physics::FLOAT_STICK_VELOCITY    =  0.0005f;
const float Physics::WATER_FRICTION          =  0.09f;
const float Physics::LADDER_FRICTION         =  0.15f;
const float Physics::FLOOR_FRICTION_COEF     =  0.30f;
const float Physics::SLICK_FRICTION_COEF     =  0.03f;

const float Physics::LAVA_LIFT               =  1.2f;
const float Physics::LAVA_DAMAGE_ABSOLUTE    =  175.0f;
const float Physics::LAVA_DAMAGE_RATIO       =  0.25f;
const int   Physics::LAVA_DAMAGE_INTERVAL    =  Timer::TICKS_PER_SEC / 2;

const float Physics::FRAG_HIT_VELOCITY2      =  100.0f;
const float Physics::FRAG_DESTROY_VELOCITY2  =  300.0f;
const float Physics::FRAG_DAMAGE_COEF        =  0.05f;
const float Physics::FRAG_FIXED_DAMAGE       =  0.75f;

//***********************************
//*   FRAGMENT COLLISION HANDLING   *
//***********************************

void Physics::handleFragHit()
{
  Vec3  fragVelocity = frag->velocity;
  float velocity2    = frag->velocity.sqN();

  frag->velocity *= frag->elasticity;
  frag->velocity -= ( 2.0f * ( frag->velocity * collider.hit.normal ) ) * collider.hit.normal;

  if( velocity2 > FRAG_HIT_VELOCITY2 ) {
    if( frag->mass != 0.0f ) {
      if( collider.hit.str != null ) {
        Struct* str = collider.hit.str;
        float damage = FRAG_DAMAGE_COEF * velocity2 * frag->mass;

        if( damage > str->resistance ) {
          damage *= FRAG_FIXED_DAMAGE + ( 1.0f - FRAG_FIXED_DAMAGE ) * Math::rand();
          str->damage( damage );
        }
      }
      else if( collider.hit.obj != null ) {
        Object* obj = collider.hit.obj;
        float damage = FRAG_DAMAGE_COEF * velocity2 * frag->mass;

        if( damage > obj->resistance ) {
          damage *= FRAG_FIXED_DAMAGE + ( 1.0f - FRAG_FIXED_DAMAGE ) * Math::rand();
          obj->damage( damage );
        }

        if( obj->flags & Object::DYNAMIC_BIT ) {
          Dynamic* dyn = static_cast<Dynamic*>( obj );

          float fragMass = frag->mass * 10.0f;
          float massSum  = fragMass + dyn->mass;

          dyn->flags   &= ~Object::DISABLED_BIT;
          dyn->momentum = ( fragVelocity * fragMass + dyn->momentum * dyn->mass ) / massSum;
        }
      }
    }

    if( velocity2 > FRAG_DESTROY_VELOCITY2 ) {
      // We abuse velocity to hold the normal of the fatal hit, needed for positioning decals.
      frag->velocity = collider.hit.normal;
      frag->life     = -Math::INF;
    }
  }
}

void Physics::handleFragMove()
{
  move = frag->velocity * Timer::TICK_TIME;

  float leftRatio = 1.0f;

  int traceSplits = 0;
  do {
    collider.translate( frag->p, move );
    frag->p += collider.hit.ratio * move;
    leftRatio -= leftRatio * collider.hit.ratio;

    if( collider.hit.ratio == 1.0f ) {
      break;
    }
    // collision response
    handleFragHit();

    // We must check lifeTime <= 0.0f to prevent an already destroyed fragment to bounce off a
    // surface and hit something and to position decal properly.
    if( traceSplits >= 3 || frag->life <= 0.0f ) {
      break;
    }
    ++traceSplits;

    move *= 1.0f - collider.hit.ratio;
    move -= ( move * collider.hit.normal - MOVE_BOUNCE ) * collider.hit.normal;
  }
  while( true );

  orbis.reposition( frag );
}

//***********************************
//*    OBJECT COLLISION HANDLING    *
//***********************************

bool Physics::handleObjFriction()
{
  float systemMom = gravity * Timer::TICK_TIME;

  if( dyn->flags & Object::IN_LIQUID_BIT ) {
    float lift = dyn->flags & Object::IN_LAVA_BIT ? LAVA_LIFT : dyn->lift;
    float frictionFactor = 0.5f * dyn->depth / dyn->dim.z;

    dyn->momentum *= 1.0f - frictionFactor * WATER_FRICTION;
    systemMom -= frictionFactor * lift * gravity * Timer::TICK_TIME;
  }

  if( dyn->flags & Object::ON_LADDER_BIT ) {
    float momentum2 = dyn->momentum.sqN();

    if( momentum2 <= STICK_VELOCITY ) {
      dyn->momentum = Vec3::ZERO;

      return dyn->flags & Object::ENABLE_BIT;
    }
    else {
      dyn->momentum *= 1.0f - LADDER_FRICTION;
    }
  }
  else {
    bool isLowerStill = true;

    float deltaVelX = dyn->momentum.x;
    float deltaVelY = dyn->momentum.y;

    if( dyn->lower >= 0 ) {
      if( dyn->flags & Object::ON_FLOOR_BIT ) {
        int structIndex = dyn->lower / Struct::MAX_ENTITIES;
        int entityIndex = dyn->lower % Struct::MAX_ENTITIES;

        const Entity& entity = orbis.structs[structIndex]->entities[entityIndex];

        if( entity.velocity != Vec3::ZERO ) {
          isLowerStill = false;

          deltaVelX -= entity.velocity.x;
          deltaVelY -= entity.velocity.y;

          // Push a little into entity if e.g. on an elevator going down.
          if( entity.velocity.z < 0.0f && !( dyn->flags & Object::IN_LIQUID_BIT ) ) {
            systemMom += ENTITY_BOND_G_RATIO * gravity;
          }
        }
      }
      else {
        const Dynamic* sDyn = static_cast<const Dynamic*>( orbis.objects[dyn->lower] );

        if( sDyn->velocity != Vec3::ZERO ) {
          isLowerStill = false;

          deltaVelX -= sDyn->velocity.x;
          deltaVelY -= sDyn->velocity.y;
        }
      }
    }

    // on floor or on a still object
    if( ( dyn->flags & Object::ON_FLOOR_BIT ) || dyn->lower >= 0  ) {
      float deltaVel2 = deltaVelX*deltaVelX + deltaVelY*deltaVelY;
      float friction  = FLOOR_FRICTION_COEF;
      float stickVel  = STICK_VELOCITY;

      if( dyn->flags & Object::ON_SLICK_BIT ) {
        friction = SLICK_FRICTION_COEF;
        stickVel = SLICK_STICK_VELOCITY;
      }

      dyn->momentum   += ( systemMom * dyn->floor.z ) * dyn->floor;
      dyn->momentum.x -= deltaVelX * friction;
      dyn->momentum.y -= deltaVelY * friction;
      dyn->momentum.z *= 1.0f - friction;

      // Push into floor just enough that collision occurs continuously each tick.
      dyn->momentum.z -= EPSILON / Timer::TICK_TIME;

      if( deltaVel2 > stickVel ) {
        dyn->flags |= Object::FRICTING_BIT;

        if( deltaVel2 > SLIDE_DAMAGE_THRESHOLD ) {
          dyn->damage( SLIDE_DAMAGE_COEF * Math::fastSqrt( deltaVel2 ) * friction * gravity );
        }
      }
      else if( isLowerStill ) {
        dyn->momentum.x = 0.0f;
        dyn->momentum.y = 0.0f;

        if( dyn->momentum.z <= 0.0f && !( dyn->flags & Object::ENABLE_BIT ) ) {
          dyn->momentum.z = 0.0f;

          if( systemMom <= 0.0f ) {
            return dyn->flags & Object::ENABLE_BIT;
          }
        }
      }
    }
    // in air or swimming
    else {
      dyn->momentum.z += systemMom;

      if( Math::fabs( systemMom ) <= FLOAT_STICK_VELOCITY &&
          dyn->momentum.sqN() <= FLOAT_STICK_VELOCITY )
      {
        dyn->momentum = Vec3::ZERO;

        return dyn->flags & Object::ENABLE_BIT;
      }
    }
  }

  return true;
}

void Physics::handleObjHit()
{
  const Hit& hit = collider.hit;

  if( hit.obj != null && ( hit.obj->flags & Object::DYNAMIC_BIT ) ) {
    Dynamic* sDyn = static_cast<Dynamic*>( hit.obj );

    float massSum     = dyn->mass + sDyn->mass;
    Vec3  momentum    = ( dyn->momentum * dyn->mass + sDyn->momentum * sDyn->mass ) / massSum;
    float hitMomentum = ( dyn->momentum - sDyn->momentum ) * hit.normal;
    float hitVelocity = dyn->velocity * hit.normal;

    if( hitMomentum < HIT_THRESHOLD && hitVelocity < HIT_THRESHOLD ) {
      float momentum2 = hitMomentum*hitMomentum;
      float energy    = min( dyn->mass, MAX_HIT_MASS ) * momentum2;
      float intensity = momentum2 * HIT_INTENSITY_COEF;
      float damage    = energy * HIT_ENERGY_COEF;
      // Since it can only be -1, 0 or +1, it's enough to test for sign.
      bool  hasLanded = hit.normal.z > 0.0f;

      dyn->addEvent( Object::EVENT_HIT + hasLanded, intensity );
      dyn->damage( damage );

      sDyn->addEvent( Object::EVENT_HIT, intensity );
      sDyn->damage( damage );
    }

    if( hit.normal.z == 0.0f ) {
      float momProj     =       momentum.x * hit.normal.x +       momentum.y * hit.normal.y;
      float dynMomProj  =  dyn->momentum.x * hit.normal.x +  dyn->momentum.y * hit.normal.y;
      float sDynMomProj = sDyn->momentum.x * hit.normal.x + sDyn->momentum.y * hit.normal.y;
      float sDynVelProj = sDyn->velocity.x * hit.normal.x + sDyn->velocity.y * hit.normal.y;
      float directPushX = ( momProj - sDynMomProj ) * hit.normal.x;
      float directPushY = ( momProj - sDynMomProj ) * hit.normal.y;

      dyn->momentum.x -= ( dynMomProj - sDynVelProj ) * hit.normal.x;
      dyn->momentum.y -= ( dynMomProj - sDynVelProj ) * hit.normal.y;

      sDyn->flags      &= ~Object::DISABLED_BIT;
      sDyn->momentum.x += directPushX;
      sDyn->momentum.y += directPushY;

      if( dyn->flags & Object::BOT_BIT ) {
        float pushX = momentum.x - sDyn->momentum.x;
        float pushY = momentum.y - sDyn->momentum.y;

        sDyn->momentum.x += SIDE_PUSH_RATIO * ( pushX - directPushX );
        sDyn->momentum.y += SIDE_PUSH_RATIO * ( pushY - directPushY );

        // Allow side-pushing downwards in water.
        if( ( sDyn->flags & Object::IN_LIQUID_BIT ) && momentum.z < 0.0f ) {
          sDyn->momentum.z += SIDE_PUSH_RATIO * ( momentum.z - sDyn->momentum.z );
        }
      }
    }
    else if( hit.normal.z == -1.0f ) {
      dyn->flags      |= Object::BELOW_BIT;
      dyn->momentum.z  = sDyn->velocity.z;

      sDyn->flags     &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT );
      sDyn->lower      = dyn->index;
      sDyn->floor      = Vec3( 0.0f, 0.0f, 1.0f );
      sDyn->momentum.z = momentum.z;
    }
    else { // hit.normal.z == 1.0f
      hard_assert( hit.normal.z == 1.0f );

      dyn->flags     &= ~Object::ON_FLOOR_BIT;
      dyn->lower      = sDyn->index;
      dyn->floor      = Vec3( 0.0f, 0.0f, 1.0f );
      dyn->momentum.z = sDyn->velocity.z;

      sDyn->flags    |= Object::BELOW_BIT;

      if( dyn->mass > WEIGHT_DAMAGE_THRESHOLD ) {
        sDyn->damage( dyn->mass / sDyn->mass * WEIGHT_DAMAGE_FACTOR );
      }

      if( !( sDyn->flags & Object::ON_FLOOR_BIT ) && sDyn->lower < 0 ) {
        sDyn->flags     &= ~Object::DISABLED_BIT;
        sDyn->momentum.z = momentum.z;
      }
    }
  }
  else {
    float hitMomentum = dyn->momentum * hit.normal;
    float hitVelocity = dyn->velocity * hit.normal;

    if( hitMomentum < HIT_THRESHOLD && hitVelocity < HIT_THRESHOLD ) {
      float momentum2 = hitMomentum*hitMomentum;
      float energy    = min( dyn->mass, MAX_HIT_MASS ) * momentum2;
      float intensity = momentum2 * HIT_INTENSITY_COEF;
      float damage    = energy * HIT_ENERGY_COEF;
      // Since it can only be -1, 0 or +1, it's enough to test for sign.
      bool  hasLanded = hit.normal.z >= FLOOR_NORMAL_Z;

      dyn->addEvent( Object::EVENT_HIT + hasLanded, intensity );
      dyn->damage( damage );

      if( hit.obj != null ) {
        hit.obj->addEvent( Object::EVENT_HIT, intensity );
        hit.obj->damage( damage );
      }
      else if( hit.str != null ) {
        hit.str->damage( damage );
      }
    }

    dyn->momentum -= ( dyn->momentum * hit.normal ) * hit.normal;

    if( hit.normal.z >= FLOOR_NORMAL_Z ) {
      dyn->flags |= Object::ON_FLOOR_BIT;
      dyn->flags |= hit.material & Material::SLICK_BIT ? Object::ON_SLICK_BIT : 0;
      dyn->floor  = hit.normal;

      if( hit.entity == null ) {
        dyn->lower = -1;
      }
      else {
        int structIndex = hit.str->index;
        int entityIndex = int( hit.entity - hit.str->entities );

        hard_assert( uint( entityIndex ) < uint( Struct::MAX_ENTITIES ) );

        dyn->lower = structIndex * Struct::MAX_ENTITIES + entityIndex;
      }
    }
  }
}

void Physics::handleObjMove()
{
  move = dyn->momentum * Timer::TICK_TIME;

  float moveLen = !move;
  if( moveLen == 0.0f ) {
    return;
  }

  Vec3  originalDir = move / moveLen;
  float leftRatio   = 1.0f;

  int traceSplits = 0;
  do {
    collider.translate( dyn, move );
    dyn->p += collider.hit.ratio * move;
    leftRatio -= leftRatio * collider.hit.ratio;

    if( collider.hit.ratio == 1.0f ) {
      break;
    }
    // collision response
    handleObjHit();

    if( traceSplits >= 3 || dyn->life <= 0.0f ) {
      break;
    }
    ++traceSplits;

    move *= 1.0f - collider.hit.ratio;
    move -= ( move * collider.hit.normal - MOVE_BOUNCE ) * collider.hit.normal;

    // In obtuse corners (> 90°) we prevent oscillations by preventing move in the opposite
    // direction form the original one.
    move -= min( move * originalDir + MOVE_BOUNCE, 0.0f ) * originalDir;

    // In acute (< 90°) corners we move the object a little out of it to prevent it getting stuck.
    if( traceSplits == 1 ) {
      lastNormals[0] = collider.hit.normal;
    }
    else {
      float dot = lastNormals[0] * collider.hit.normal;

      if( dot < 0.0f ) {
        Vec3  cross    = collider.hit.normal ^ lastNormals[0];
        float crossSqN = cross.sqN();

        if( crossSqN == 0.0f ) {
          move = Vec3::ZERO;
        }
        else {
          float length_1 = Math::fastInvSqrt( crossSqN );

          cross *= length_1;
          move   = ( move * cross ) * cross;
          move  += 3.0f * MOVE_BOUNCE * length_1 * ( collider.hit.normal + lastNormals[0] );
        }
      }

      if( traceSplits == 2 ) {
        lastNormals[1] = lastNormals[0];
        lastNormals[0] = collider.hit.normal;
      }
      else {
        dot = lastNormals[1] * collider.hit.normal;

        if( dot < 0.0f ) {
          Vec3  cross    = collider.hit.normal ^ lastNormals[1];
          float crossSqN = cross.sqN();

          if( crossSqN == 0.0f ) {
            move = Vec3::ZERO;
          }
          else {
            float length_1 = Math::fastInvSqrt( crossSqN );

            cross /= length_1;
            move   = ( move * cross ) * cross;
            move  += 3.0f * MOVE_BOUNCE * length_1 * ( collider.hit.normal + lastNormals[1] );
          }
        }
      }
    }
  }
  while( true );

  orbis.reposition( dyn );
}

//***********************************
//*             PUBLIC              *
//***********************************

void Physics::updateFrag( Frag* frag_ )
{
  frag = frag_;

  hard_assert( frag->cell != null );

  frag->velocity.z += gravity * Timer::TICK_TIME;

  handleFragMove();
}

void Physics::updateObj( Dynamic* dyn_ )
{
  dyn = dyn_;

  hard_assert( dyn->cell != null );

  dyn->flags &= ~Object::TICK_CLEAR_MASK;

  if( dyn->lower >= 0 ) {
    if( dyn->flags & Object::ON_FLOOR_BIT ) {
      int structIndex = dyn->lower / Struct::MAX_ENTITIES;
      int entityIndex = dyn->lower % Struct::MAX_ENTITIES;

      const Struct* str = orbis.structs[structIndex];

      if( str == null ) {
        dyn->flags &= ~Object::DISABLED_BIT;
        dyn->lower  = -1;
      }
      else {
        const Entity& entity = str->entities[entityIndex];

        if( entity.state == Entity::OPENING || entity.state == Entity::CLOSING ) {
          dyn->flags &= ~Object::DISABLED_BIT;
        }
      }
    }
    else {
      const Object* sObj = orbis.objects[dyn->lower];

      // clear the lower object if it doesn't exist any more
      if( sObj == null || sObj->cell == null ) {
        dyn->flags &= ~Object::DISABLED_BIT;
        dyn->lower  = -1;
      }
      else {
        dyn->flags &= sObj->flags | ~Object::DISABLED_BIT;
      }
    }
  }

  // handle physics
  if( !( dyn->flags & Object::DISABLED_BIT ) ) {
    if( handleObjFriction() ) {
      Point oldPos   = dyn->p;
      int   oldFlags = dyn->flags;

      dyn->flags &= ~( Object::MOVE_CLEAR_MASK | Object::ENABLE_BIT );
      dyn->lower  = -1;

      collider.mask = dyn->flags & Object::SOLID_BIT;
      handleObjMove();
      collider.mask = Object::SOLID_BIT;

      if( collider.hit.medium & Medium::LADDER_BIT ) {
        dyn->flags |= Object::ON_LADDER_BIT;
      }
      if( collider.hit.medium & Medium::WATER_BIT ) {
        dyn->flags |= Object::IN_LIQUID_BIT;

        if( !( oldFlags & Object::IN_LIQUID_BIT ) && dyn->velocity.z <= SPLASH_THRESHOLD ) {
          float momentum2 = dyn->velocity.z*dyn->velocity.z;
          float intensity = momentum2 * SPLASH_INTENSITY_COEF;

          dyn->addEvent( Object::EVENT_SPLASH, intensity );
        }
      }
      if( collider.hit.medium & Medium::LAVA_BIT ) {
        dyn->flags |= Object::IN_LIQUID_BIT | Object::IN_LAVA_BIT;

        if( dyn->resistance <= LAVA_DAMAGE_ABSOLUTE ) {
          dyn->flags |= Object::ENABLE_BIT;

          // 199999 is some large enough prime to introduce enough spread among indices.
          if( ( uint( timer.ticks ) + uint( dyn->index * 199999 ) ) % LAVA_DAMAGE_INTERVAL == 0 ) {
            dyn->damage( max( LAVA_DAMAGE_ABSOLUTE, dyn->clazz->life * LAVA_DAMAGE_RATIO ) );
          }
        }
      }

      dyn->velocity = ( dyn->p - oldPos ) / Timer::TICK_TIME;
      dyn->momentum = dyn->velocity;
      dyn->depth    = min( collider.hit.depth, 2.0f * dyn->dim.z );
    }
    else {
      hard_assert( dyn->momentum == Vec3::ZERO );

      dyn->flags   |= Object::DISABLED_BIT;
      dyn->velocity = Vec3::ZERO;
    }
  }
}

}
}
