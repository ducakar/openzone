/*
 *  Physics.hpp
 *
 *  Physics engine
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "matrix/World.hpp"

namespace oz
{

  class Physics
  {
    public:

      static const float CLIP_BACKOFF;
      static const float HIT_THRESHOLD;
      static const float SPLASH_THRESHOLD;
      static const float FLOOR_NORMAL_Z;
      static const float G_VELOCITY;
      static const float WEIGHT_FACTOR;

      static const float STICK_VELOCITY;
      static const float AIR_STICK_VELOCITY;
      static const float SLICK_STICK_VELOCITY;
      static const float BOTTOM_STICK_VELOCITY;
      static const float AIR_FRICTION;
      static const float WATER_FRICTION;
      static const float LADDER_FRICTION;
      static const float FLOOR_FRICTION;
      static const float OBJ_FRICTION;
      static const float SLICK_FRICTION;

      static const float PART_HIT_VELOCITY2;
      static const float PART_DESTROY_VELOCITY2;

    private:

      Dynamic*  obj;
      Particle* part;
      Bounds    trace;
      Vec4      move;
      float     leftRatio;

      Vec4      lastNormals[2];

      void handlePartHit();
      void handlePartMove();

      bool handleObjFriction();
      void handleObjHit();
      void handleObjMove();

    public:

      void updatePart( Particle* part_ )
      {
        part = part_;

        assert( part->cell != null );

        part->velocity.z += G_VELOCITY;
        part->lifeTime -= Timer::TICK_TIME;

        part->rot += part->rotVelocity * Timer::TICK_TIME;
        handlePartMove();
      }

      void updateObj( Dynamic* obj );

  };

  extern Physics physics;

}
