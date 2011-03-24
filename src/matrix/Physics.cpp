/*
 *  Physics.cpp
 *
 *  Physics engine
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Physics.hpp"

#include "matrix/Collider.hpp"

namespace oz
{

  Physics physics;

  const float Physics::MOVE_BOUNCE            = 1.5f * EPSILON;
  const float Physics::HIT_THRESHOLD          = -3.0f;
  const float Physics::SPLASH_THRESHOLD       = -2.0f;
  const float Physics::FLOOR_NORMAL_Z         = 0.60f;
  const float Physics::G_MOMENTUM             = -9.81f * Timer::TICK_TIME;
  const float Physics::WEIGHT_FACTOR          = 0.1f;

  const float Physics::STICK_VELOCITY         = 0.015f;
  const float Physics::SLICK_STICK_VELOCITY   = 0.001f;
  const float Physics::AIR_STICK_VELOCITY     = 0.001f;
  const float Physics::AIR_FRICTION           = 0.02f;
  const float Physics::WATER_FRICTION         = 0.08f;
  const float Physics::LADDER_FRICTION        = 0.65f;
  const float Physics::FLOOR_FRICTION         = 0.40f;
  const float Physics::SLICK_FRICTION         = 0.02f;

  const float Physics::PART_HIT_VELOCITY2     = 100.0f;
  const float Physics::PART_DESTROY_VELOCITY2 = 900.0f;

  //***********************************
  //*   PARTICLE COLLISION HANDLING   *
  //***********************************

  void Physics::handlePartHit()
  {
    float velocity2 = part->velocity * part->velocity;
    if( velocity2 >= PART_HIT_VELOCITY2 ) {
      if( velocity2 >= PART_DESTROY_VELOCITY2 ) {
        part->lifeTime = 0.0f;
      }
      if( collider.hit.obj != null && part->mass != 0.0f ) {
        Object* sObj = const_cast<Object*>( collider.hit.obj );
        float damage = velocity2 * part->mass;

        if( damage > sObj->clazz->damageThreshold ) {
          damage -= sObj->clazz->damageThreshold;
          damage *= Math::frand();
          sObj->damage( sObj->clazz->damageThreshold + damage );
        }
      }
    }

    float hitMomentum = part->velocity * collider.hit.normal;
    part->velocity -= ( part->restitution * hitMomentum ) * collider.hit.normal;
  }

  void Physics::handlePartMove()
  {
    leftRatio = 1.0f;
    move = part->velocity * Timer::TICK_TIME;

    int traceSplits = 0;
    do {
      collider.translate( part->p, move );
      part->p += collider.hit.ratio * move;
      leftRatio -= leftRatio * collider.hit.ratio;

      if( collider.hit.ratio == 1.0f ) {
        break;
      }
      // collision response
      handlePartHit();

      // we must check lifeTime <= 0.0f to prevent an already destroyed particle to bounce off a
      // surface and hit something (e.g. if we shoot into something with a rifle, a bullet is not
      // destroyed immediately after it hits something, but bounces off and damages the shooter if
      // he stays to close to the hit surface
      if( traceSplits >= 3 || part->lifeTime <= 0.0f ) {
        break;
      }
      ++traceSplits;

      move *= 1.0f - collider.hit.ratio;
      move -= ( move * collider.hit.normal - MOVE_BOUNCE ) * collider.hit.normal;
    }
    while( true );

    orbis.reposition( part );
  }

  //***********************************
  //*    OBJECT COLLISION HANDLING    *
  //***********************************

  bool Physics::handleObjFriction()
  {
    float systemMom = G_MOMENTUM;

    if( dyn->flags & ( Object::HOVER_BIT | Object::ON_LADDER_BIT ) ) {
      // in air
      if( dyn->flags & Object::HOVER_BIT ) {
        if( dyn->momentum.sqL() <= AIR_STICK_VELOCITY ) {
          dyn->momentum = Vec3::ZERO;
        }
        else {
          dyn->momentum *= 1.0f - AIR_FRICTION;
        }
      }
      // swimming
      else if( dyn->flags & Object::IN_WATER_BIT ) {
        // lift
        systemMom += ( 0.5f * dyn->depth / dyn->dim.z ) * dyn->lift * Timer::TICK_TIME;

        dyn->momentum *= 1.0f - WATER_FRICTION;
        dyn->momentum.z += systemMom;
      }
      // on ladder
      else if( dyn->flags & Object::ON_LADDER_BIT ) {
        if( dyn->momentum.sqL() <= STICK_VELOCITY ) {
          dyn->momentum = Vec3::ZERO;
        }
        else {
          dyn->momentum *= 1.0f - LADDER_FRICTION;
        }
      }
    }
    else {
      if( dyn->flags & Object::IN_WATER_BIT ) {
        float frictionFactor = 0.5f * dyn->depth / dyn->dim.z;

        dyn->momentum *= 1.0f - frictionFactor * WATER_FRICTION;
        systemMom += frictionFactor * dyn->lift * Timer::TICK_TIME;
      }

      Dynamic* sObj = dyn->lower == -1 ? null : static_cast<Dynamic*>( orbis.objects[dyn->lower] );

      // on floor or still object
      if( ( dyn->flags & Object::ON_FLOOR_BIT ) ||
          ( sObj != null && ( sObj->flags & Object::DISABLED_BIT ) ) )
      {
        float stickVel = STICK_VELOCITY;
        float friction = FLOOR_FRICTION;

        if( dyn->flags & Object::ON_SLICK_BIT ) {
          stickVel = SLICK_STICK_VELOCITY;
          friction = SLICK_FRICTION;
        }

        dyn->momentum += ( systemMom * dyn->floor.z ) * dyn->floor;
        dyn->momentum *= 1.0f - friction;

        if( dyn->velocity.x*dyn->velocity.x + dyn->velocity.y*dyn->velocity.y > stickVel ) {
          dyn->flags |= Object::FRICTING_BIT;
        }

        if( dyn->momentum.x*dyn->momentum.x + dyn->momentum.y*dyn->momentum.y <= stickVel ) {
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
      // on a moving object
      else if( sObj != null ) {
        dyn->momentum.x += ( sObj->velocity.x - dyn->velocity.x ) * FLOOR_FRICTION;
        dyn->momentum.y += ( sObj->velocity.y - dyn->velocity.y ) * FLOOR_FRICTION;
        dyn->momentum.z += systemMom;

        dyn->flags |= Object::FRICTING_BIT;
      }
      else {
        dyn->momentum.x *= 1.0f - AIR_FRICTION;
        dyn->momentum.y *= 1.0f - AIR_FRICTION;
        dyn->momentum.z += systemMom;
      }
    }

    dyn->flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT |
        Object::IN_WATER_BIT | Object::ON_LADDER_BIT | Object::ON_SLICK_BIT );
    dyn->lower = -1;

    return true;
  }

  void Physics::handleObjHit()
  {
    const Hit& hit = collider.hit;

    if( hit.obj != null && ( hit.obj->flags & Object::DYNAMIC_BIT ) ) {
      Dynamic* sDyn = static_cast<Dynamic*>( const_cast<Object*>( hit.obj ) );

      Vec3  momentum    = ( dyn->momentum * dyn->mass + sDyn->momentum * sDyn->mass ) /
          ( dyn->mass + sDyn->mass );
      float hitMomentum = ( dyn->momentum - sDyn->momentum ) * hit.normal;
      float hitVelocity = dyn->velocity * hit.normal;

      if( hitMomentum <= HIT_THRESHOLD && hitVelocity <= HIT_THRESHOLD ) {
        dyn->hit( &hit, hitMomentum );
        sDyn->hit( &hit, hitMomentum );
      }

      if( hit.normal.z == 0.0f ) {
        sDyn->flags &= ~Object::DISABLED_BIT;

        float dynMomProj  =  dyn->momentum.x * hit.normal.x +  dyn->momentum.y * hit.normal.y;
        float sDynMomProj = sDyn->momentum.x * hit.normal.x + sDyn->momentum.y * hit.normal.y;
        float sDynVelProj = sDyn->velocity.x * hit.normal.x + sDyn->velocity.y * hit.normal.y;

        if( dyn->flags & Object::PUSHER_BIT ) {
          sDyn->momentum.x = momentum.x;
          sDyn->momentum.y = momentum.y;
        }
        else {
          sDyn->momentum.x += ( dynMomProj - sDynMomProj ) * hit.normal.x;
          sDyn->momentum.y += ( dynMomProj - sDynMomProj ) * hit.normal.y;
        }

        dyn->momentum.x  -= ( dynMomProj - sDynVelProj ) * hit.normal.x;
        dyn->momentum.y  -= ( dynMomProj - sDynVelProj ) * hit.normal.y;
      }
      else if( hit.normal.z == -1.0f ) {
        sDyn->flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT );
        sDyn->lower = dyn->index;
        sDyn->floor = Vec3( 0.0f, 0.0f, 1.0f );

        dyn->flags  |= Object::UPPER_BIT;

        sDyn->momentum.z = momentum.z;
        dyn->momentum.z  = sDyn->velocity.z;
      }
      else { // hit.normal.z == 1.0f
        hard_assert( hit.normal.z == 1.0f );

        sDyn->damage( dyn->mass * WEIGHT_FACTOR );

        dyn->flags  &= ~Object::ON_FLOOR_BIT;
        dyn->lower  = sDyn->index;
        dyn->floor  = Vec3( 0.0f, 0.0f, 1.0f );

        if( !( sDyn->flags & Object::DISABLED_BIT ) ) {
          sDyn->momentum.z = momentum.z;
          dyn->momentum.z  = sDyn->velocity.z;
        }
        else {
          dyn->momentum.z = 0.0f;
        }
      }
    }
    else {
      float hitMomentum = dyn->momentum * hit.normal;
      float hitVelocity = dyn->velocity * hit.normal;

      if( hitMomentum <= HIT_THRESHOLD && hitVelocity <= HIT_THRESHOLD ) {
        dyn->hit( &hit, hitMomentum );

        if( hit.obj != null ) {
          Object* sObj = const_cast<Object*>( hit.obj );

          sObj->hit( &hit, hitMomentum );
        }
      }

      dyn->momentum -= ( dyn->momentum * hit.normal ) * hit.normal;

      if( hit.normal.z >= FLOOR_NORMAL_Z ) {
        dyn->flags |= Object::ON_FLOOR_BIT;
        dyn->flags |= ( hit.material & Material::SLICK_BIT ) ? Object::ON_SLICK_BIT : 0;
        dyn->lower = -1;
        dyn->floor = hit.normal;
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

      if( traceSplits >= 3 ) {
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

    dyn->flags |= collider.hit.inWater  ? Object::IN_WATER_BIT  : 0;
    dyn->flags |= collider.hit.onLadder ? Object::ON_LADDER_BIT : 0;
    dyn->depth = min( collider.hit.waterDepth, 2.0f * dyn->dim.z );

    hard_assert( ( dyn->depth != 0.0f ) == collider.hit.inWater );

    if( ( dyn->flags & ~dyn->oldFlags & Object::IN_WATER_BIT ) &&
        dyn->velocity.z <= SPLASH_THRESHOLD )
    {
      dyn->splash( dyn->velocity.z );
    }

    orbis.reposition( dyn );
  }

  //***********************************
  //*             PUBLIC              *
  //***********************************

  void Physics::updatePart( Particle* part_ )
  {
    part = part_;

    hard_assert( part->cell != null );

    part->velocity.z += G_MOMENTUM;
    part->lifeTime -= Timer::TICK_TIME;

    part->rot += part->rotVelocity * Timer::TICK_TIME;
    handlePartMove();
  }

  void Physics::updateObj( Dynamic* dyn_ )
  {
    dyn = dyn_;

    hard_assert( dyn->cell != null );
    hard_assert( !( dyn->flags & Object::ON_FLOOR_BIT ) || ( dyn->lower == -1 ) );

    dyn->flags &= ~( Object::HIT_BIT | Object::FRICTING_BIT | Object::UPPER_BIT );

    if( dyn->lower != -1 ) {
      Object* sObj = orbis.objects[dyn->lower];

      // clear the lower object if it doesn't exist any more
      if( sObj == null || sObj->cell == null ) {
        dyn->flags &= ~Object::DISABLED_BIT;
        dyn->lower = -1;
      }
      else if( !( sObj->flags & Object::DISABLED_BIT ) ) {
        dyn->flags &= ~Object::DISABLED_BIT;
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
