/*
 *  Physics.cpp
 *
 *  Physics engine
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Physics.h"

namespace oz
{

  Physics physics;

  const float Physics::CLIP_BACKOFF     = 3.0f * EPSILON;
  const float Physics::STICK_VELOCITY   = 0.015f;
  const float Physics::HIT_VELOCITY     = 4.0f;

  const float Physics::AIR_FRICTION     = 0.05f;
  const float Physics::WATER_FRICTION   = 0.05f;
  const float Physics::LADDER_FRICTION  = 0.05f;
  const float Physics::FLOOR_FRICTION   = 0.20f;
  const float Physics::OBJ_FRICTION     = 0.20f;

  // default 10000.0f: 100 m/s
  const float Physics::MAX_VELOCITY2    = 10000.0f;
  const float Physics::FLOOR_NORMAL_Z   = 0.70f;

  //***********************************
  //*   PARTICLE COLLISION HANDLING   *
  //***********************************

  void Physics::handlePartHit()
  {
    float hitVelocity = part->velocity * collider.hit.normal;
    part->velocity -= ( part->rejection * hitVelocity ) * collider.hit.normal;

    Object *sObj = collider.hit.obj;
    if( sObj != null ) {
      sObj->hit( &collider.hit, hitVelocity );
    }
  }

  void Physics::handlePartMove()
  {
    leftRatio = 1.0f;
    move = part->velocity * timer.frameTime;

    Sector *oldSector = part->sector;

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

      if( traceSplits >= MAX_TRACE_SPLITS ) {
        break;
      }
      traceSplits++;

      move *= 1.0f - collider.hit.ratio;
      move -= ( move * collider.hit.normal - CLIP_BACKOFF ) * collider.hit.normal;
    }
    while( true );

    Sector *newSector = world.getSector( part->p );

    if( oldSector != newSector ) {
      oldSector->particles.remove( part );
      newSector->particles << part;
      part->sector = newSector;
    }
  }

  //***********************************
  //*    OBJECT COLLISION HANDLING    *
  //***********************************

  bool Physics::handleObjFriction()
  {
    if( obj->flags & ( Object::HOVER_BIT | Object::UNDER_WATER_BIT | Object::ON_LADDER_BIT ) ) {
      // in air
      if( obj->flags & Object::HOVER_BIT ) {
        if( obj->newVelocity.sqL() <= STICK_VELOCITY ) {
          obj->newVelocity.setZero();
        }
        else {
          obj->newVelocity *= 1.0f - AIR_FRICTION;
        }
      }
      // swimming
      else if( obj->flags & Object::UNDER_WATER_BIT ) {
        obj->newVelocity *= 1.0f - WATER_FRICTION;
        obj->newVelocity.z += obj->lift;
      }
      // on ladder
      else if( obj->flags &  Object::ON_LADDER_BIT ) {
        if( obj->newVelocity.sqL() <= STICK_VELOCITY ) {
          obj->newVelocity.setZero();
        }
        else {
          obj->newVelocity *= 1.0f - LADDER_FRICTION;
        }
      }
      obj->flags &= ~( Object::HOVER_BIT | Object::UNDER_WATER_BIT | Object::ON_LADDER_BIT |
          Object::ON_FLOOR_BIT );
      obj->lower = -1;
    }
    else {
      if( obj->flags & Object::IN_WATER_BIT ) {
        obj->flags &= ~Object::IN_WATER_BIT;

        obj->newVelocity *= 1.0f - WATER_FRICTION;
      }
      // on another object
      if( obj->lower >= 0 ) {
        DynObject *sObj = (DynObject*) world.objects[obj->lower];

        if( obj->newVelocity.x != 0.0f || obj->newVelocity.y != 0.0f ||
            sObj->velocity.x != 0.0f || sObj->velocity.y != 0.0f )
        {
          float dx = sObj->velocity.x - obj->newVelocity.x;
          float dy = sObj->velocity.y - obj->newVelocity.y;

          obj->newVelocity.z += gVelocity;
          obj->lower = -1;

          if( ( dx*dx + dy*dy ) > STICK_VELOCITY ) {
            obj->newVelocity.x += dx * OBJ_FRICTION;
            obj->newVelocity.y += dy * OBJ_FRICTION;
            obj->flags |= Object::FRICTING_BIT;
          }
          else {
            obj->newVelocity.x = sObj->velocity.x;
            obj->newVelocity.y = sObj->velocity.y;
          }
        }
        else {
          if( obj->newVelocity.z == 0.0f && sObj->velocity.z == 0.0f ) {
            return false;
          }
          else {
            obj->newVelocity.z += gVelocity;
            obj->lower = -1;
          }
        }
      }
      else if( obj->flags & Object::ON_FLOOR_BIT ) {
        float xyDot = obj->newVelocity.x * obj->newVelocity.x +
            obj->newVelocity.y * obj->newVelocity.y;

        if( xyDot > STICK_VELOCITY ) {
          obj->newVelocity.x *= 1.0f - FLOOR_FRICTION;
          obj->newVelocity.y *= 1.0f - FLOOR_FRICTION;

          obj->newVelocity += ( gVelocity * obj->floor.z ) * obj->floor;
          obj->flags &= ~Object::ON_FLOOR_BIT;
          obj->flags |= Object::FRICTING_BIT;
        }
        else {
          obj->newVelocity.x = 0.0f;
          obj->newVelocity.y = 0.0f;

          if( obj->newVelocity.z <= STICK_VELOCITY ) {
            obj->newVelocity.z = 0.0f;
            return false;
          }
          else {
            obj->flags &= ~Object::ON_FLOOR_BIT;
          }
        }
      }
      else {
        obj->newVelocity.x *= 1.0f - AIR_FRICTION;
        obj->newVelocity.y *= 1.0f - AIR_FRICTION;
        obj->newVelocity.z += gVelocity;
      }
    }
    return true;
  }

  void Physics::handleObjHit()
  {
    Object *sObj = collider.hit.obj;
    Vec3 oldVelocity = obj->newVelocity;

    if( collider.hit.obj != null && ( collider.hit.obj->flags & Object::DYNAMIC_BIT ) ) {
      DynObject *sDynObj = (DynObject*) sObj;
      sDynObj->flags &= ~Object::DISABLED_BIT;

      Vec3  deltaVel = obj->newVelocity - sDynObj->velocity;
      float massSum  = obj->mass + sDynObj->mass;

      float hitVelocity = max( max( Math::abs( deltaVel.x ), Math::abs( deltaVel.y ) ),
                               Math::abs( deltaVel.z ) );

      if( hitVelocity > HIT_VELOCITY ) {
        obj->hit( &collider.hit, hitVelocity );
        obj->flags |= Object::HIT_BIT;

        sDynObj->hit( &collider.hit, hitVelocity );
        sDynObj->flags |= Object::HIT_BIT;
      }

      if( collider.hit.normal.z == 0.0f ) {
        if( obj->flags & Object::PUSHING_BIT ) {
          float pushX = deltaVel.x / massSum;
          float pushY = deltaVel.y / massSum;

          obj->newVelocity.x -= pushX * sDynObj->mass;
          obj->newVelocity.y -= pushY * sDynObj->mass;

          sDynObj->newVelocity.x += pushX * obj->mass;
          sDynObj->newVelocity.y += pushY * obj->mass;
        }
        else if( collider.hit.normal.y == 0.0f ) {
          float pushX = deltaVel.x / massSum;

          obj->newVelocity.x -= pushX * sDynObj->mass;
          sDynObj->newVelocity.x += pushX * obj->mass;
        }
        else {
          float pushY = deltaVel.y / massSum;

          obj->newVelocity.y -= pushY * sDynObj->mass;
          sDynObj->newVelocity.y += pushY * obj->mass;
        }
      }
      else if( collider.hit.normal.z == -1.0f ) {
        float pushZ = deltaVel.z / massSum;

        obj->newVelocity.z -= pushZ * sDynObj->mass;
        sDynObj->newVelocity.z += pushZ * obj->mass;

        sDynObj->flags &= ~Object::ON_FLOOR_BIT;
        sDynObj->lower = obj->index;
      }
      else {
        float pushZ = deltaVel.z / massSum;

        obj->newVelocity.z = sDynObj->velocity.z;
        sDynObj->newVelocity.z += pushZ * obj->mass;

        obj->flags &= ~Object::ON_FLOOR_BIT;
        obj->lower = sDynObj->index;
      }
    }
    else {
      float hitVelocity = obj->newVelocity * collider.hit.normal;
      if( hitVelocity > HIT_VELOCITY ) {
        obj->hit( &collider.hit, hitVelocity );
        obj->flags |= Object::HIT_BIT;

        if( sObj != null ) {
          sObj->hit( &collider.hit, hitVelocity );
          sObj->flags |= Object::HIT_BIT;
        }
      }

      obj->newVelocity -= ( obj->newVelocity * collider.hit.normal ) * collider.hit.normal;

      if( !( obj->flags & Object::HOVER_BIT ) && collider.hit.normal.z > FLOOR_NORMAL_Z ) {
        obj->lower = -1;
        obj->floor = collider.hit.normal;
        obj->flags |= Object::ON_FLOOR_BIT;
      }
    }
  }

  void Physics::handleObjMove()
  {
    leftRatio = 1.0f;
    move = obj->newVelocity * timer.frameTime;

    Sector *oldSector = obj->sector;

    int traceSplits = 0;
    do {
      collider.translate( obj, move );
      obj->p += collider.hit.ratio * move;
      leftRatio -= leftRatio * collider.hit.ratio;

      obj->flags |= collider.hit.inWater    ? Object::IN_WATER_BIT    : 0;
      obj->flags |= collider.hit.underWater ? Object::UNDER_WATER_BIT : 0;
      obj->flags |= collider.hit.onLadder   ? Object::ON_LADDER_BIT   : 0;

      if( collider.hit.ratio == 1.0f ) {
        break;
      }
      // collision response
      handleObjHit();

      if( traceSplits >= MAX_TRACE_SPLITS ) {
        break;
      }
      traceSplits++;

      move *= 1.0f - collider.hit.ratio;
      move -= ( move * collider.hit.normal - CLIP_BACKOFF ) * collider.hit.normal;

      // to prevent getting stuck in corners < 90° and to prevent oscillations in corners > 90°
      if( traceSplits > 1 ) {
        float dot = lastNormals[0] * collider.hit.normal;

        if( dot != 0.0f ) {
          Vec3 cross = collider.hit.normal ^ lastNormals[0];

          if( !cross.isZero() ) {
            cross.norm();
            move = ( move * cross ) * cross;
            move += ( collider.hit.normal + lastNormals[0] ) * EPSILON;
          }
        }
        if( traceSplits > 2 ) {
          dot = lastNormals[1] * collider.hit.normal;

          if( dot != 0.0f ) {
            Vec3 cross = collider.hit.normal ^ lastNormals[1];

            if( !cross.isZero() ) {
              cross.norm();
              move = ( move * cross ) * cross;
              move += ( collider.hit.normal + lastNormals[1] ) * EPSILON;
            }
          }
        }
        else {
          lastNormals[1] = lastNormals[0];
          lastNormals[0] = collider.hit.normal;
        }
      }
      else {
        lastNormals[0] = collider.hit.normal;
      }
    }
    while( true );

    Sector *newSector = world.getSector( obj->p );

    if( oldSector != newSector ) {
      oldSector->objects.remove( obj );
      newSector->objects << obj;
      obj->sector = newSector;
    }
  }

  //***********************************
  //*             PUBLIC              *
  //***********************************

  void Physics::init( float gAccel_ )
  {
    gAccel = gAccel_;
  }

  void Physics::updateObj( DynObject *obj_ )
  {
    obj = obj_;

    assert( obj->flags & Object::DYNAMIC_BIT );
    assert( obj->sector != null );
    assert( !( obj->flags & Object::ON_FLOOR_BIT ) || !( obj->lower >= 0 ) );

    if( obj->flags & Object::CLIP_BIT ) {
      obj->flags &= ~( Object::HIT_BIT | Object::FRICTING_BIT );

      // clear lower object if doesn't exist any more
      if( obj->lower >= 0 &&
          ( obj->lower >= world.objects.length() || world.objects[obj->lower] == null ) )
      {
        obj->lower = -1;
        obj->flags &= ~Object::DISABLED_BIT;
      }
      // check if object can remain disabled
      else if( obj->flags & Object::DISABLED_BIT ) {
        if( obj->lower >= 0 && !( world.objects[obj->lower]->flags & Object::DISABLED_BIT ) ) {
          obj->flags &= ~Object::DISABLED_BIT;
        }
        if( obj->flags & Object::IN_WATER_BIT ) {
          obj->flags &= ~Object::DISABLED_BIT;
        }
      }
      // disable object if it is still and on still surface
      else if( obj->newVelocity.isZero() &&
          !( obj->flags & Object::UNDER_WATER_BIT ) &&
          ( ( obj->flags & Object::ON_FLOOR_BIT ) ||
          ( obj->lower >= 0 &&
          ( (DynObject*) world.objects[obj->lower] )->newVelocity.isZero() ) ) )
      {
        obj->flags |= Object::DISABLED_BIT;
      }
      // handle physics
      if( !( obj->flags & Object::DISABLED_BIT ) ) {
        if( handleObjFriction() ) {
          obj->flags &= ~( Object::IN_WATER_BIT | Object::UNDER_WATER_BIT | Object::SLIPPING_BIT );
          // if objects is still in movement or not on a still surface after friction changed its
          // velocity, handle physics
          handleObjMove();
          obj->newVelocity *= 1.0f - leftRatio;
        }
        obj->velocity = obj->newVelocity;
      }
    }
    else {
      if( !( obj->flags & Object::DISABLED_BIT ) ) {
        if( obj->newVelocity * obj->newVelocity <= STICK_VELOCITY ) {
          obj->newVelocity.setZero();
          obj->flags |= Object::DISABLED_BIT;
        }
        else {
          obj->newVelocity *= 1.0f - AIR_FRICTION;
        }

        obj->p += obj->newVelocity * timer.frameTime;
        obj->velocity = obj->newVelocity;

        Sector *sector = world.getSector( obj->p );

        if( obj->sector != sector ) {
          obj->sector->objects.remove( obj );
          sector->objects << obj;
          obj->sector = sector;
        }
      }
    }
  }

}
