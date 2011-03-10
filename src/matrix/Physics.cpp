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

  const float Physics::CLIP_BACKOFF           = EPSILON;
  const float Physics::HIT_THRESHOLD          = -2.0f;
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
      move -= ( move * collider.hit.normal - CLIP_BACKOFF ) * collider.hit.normal;
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

    if( obj->flags & ( Object::HOVER_BIT | Object::ON_LADDER_BIT ) ) {
      // in air
      if( obj->flags & Object::HOVER_BIT ) {
        if( obj->momentum.sqL() <= AIR_STICK_VELOCITY ) {
          obj->momentum = Vec3::ZERO;
        }
        else {
          obj->momentum *= 1.0f - AIR_FRICTION;
        }
      }
      // swimming
      else if( obj->flags & Object::IN_WATER_BIT ) {
        // lift
        systemMom += ( 0.5f * obj->depth / obj->dim.z ) * obj->lift * Timer::TICK_TIME;

        obj->momentum *= 1.0f - WATER_FRICTION;
        obj->momentum.z += systemMom;
      }
      // on ladder
      else if( obj->flags & Object::ON_LADDER_BIT ) {
        if( obj->momentum.sqL() <= STICK_VELOCITY ) {
          obj->momentum = Vec3::ZERO;
        }
        else {
          obj->momentum *= 1.0f - LADDER_FRICTION;
        }
      }
    }
    else {
      if( obj->flags & Object::IN_WATER_BIT ) {
        float frictionFactor = 0.5f * obj->depth / obj->dim.z;

        obj->momentum *= 1.0f - frictionFactor * WATER_FRICTION;
        systemMom += frictionFactor * obj->lift * Timer::TICK_TIME;
      }

      Dynamic* sObj = obj->lower == -1 ? null : static_cast<Dynamic*>( orbis.objects[obj->lower] );

      // on floor
      if( ( obj->flags & Object::ON_FLOOR_BIT ) ||
          ( sObj != null && ( sObj->flags & Object::DISABLED_BIT ) ) )
      {
        float stickVel = STICK_VELOCITY;
        float friction = FLOOR_FRICTION;

        if( obj->flags & Object::ON_SLICK_BIT ) {
          stickVel = SLICK_STICK_VELOCITY;
          friction = SLICK_FRICTION;
        }

        float dx = obj->momentum.x;
        float dy = obj->momentum.y;
        float dv2 = dx*dx + dy*dy;

        if( dv2 > stickVel ) {
          obj->momentum.x *= 1.0f - friction;
          obj->momentum.y *= 1.0f - friction;

          obj->momentum += ( systemMom * obj->floor.z ) * obj->floor;

          obj->flags |= Object::FRICTING_BIT;
        }
        else {
          obj->momentum.x = 0.0f;
          obj->momentum.y = 0.0f;
          obj->momentum.z += systemMom;

          if( obj->momentum.z <= 0.0f ) {
            obj->momentum.z = 0.0f;
            return false;
          }
        }
      }
      // on a moving object
      else if( sObj != null ) {
        float dx  = sObj->velocity.x - obj->momentum.x;
        float dy  = sObj->velocity.y - obj->momentum.y;
        float dv2 = dx*dx + dy*dy;

        obj->momentum.x += dx * FLOOR_FRICTION;
        obj->momentum.y += dy * FLOOR_FRICTION;
        obj->momentum.z += systemMom;

        if( dv2 > STICK_VELOCITY ) {
          obj->flags |= Object::FRICTING_BIT;
        }
      }
      else {
        obj->momentum.x *= 1.0f - AIR_FRICTION;
        obj->momentum.y *= 1.0f - AIR_FRICTION;
        obj->momentum.z += systemMom;
      }
    }

    obj->flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT |
        Object::IN_WATER_BIT | Object::ON_LADDER_BIT | Object::ON_SLICK_BIT );
    obj->lower = -1;

    return true;
  }

  void Physics::handleObjHit()
  {
    const Hit& hit = collider.hit;

    if( hit.obj != null && ( hit.obj->flags & Object::DYNAMIC_BIT ) ) {
      Dynamic* sDyn = static_cast<Dynamic*>( const_cast<Object*>( hit.obj ) );

      Vec3  momentum    = ( obj->momentum * obj->mass + sDyn->momentum * sDyn->mass ) /
          ( obj->mass + sDyn->mass );
      float hitMomentum = ( obj->momentum - sDyn->momentum ) * hit.normal;
      float hitVelocity = obj->velocity * hit.normal;

      if( hitMomentum <= HIT_THRESHOLD && hitVelocity <= HIT_THRESHOLD ) {
        obj->hit( &hit, hitMomentum );
        sDyn->hit( &hit, hitMomentum );
      }

      if( hit.normal.z == 0.0f ) {
        sDyn->flags &= ~Object::DISABLED_BIT;

        if( obj->flags & Object::PUSHER_BIT ) {
          sDyn->momentum.x = momentum.x;
          sDyn->momentum.y = momentum.y;

          if( hit.normal.y == 0.0f ) {
            obj->momentum.x = sDyn->velocity.x;
          }
          else {
            obj->momentum.y = sDyn->velocity.y;
          }
        }
        else if( hit.normal.y == 0.0f ) {
          sDyn->momentum.x = momentum.x;
          obj->momentum.x  = sDyn->velocity.x;
        }
        else {
          sDyn->momentum.y = momentum.y;
          obj->momentum.y  = sDyn->velocity.y;
        }
      }
      else if( hit.normal.z == -1.0f ) {
        sDyn->flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT );
        sDyn->lower = obj->index;
        sDyn->floor = Vec3( 0.0f, 0.0f, 1.0f );

        obj->flags  |= Object::UPPER_BIT;

        sDyn->momentum.z = momentum.z;
        obj->momentum.z  = sDyn->velocity.z;
      }
      else { // hit.normal.z == 1.0f
        hard_assert( hit.normal.z == 1.0f );

        sDyn->damage( obj->mass * WEIGHT_FACTOR );

        obj->flags  &= ~Object::ON_FLOOR_BIT;
        obj->lower  = sDyn->index;
        obj->floor  = Vec3( 0.0f, 0.0f, 1.0f );

        if( !( sDyn->flags & Object::DISABLED_BIT ) ) {
          sDyn->momentum.z = momentum.z;
          obj->momentum.z  = sDyn->velocity.z;
        }
        else {
          obj->momentum.z = 0.0f;
        }
      }
    }
    else {
      float hitMomentum = obj->momentum * hit.normal;
      float hitVelocity = obj->velocity * hit.normal;

      if( hitMomentum <= HIT_THRESHOLD && hitVelocity <= HIT_THRESHOLD ) {
        obj->hit( &hit, hitMomentum );

        if( hit.obj != null ) {
          Object* sObj = const_cast<Object*>( hit.obj );

          sObj->hit( &hit, hitMomentum );
        }
      }

      obj->momentum -= ( obj->momentum * hit.normal ) * hit.normal;

      if( hit.normal.z >= FLOOR_NORMAL_Z ) {
        obj->flags |= Object::ON_FLOOR_BIT;
        obj->flags |= ( hit.material & Material::SLICK_BIT ) ? Object::ON_SLICK_BIT : 0;
        obj->lower = -1;
        obj->floor = hit.normal;
      }
    }
  }

  void Physics::handleObjMove()
  {
    move = obj->momentum * Timer::TICK_TIME;
    leftRatio = 1.0f;

    int traceSplits = 0;
    do {
      collider.translate( obj, move );
      obj->p += collider.hit.ratio * move;
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
      move -= ( move * collider.hit.normal - CLIP_BACKOFF ) * collider.hit.normal;

      // to prevent getting stuck in corners < 90° and to prevent oscillations in corners > 90°
      if( traceSplits == 1 ) {
        lastNormals[0] = collider.hit.normal;
      }
      else {
        float dot = lastNormals[0] * collider.hit.normal;

        if( dot < 0.0f ) {
          Vec3 out   = collider.hit.normal + lastNormals[0];
          Vec3 cross = collider.hit.normal ^ lastNormals[0];

          if( cross != Vec3::ZERO ) {
            cross = ~cross;
            move = ( move * cross ) * cross;
            move += out * EPSILON;
          }
          if( out != Vec3::ZERO ) {
            out = ~out;
            obj->momentum -= ( obj->momentum * out ) * out;
          }
        }
        if( traceSplits == 2 ) {
          lastNormals[1] = lastNormals[0];
          lastNormals[0] = collider.hit.normal;
        }
        else {
          dot = lastNormals[1] * collider.hit.normal;

          if( dot < 0.0f ) {
            Vec3 out   = collider.hit.normal + lastNormals[1];
            Vec3 cross = collider.hit.normal ^ lastNormals[1];

            if( cross != Vec3::ZERO ) {
              cross = ~cross;
              move = ( move * cross ) * cross;
              move += out * EPSILON;
            }
            if( out != Vec3::ZERO ) {
              out = ~out;
              obj->momentum -= ( obj->momentum * out ) * out;
            }
          }
        }
      }
    }
    while( true );

    obj->flags |= collider.hit.inWater  ? Object::IN_WATER_BIT  : 0;
    obj->flags |= collider.hit.onLadder ? Object::ON_LADDER_BIT : 0;
    obj->depth = min( collider.hit.waterDepth, 2.0f * obj->dim.z );

    hard_assert( ( obj->depth != 0.0f ) == collider.hit.inWater );

    if( ( obj->flags & ~obj->oldFlags & Object::IN_WATER_BIT ) &&
        obj->velocity.z <= SPLASH_THRESHOLD )
    {
      obj->splash( obj->velocity.z );
    }

    orbis.reposition( obj );
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

  void Physics::updateObj( Dynamic* obj_ )
  {
    obj = obj_;

    hard_assert( obj->cell != null );
    hard_assert( !( obj->flags & Object::ON_FLOOR_BIT ) || ( obj->lower == -1 ) );

    obj->flags &= ~( Object::HIT_BIT | Object::FRICTING_BIT | Object::UPPER_BIT );

    if( obj->lower != -1 ) {
      Object* sObj = orbis.objects[obj->lower];

      // clear the lower object if it doesn't exist any more
      if( sObj == null || sObj->cell == null ) {
        obj->flags &= ~Object::DISABLED_BIT;
        obj->lower = -1;
      }
      else if( !( sObj->flags & Object::DISABLED_BIT ) ) {
        obj->flags &= ~Object::DISABLED_BIT;
      }
    }
    // handle physics
    if( !( obj->flags & Object::DISABLED_BIT ) ) {
      if( handleObjFriction() ) {
        // if objects is still in movement or not on a still surface after friction changed its
        // velocity, handle physics
        Point3 oldPos = obj->p;

        collider.mask = obj->flags & Object::SOLID_BIT;
        handleObjMove();
        collider.mask = Object::SOLID_BIT;

        obj->velocity = ( obj->p - oldPos ) / Timer::TICK_TIME;
      }
      else {
        hard_assert( obj->momentum == Vec3::ZERO );

        obj->flags |= Object::DISABLED_BIT;
        obj->velocity = Vec3::ZERO;
      }
    }
  }

}
