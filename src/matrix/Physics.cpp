/*
 *  Physics.cpp
 *
 *  Physics engine
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "Physics.h"

#include "Collider.h"

namespace oz
{

  Physics physics;

  const float Physics::CLIP_BACKOFF         = EPSILON;
  const float Physics::HIT_TRESHOLD         = -2.0f;
  const float Physics::SPLASH_TRESHOLD      = -2.0f;
  const float Physics::FLOOR_NORMAL_Z       = 0.60f;
  const float Physics::G_VELOCITY           = -9.81f * Timer::TICK_TIME;
  const float Physics::WEIGHT_FACTOR        = 0.1f;

  const float Physics::STICK_VELOCITY       = 0.015f;
  const float Physics::SLICK_STICK_VELOCITY = 0.0001f;
  const float Physics::AIR_STICK_VELOCITY   = 0.0001f;
  const float Physics::AIR_FRICTION         = 0.02f;
  const float Physics::IN_WATER_FRICTION    = 0.08f;
  const float Physics::ON_WATER_FRICTION    = 0.30f;
  const float Physics::LADDER_FRICTION      = 0.65f;
  const float Physics::FLOOR_FRICTION       = 0.40f;
  const float Physics::OBJ_FRICTION         = 0.40f;
  const float Physics::SLICK_FRICTION       = 0.02f;

  //***********************************
  //*   PARTICLE COLLISION HANDLING   *
  //***********************************

  void Physics::handlePartHit()
  {
    float hitMomentum = part->velocity * collider.hit.normal;
    part->velocity -= ( part->rejection * hitMomentum ) * collider.hit.normal;

    if( part->mass != 0.0f && collider.hit.obj != null ) {
      collider.hit.obj->hit( &collider.hit, hitMomentum * part->mass );
    }
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

      if( traceSplits >= 3 ) {
        break;
      }
      traceSplits++;

      move *= 1.0f - collider.hit.ratio;
      move -= ( move * collider.hit.normal - CLIP_BACKOFF ) * collider.hit.normal;
    }
    while( true );

    world.reposition( part );
  }

  //***********************************
  //*    OBJECT COLLISION HANDLING    *
  //***********************************

  bool Physics::handleObjFriction()
  {
    if( ( obj->flags & ( Object::HOVER_BIT | Object::ON_LADDER_BIT ) ) ||
        obj->depth >= obj->dim.z )
    {
      // in air
      if( obj->flags & Object::HOVER_BIT ) {
        if( obj->momentum.sqL() <= AIR_STICK_VELOCITY ) {
          obj->momentum.setZero();
        }
        else {
          obj->momentum *= 1.0f - AIR_FRICTION;
        }
      }
      // swimming
      else if( obj->flags & Object::IN_WATER_BIT ) {
        float lift = ( 0.5f * obj->depth / obj->dim.z ) * obj->lift * Timer::TICK_TIME;

        obj->momentum *= 1.0f - IN_WATER_FRICTION;
        obj->momentum.z += lift + G_VELOCITY;
      }
      // on ladder
      else if( obj->flags & Object::ON_LADDER_BIT ) {
        if( obj->momentum.sqL() <= STICK_VELOCITY ) {
          obj->momentum.setZero();
        }
        else {
          obj->momentum *= 1.0f - LADDER_FRICTION;
        }
      }
    }
    else {
      if( obj->flags & Object::IN_WATER_BIT ) {
        float lift = ( 0.5f * obj->depth / obj->dim.z ) * obj->lift * Timer::TICK_TIME;

        obj->momentum.z += lift;
      }
      // on another object
      if( obj->lower != -1 ) {
        DynObject *sObj = static_cast<DynObject*>( world.objects[obj->lower] );

        if( obj->momentum.x != 0.0f || obj->momentum.y != 0.0f ||
            ( ~sObj->flags & Object::DISABLED_BIT ) )
        {
          float dx  = sObj->velocity.x - obj->momentum.x;
          float dy  = sObj->velocity.y - obj->momentum.y;
          float dv2 = dx*dx + dy*dy;

          if( dv2 > STICK_VELOCITY ) {
            obj->momentum.x += dx * OBJ_FRICTION;
            obj->momentum.y += dy * OBJ_FRICTION;

            obj->momentum.z += G_VELOCITY;
            obj->flags |= Object::FRICTING_BIT;
          }
          else {
            obj->momentum.x = sObj->velocity.x;
            obj->momentum.y = sObj->velocity.y;
            obj->momentum.z += G_VELOCITY;

            if( ( sObj->flags & Object::DISABLED_BIT ) && obj->momentum.z < 0.0f ) {
              obj->momentum.setZero();
              return false;
            }
          }
        }
        else if( obj->momentum.z > 0.0f ) {
          obj->momentum.z += G_VELOCITY;
        }
        else {
          obj->momentum.z = 0.0f;
          return false;
        }
      }
      else if( obj->flags & Object::ON_FLOOR_BIT ) {
        if( obj->flags & Object::ON_SLICK_BIT ) {
          float dx = obj->momentum.x;
          float dy = obj->momentum.y;
          float dv2 = dx*dx + dy*dy;

          if( dv2 > SLICK_STICK_VELOCITY ) {
            obj->momentum.x *= 1.0f - SLICK_FRICTION;
            obj->momentum.y *= 1.0f - SLICK_FRICTION;

            obj->momentum += ( G_VELOCITY * obj->floor.z ) * obj->floor;
            obj->flags |= Object::FRICTING_BIT;
          }
          else {
            obj->momentum.x = 0.0f;
            obj->momentum.y = 0.0f;

            if( obj->momentum.z > 0.0f ) {
              obj->momentum.z += G_VELOCITY;
            }
            else {
              obj->momentum.z = 0.0f;
              return false;
            }
          }
        }
        else {
          float dx = obj->momentum.x;
          float dy = obj->momentum.y;
          float dv2 = dx*dx + dy*dy;

          if( dv2 > STICK_VELOCITY ) {
            obj->momentum.x *= 1.0f - FLOOR_FRICTION;
            obj->momentum.y *= 1.0f - FLOOR_FRICTION;

            obj->momentum += ( G_VELOCITY * obj->floor.z ) * obj->floor;
            obj->flags |= Object::FRICTING_BIT;
          }
          else {
            obj->momentum.x = 0.0f;
            obj->momentum.y = 0.0f;

            if( obj->momentum.z > 0.0f ) {
              obj->momentum.z += G_VELOCITY;
            }
            else {
              obj->momentum.z = 0.0f;
              return false;
            }
          }
        }
      }
      else {
        obj->momentum.x *= 1.0f - AIR_FRICTION;
        obj->momentum.y *= 1.0f - AIR_FRICTION;
        obj->momentum.z += G_VELOCITY;
      }
    }

    obj->flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT | Object::IN_WATER_BIT |
        Object::ON_LADDER_BIT | Object::ON_SLICK_BIT );
    obj->lower = -1;

    return true;
  }

  void Physics::handleObjHit()
  {
    const Hit &hit = collider.hit;
    Object *sObj = hit.obj;

    if( hit.obj != null && ( hit.obj->flags & Object::DYNAMIC_BIT ) ) {
      DynObject *sDynObj = static_cast<DynObject*>( sObj );

      Vec3  momentum    = ( obj->momentum * obj->mass + sDynObj->momentum * sDynObj->mass ) /
          ( obj->mass + sDynObj->mass );
      float hitMomentum = ( obj->momentum - sDynObj->momentum ) * hit.normal;
      float hitVelocity = obj->velocity * hit.normal;

      if( hitMomentum < HIT_TRESHOLD && hitVelocity < HIT_TRESHOLD ) {
        obj->hit( &hit, hitMomentum );
        sDynObj->hit( &hit, hitMomentum );
      }

      if( hit.normal.z == 0.0f ) {
        sDynObj->flags &= ~Object::DISABLED_BIT;

        if( obj->flags & Object::PUSHER_BIT ) {
          obj->momentum.x     = sDynObj->velocity.x;
          obj->momentum.y     = sDynObj->velocity.y;
          sDynObj->momentum.x = momentum.x;
          sDynObj->momentum.y = momentum.y;
        }
        else if( hit.normal.y == 0.0f ) {
          obj->momentum.x     = sDynObj->velocity.x;
          sDynObj->momentum.x = momentum.x;
        }
        else {
          obj->momentum.y     = sDynObj->velocity.y;
          sDynObj->momentum.y = momentum.y;
        }
      }
      else if( hit.normal.z == -1.0f ) {
        sDynObj->flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT );
        sDynObj->lower = obj->index;

        obj->momentum.z     = sDynObj->velocity.z;
        sDynObj->momentum.z = momentum.z;
      }
      else { // hit.normal.z == 1.0f
        assert( hit.normal.z == 1.0f );

        sDynObj->damage( obj->mass * WEIGHT_FACTOR );

        obj->flags &= ~Object::ON_FLOOR_BIT;
        obj->lower = sDynObj->index;

        if( ~sDynObj->flags & Object::DISABLED_BIT ) {
          obj->momentum.z     = sDynObj->velocity.z;
          sDynObj->momentum.z = momentum.z;
        }
        else {
          obj->momentum.z = 0.0f;
        }
      }
    }
    else {
      float hitMomentum = obj->momentum * hit.normal;
      float hitVelocity = obj->velocity * hit.normal;

      if( hitMomentum < HIT_TRESHOLD && hitVelocity < HIT_TRESHOLD ) {
        obj->hit( &hit, hitMomentum );

        if( sObj != null ) {
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
    leftRatio = 1.0f;
    move = obj->momentum * Timer::TICK_TIME;

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
      traceSplits++;

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

          if( !cross.isZero() ) {
            cross.norm();
            move = ( move * cross ) * cross;
            move += out * EPSILON;
          }
          if( !out.isZero() ) {
            out.norm();
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

            if( !cross.isZero() ) {
              cross.norm();
              move = ( move * cross ) * cross;
              move += out * EPSILON;
            }
            if( !out.isZero() ) {
              out.norm();
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

    if( ( obj->flags & ~obj->oldFlags & Object::IN_WATER_BIT ) &&
        obj->velocity.z < SPLASH_TRESHOLD )
    {
      obj->splash( obj->velocity.z );
    }

    world.reposition( obj );
  }

  //***********************************
  //*             PUBLIC              *
  //***********************************

  void Physics::updateObj( DynObject *obj_ )
  {
    obj = obj_;

    assert( obj->cell != null );
    assert( ( ~obj->flags & Object::ON_FLOOR_BIT ) || ( obj->lower == -1 ) );

    obj->flags &= ~( Object::FRICTING_BIT | Object::HIT_BIT );

    if( obj->lower != -1 ) {
      Object *sObj = world.objects[obj->lower];

      // clear the lower object if it doesn't exist any more
      if( sObj == null || sObj->cell == null ) {
        obj->flags &= ~Object::DISABLED_BIT;
        obj->lower = -1;
      }
    }
    // handle physics
    if( ~obj->flags & Object::DISABLED_BIT ) {
      if( handleObjFriction() ) {
        // if objects is still in movement or not on a still surface after friction changed its
        // velocity, handle physics
        Vec3 oldPos = obj->p;
        handleObjMove();
        obj->velocity = ( obj->p - oldPos ) / Timer::TICK_TIME;
      }
      else {
        assert( obj->momentum.isZero() );

        obj->flags |= Object::DISABLED_BIT;
        obj->velocity.setZero();
      }
    }
  }

}
