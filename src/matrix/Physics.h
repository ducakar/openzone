/*
 *  Physics.h
 *
 *  Physics engine
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Collider.h"

namespace oz
{

  class Physics
  {
    public:

      static const float CLIP_BACKOFF;
      static const float STICK_VELOCITY;
      static const float SLICK_STICK_VELOCITY;
      static const float HIT_MOMENTUM;
      static const float FLOOR_NORMAL_Z;

      static const float AIR_FRICTION;
      static const float IN_WATER_FRICTION;
      static const float ON_WATER_FRICTION;
      static const float LADDER_FRICTION;
      static const float FLOOR_FRICTION;
      static const float OBJ_FRICTION;
      static const float SLICK_FRICTION;

    private:

      Vec3      leafStartPos;
      Vec3      leafEndPos;
      Vec3      globalStartPos;
      Vec3      globalEndPos;

      float     leafStartRatio;
      float     leafEndRatio;

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

      float     gAccel;
      float     gVelocity;

      void setG( float gAccel );
      void update();

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
