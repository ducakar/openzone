/*
 *  Physics.hpp
 *
 *  Physics engine
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Orbis.hpp"

namespace oz
{

  class Physics
  {
    public:

      static const float MOVE_BOUNCE;
      static const float HIT_THRESHOLD;
      static const float SPLASH_THRESHOLD;
      static const float FLOOR_NORMAL_Z;
      static const float G_ACCEL;
      static const float WEIGHT_FACTOR;

      static const float STICK_VELOCITY;
      static const float SLICK_STICK_VELOCITY;
      static const float FLOAT_STICK_VELOCITY;
      static const float AIR_FRICTION;
      static const float WATER_FRICTION;
      static const float LADDER_FRICTION;
      static const float FLOOR_FRICTION;
      static const float SLICK_FRICTION;

      static const float STRUCT_HIT_MAX_MASS;
      static const float STRUCT_HIT_RATIO;

      static const float PART_HIT_VELOCITY2;
      static const float PART_DESTROY_VELOCITY2;

    private:

      Dynamic*  dyn;
      Particle* part;
      Bounds    trace;
      Vec3      move;
      float     leftRatio;

      Vec3      lastNormals[2];

      void handlePartHit();
      void handlePartMove();

      bool handleObjFriction();
      void handleObjHit();
      void handleObjMove();

    public:

      void updatePart( Particle* part );
      void updateObj( Dynamic* dyn );

  };

  extern Physics physics;

}
