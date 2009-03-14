/*
 *  Physics.h
 *
 *  Physics engine
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Collider.hpp"
#include "DynObject.hpp"

namespace oz
{

  class Physics
  {
    protected:

      static const int   MAX_TRACE_SPLITS = 3;
      static const float CLIP_BACKOFF;

      static const float STICK_VELOCITY;

      static const float AIR_FRICTION;
      static const float WATER_FRICTION;
      static const float FLOOR_FRICTION;
      static const float OBJ_FRICTION;

      Vec3      leafStartPos;
      Vec3      leafEndPos;
      Vec3      globalStartPos;
      Vec3      globalEndPos;

      float     leafStartRatio;
      float     leafEndRatio;

      float     gVelocity;
      float     gAccel;
      Vec3      lastNormals[2];
      float     leftRatio;

      Vec3      move;
      Bounds    trace;

      Particle  *part;
      DynObject *obj;
      BSP       *bsp;

      void handlePartHit();
      void handlePartMove();

      bool handleObjFriction();
      void handleObjHit();
      void handleObjMove();

    public:

      static const float MAX_VELOCITY2;
      static const float FLOOR_NORMAL_Z;

      void init( float gAccel );

      void beginUpdate()
      {
        gVelocity = gAccel * timer.frameTime;
      }

      void endUpdate()
      {}

      void updatePart( Particle *part_ )
      {
        part = part_;

        part->velocity.z += gVelocity;
        part->lifeTime -= timer.frameTime;

        part->rot += part->rotVelocity * timer.frameTime;
        handlePartMove();
      }

      void updateObj( DynObject *obj );

  };

  extern Physics physics;

}
