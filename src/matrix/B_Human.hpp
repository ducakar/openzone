/*
 *  B_Human.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Bot.hpp"
#include "B_Spirit.hpp"

namespace oz
{

  class B_Human : public Bot
  {
    protected:

      static const Vec3  CAM_POS;
      static const Vec3  CAM_POS_CROUCH;
      static const Vec3  DIM;
      static const Vec3  DIM_CROUCH;

      static const float BOB_INC;
      static const float BOB_AMPLITUDE;

      static const float WALK_VELOCITY;
      static const float RUN_VELOCITY;
      static const float JUMP_VELOCITY;

      static const float CROUCH_VELOCITY;
      static const float AIR_CONTROL;
      static const float GRAB_DIST;

      static const float STEP_INC;
      static const float STEP_MAX;

      void onUpdate();
      void onHit( const Hit *hit );
      void onDestroy();

    public:

      static const char *NAME;
      static const int TYPE;

      explicit B_Human( const Vec3 &p, float h = 0.0f, float v = 0.0f, Mind *mind = null );

//       void createModel();
  };

}
