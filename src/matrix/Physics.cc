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

const float Physics::MOVE_BOUNCE             =  1.5f * EPSILON;
const float Physics::HIT_THRESHOLD           = -3.0f;
const float Physics::SPLASH_THRESHOLD        = -2.0f;
const float Physics::FLOOR_NORMAL_Z          =  0.60f;
const float Physics::SIDE_PUSH_RATIO         =  0.5f;
const float Physics::WEIGHT_DAMAGE_THRESHOLD =  1000.0f;
const float Physics::WEIGHT_DAMAGE_FACTOR    =  20.0f;
const float Physics::G_ACCEL                 = -9.81f;

const float Physics::SLIDE_DAMAGE_THRESHOLD  =  25.0f;
const float Physics::SLIDE_DAMAGE_COEF       =  0.65f;
const float Physics::STICK_VELOCITY          =  0.02f;
const float Physics::SLICK_STICK_VELOCITY    =  0.001f;
const float Physics::FLOAT_STICK_VELOCITY    =  0.0002f;
const float Physics::WATER_FRICTION          =  0.08f;
const float Physics::LADDER_FRICTION         =  0.50f;
const float Physics::FLOOR_FRICTION          =  0.25f;
const float Physics::SLICK_FRICTION          =  0.02f;

const float Physics::FRAG_HIT_VELOCITY2      =  100.0f;
const float Physics::FRAG_DESTROY_VELOCITY2  =  300.0f;
const float Physics::FRAG_STR_DAMAGE_COEF    =  0.05f;
const float Physics::FRAG_OBJ_DAMAGE_COEF    =  0.05f;
const float Physics::FRAG_FIXED_DAMAGE       =  0.75f;

//***********************************
//*   FRAGMENT COLLISION HANDLING   *
//***********************************

void Physics::handleFragHit()
{
  float velocity2 = frag->velocity.sqL();

  frag->velocity *= frag->restitution;
  frag->velocity -= ( 2.0f * frag->velocity * collider.hit.normal ) * collider.hit.normal;

  if( velocity2 > FRAG_HIT_VELOCITY2 ) {
    if( velocity2 > FRAG_DESTROY_VELOCITY2 ) {
      // we abuse velocity to hold the normal of the fatal hit, needed for positioning decals
      frag->velocity = collider.hit.normal;
      frag->life = -Math::INF;
    }

    if( frag->mass != 0.0f ) {
      if( collider.hit.str != null ) {
        Struct* str = collider.hit.str;
        float damage = FRAG_STR_DAMAGE_COEF * velocity2 * frag->mass;

        if( damage > str->resistance ) {
          damage *= FRAG_FIXED_DAMAGE + ( 1.0f - FRAG_FIXED_DAMAGE ) * Math::rand();
          str->damage( damage );
        }
      }
      else if( collider.hit.obj != null ) {
        Object* obj = collider.hit.obj;
        float damage = FRAG_OBJ_DAMAGE_COEF * velocity2 * frag->mass;

        if( damage > obj->resistance ) {
          damage *= FRAG_FIXED_DAMAGE + ( 1.0f - FRAG_FIXED_DAMAGE ) * Math::rand();
          obj->damage( damage );
        }
      }
    }
  }
}

void Physics::handleFragMove()
{
  leftRatio = 1.0f;
  move = frag->velocity * Timer::TICK_TIME;

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
  float systemMom = G_ACCEL * Timer::TICK_TIME;

  if( dyn->flags & Object::ON_LADDER_BIT ) {
    if( dyn->momentum.sqL() <= STICK_VELOCITY ) {
      dyn->momentum = Vec3::ZERO;
    }
    else {
      dyn->momentum *= 1.0f - LADDER_FRICTION;
    }
  }
  else {
    if( dyn->flags & Object::IN_WATER_BIT ) {
      float frictionFactor = 0.5f * dyn->depth / dyn->dim.z;

      dyn->momentum *= 1.0f - frictionFactor * WATER_FRICTION;
      systemMom += frictionFactor * dyn->lift * Timer::TICK_TIME;
    }

    float deltaVelX = dyn->momentum.x;
    float deltaVelY = dyn->momentum.y;

    if( dyn->lower != -1 ) {
      if( dyn->flags & Object::ON_FLOOR_BIT ) {
        int structIndex = dyn->lower / Struct::MAX_ENTITIES;
        int entityIndex = dyn->lower % Struct::MAX_ENTITIES;

        const Entity& entity = orbis.structs[structIndex]->entities[entityIndex];

        deltaVelX -= entity.velocity.x;
        deltaVelY -= entity.velocity.y;
      }
      else {
        const Dynamic* sDyn = static_cast<const Dynamic*>( orbis.objects[dyn->lower] );

        deltaVelX -= sDyn->velocity.x;
        deltaVelY -= sDyn->velocity.y;
      }
    }

    // on floor or on a still object
    if( ( dyn->flags & Object::ON_FLOOR_BIT ) || dyn->lower != -1  ) {
      float deltaVel2 = deltaVelX*deltaVelX + deltaVelY*deltaVelY;
      float friction  = FLOOR_FRICTION;
      float stickVel  = STICK_VELOCITY;

      if( dyn->flags & Object::ON_SLICK_BIT ) {
        deltaVel2 = 0.0f;
        friction  = SLICK_FRICTION;
        stickVel  = SLICK_STICK_VELOCITY;
      }

      dyn->momentum += ( systemMom * dyn->floor.z ) * dyn->floor;
      dyn->momentum.x -= deltaVelX * friction;
      dyn->momentum.y -= deltaVelY * friction;
      dyn->momentum.z *= 1.0f - friction;

      if( deltaVel2 > stickVel ) {
        dyn->flags |= Object::FRICTING_BIT;

        if( deltaVel2 > SLIDE_DAMAGE_THRESHOLD ) {
          dyn->damage( SLIDE_DAMAGE_COEF * deltaVel2 );
        }
      }

      if( dyn->lower == -1 &&
          dyn->momentum.x*dyn->momentum.x + dyn->momentum.y*dyn->momentum.y <= stickVel )
      {
        dyn->momentum.x = 0.0f;
        dyn->momentum.y = 0.0f;

        if( dyn->momentum.z <= 0.0f ) {
          dyn->momentum.z = 0.0f;

          if( systemMom <= 0.0f ) {
            return false;
          }
        }
      }
    }
    // in air or swimming
    else {
      dyn->momentum.z += systemMom;

      if( Math::fabs( systemMom ) <= FLOAT_STICK_VELOCITY &&
          dyn->momentum.sqL() <= FLOAT_STICK_VELOCITY )
      {
        dyn->momentum = Vec3::ZERO;
        return false;
      }
    }
  }

  dyn->flags &= ~( Object::ON_FLOOR_BIT | Object::ON_SLICK_BIT );
  dyn->lower = -1;

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
      dyn->hit( &hit, hitMomentum );
      sDyn->hit( &hit, hitMomentum );
    }

    if( hit.normal.z == 0.0f ) {
      float momProj     =       momentum.x * hit.normal.x +       momentum.y * hit.normal.y;
      float dynMomProj  =  dyn->momentum.x * hit.normal.x +  dyn->momentum.y * hit.normal.y;
      float sDynMomProj = sDyn->momentum.x * hit.normal.x + sDyn->momentum.y * hit.normal.y;
      float sDynVelProj = sDyn->velocity.x * hit.normal.x + sDyn->velocity.y * hit.normal.y;

      dyn->momentum.x -= ( dynMomProj - sDynVelProj ) * hit.normal.x;
      dyn->momentum.y -= ( dynMomProj - sDynVelProj ) * hit.normal.y;

      sDyn->flags &= ~Object::DISABLED_BIT;

      if( dyn->flags & Object::PUSHER_BIT ) {
        float pushX       = momentum.x - sDyn->momentum.x;
        float pushY       = momentum.y - sDyn->momentum.y;
        float directPushX = ( momProj - sDynMomProj ) * hit.normal.x;
        float directPushY = ( momProj - sDynMomProj ) * hit.normal.y;

        sDyn->momentum.x += directPushX + SIDE_PUSH_RATIO * ( pushX - directPushX );
        sDyn->momentum.y += directPushY + SIDE_PUSH_RATIO * ( pushY - directPushY );

        // allow side-pushing downwards in water
        if( ( dyn->flags & sDyn->flags & Object::IN_WATER_BIT ) && momentum.z < 0.0f ) {
          sDyn->momentum.z += SIDE_PUSH_RATIO * ( momentum.z - sDyn->momentum.z );
        }
      }
      else {
        sDyn->momentum.x += ( momProj - sDynMomProj ) * hit.normal.x;
        sDyn->momentum.y += ( momProj - sDynMomProj ) * hit.normal.y;
      }
    }
    else if( hit.normal.z == -1.0f ) {
      dyn->flags |= Object::BELOW_BIT;
      dyn->momentum.z = sDyn->velocity.z;

      sDyn->flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT );
      sDyn->lower = dyn->index;
      sDyn->floor = Vec3( 0.0f, 0.0f, 1.0f );
      sDyn->momentum.z = momentum.z;
    }
    else { // hit.normal.z == 1.0f
      hard_assert( hit.normal.z == 1.0f );

      dyn->flags &= ~Object::ON_FLOOR_BIT;
      dyn->lower = sDyn->index;
      dyn->floor = Vec3( 0.0f, 0.0f, 1.0f );
      dyn->momentum.z = sDyn->velocity.z;

      sDyn->flags |= Object::BELOW_BIT;
      if( dyn->mass > WEIGHT_DAMAGE_THRESHOLD ) {
        sDyn->damage( dyn->mass / sDyn->mass * WEIGHT_DAMAGE_FACTOR );
      }

      if( !( sDyn->flags & Object::ON_FLOOR_BIT ) && sDyn->lower == -1 ) {
        sDyn->flags &= ~Object::DISABLED_BIT;
        sDyn->momentum.z = momentum.z;
      }
    }
  }
  else {
    float hitMomentum = dyn->momentum * hit.normal;
    float hitVelocity = dyn->velocity * hit.normal;

    if( hitMomentum < HIT_THRESHOLD && hitVelocity < HIT_THRESHOLD ) {
      if( hit.obj != null ) {
        Object* sObj = hit.obj;

        dyn->hit( &hit, hitMomentum );
        sObj->hit( &hit, hitMomentum );
      }
      else {
        dyn->hit( &hit, hitMomentum );

        if( hit.str != null ) {
          hit.str->hit( dyn->mass, hitMomentum );
        }
      }
    }

    dyn->momentum -= ( dyn->momentum * hit.normal ) * hit.normal;

    if( hit.normal.z >= FLOOR_NORMAL_Z ) {
      dyn->flags |= Object::ON_FLOOR_BIT;
      dyn->flags |= hit.material & Material::SLICK_BIT ? Object::ON_SLICK_BIT : 0;
      dyn->floor = hit.normal;

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
  leftRatio = 1.0f;

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

    // to prevent getting stuck in corners < 90° and to prevent oscillations in corners > 90°
    if( traceSplits == 1 ) {
      lastNormals[0] = collider.hit.normal;
    }
    else {
      float dot = lastNormals[0] * collider.hit.normal;

      if( dot < 0.0f ) {
        Vec3  cross    = collider.hit.normal ^ lastNormals[0];
        float crossSqL = cross.sqL();

        if( crossSqL != 0.0f ) {
          cross /= Math::sqrt( crossSqL );
          move = ( move * cross ) * cross;
          move += MOVE_BOUNCE * ( collider.hit.normal + lastNormals[0] );
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
          float crossSqL = cross.sqL();

          if( crossSqL != 0.0f ) {
            cross /= Math::sqrt( crossSqL );
            move = ( move * cross ) * cross;
            move += MOVE_BOUNCE * ( collider.hit.normal + lastNormals[1] );
          }
        }
      }
    }
  }
  while( true );

  int newFlags = ( collider.hit.medium & Material::WATER_BIT ? Object::IN_WATER_BIT : 0 ) |
                 ( collider.hit.medium & Material::LADDER_BIT ? Object::ON_LADDER_BIT : 0 );

  if( ( newFlags & ~dyn->flags & Object::IN_WATER_BIT ) && dyn->velocity.z <= SPLASH_THRESHOLD ) {
    dyn->splash( dyn->velocity.z );
  }

  dyn->flags &= ~( Object::IN_WATER_BIT | Object::ON_LADDER_BIT );
  dyn->flags |= newFlags;
  dyn->depth = min( collider.hit.waterDepth, 2.0f * dyn->dim.z );

  orbis.reposition( dyn );
}

//***********************************
//*             PUBLIC              *
//***********************************

void Physics::updateFrag( Frag* frag_ )
{
  frag = frag_;

  hard_assert( frag->cell != null );

  frag->velocity.z += G_ACCEL * Timer::TICK_TIME;

  handleFragMove();
}

void Physics::updateObj( Dynamic* dyn_ )
{
  dyn = dyn_;

  hard_assert( dyn->cell != null );

  dyn->flags &= ~Object::TICK_CLEAR_MASK;

  if( dyn->lower != -1 ) {
    if( dyn->flags & Object::ON_FLOOR_BIT ) {
      int structIndex = dyn->lower / Struct::MAX_ENTITIES;
      int entityIndex = dyn->lower % Struct::MAX_ENTITIES;

      const Struct* str = orbis.structs[structIndex];

      if( str == null ) {
        dyn->flags &= ~Object::DISABLED_BIT;
        dyn->lower = -1;
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
        dyn->lower = -1;
      }
      else if( !( sObj->flags & Object::DISABLED_BIT ) ) {
        dyn->flags &= ~Object::DISABLED_BIT;
      }
    }
  }
  // handle physics
  if( !( dyn->flags & Object::DISABLED_BIT ) ) {
    if( handleObjFriction() ) {
      // if objects is still in movement or not on a still surface after friction changed its
      // velocity, handle physics
      Point3 oldPos = dyn->p;

      collider.mask = dyn->flags & Object::SOLID_BIT;
      handleObjMove();
      collider.mask = Object::SOLID_BIT;

      dyn->velocity = ( dyn->p - oldPos ) / Timer::TICK_TIME;

      Vec3 absVelocity = dyn->velocity.abs();
      dyn->momentum.x = clamp( dyn->momentum.x, -absVelocity.x, +absVelocity.x );
      dyn->momentum.y = clamp( dyn->momentum.y, -absVelocity.y, +absVelocity.y );
      dyn->momentum.z = clamp( dyn->momentum.z, -absVelocity.z, +absVelocity.z );
    }
    else {
      hard_assert( dyn->momentum == Vec3::ZERO );

      dyn->flags |= Object::DISABLED_BIT;
      dyn->velocity = Vec3::ZERO;
    }
  }
}

}
}
