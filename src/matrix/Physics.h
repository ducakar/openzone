/*
 *  Physics.h
 *
 *  Physics engine
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "World.h"

namespace oz
{

  class Physics
  {
    public:

      static const float CLIP_BACKOFF;
      static const float HIT_TRESHOLD;
      static const float HIT_NORMAL_TRESHOLD;
      static const float SPLASH_TRESHOLD;
      static const float FLOOR_NORMAL_Z;
      static const float G_VELOCITY;
      static const float WEIGHT_FACTOR;

      static const float STICK_VELOCITY;
      static const float AIR_STICK_VELOCITY;
      static const float SLICK_STICK_VELOCITY;
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

      void updatePart( Particle *part_ )
      {
        part = part_;

        assert( part->cell != null );

        part->velocity.z += G_VELOCITY;
        part->lifeTime -= Timer::TICK_TIME;

        part->rot += part->rotVelocity * Timer::TICK_TIME;
        handlePartMove();
      }

      void updateObj( DynObject *obj );

  };

  extern Physics physics;

}
