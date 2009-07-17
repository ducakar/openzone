/*
 *  Bot.h
 *
 *  Bot class
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "DynObject.h"
#include "Weapon.h"

#define BOT_GRAB_DIST         1.0

namespace oz
{

  class Mind;

  class Bot : public DynObject
  {
    public:

      static const int KEY_FORWARD     = 0x00000001;
      static const int KEY_BACKWARD    = 0x00000002;
      static const int KEY_LEFT        = 0x00000004;
      static const int KEY_RIGHT       = 0x00000008;
      static const int KEY_JUMP        = 0x00000010;
      static const int KEY_CROUCH      = 0x00000020;
      static const int KEY_RUN         = 0x00000040;
      static const int KEY_FIRE        = 0x00000080;
      static const int KEY_NOCLIP      = 0x00000100;
      static const int KEY_SUICIDE     = 0x00000200;
      static const int KEY_GESTURE0    = 0x00000400;
      static const int KEY_GESTURE1    = 0x00000800;
      static const int KEY_USE         = 0x00001000;
      static const int KEY_STEP        = 0x00002000;
      static const int KEY_FREELOOK    = 0x00004000;

      static const int STEPPING_BIT    = 0x00000001;
      static const int CROUCHING_BIT   = 0x00000002;
      static const int FREELOOK_BIT    = 0x00000004;
      static const int RUNNING_BIT     = 0x00000010;
      static const int SHOOTING_BIT    = 0x00000020;
      static const int GROUNDED_BIT    = 0x00000040;
      static const int MOVING_BIT      = 0x00000080;
      static const int GESTURE0_BIT    = 0x00000100;
      static const int GESTURE1_BIT    = 0x00000200;
      static const int DEATH_BIT       = 0x00000400;

      enum AnimEnum
      {
        ANIM_STAND,
        ANIM_RUN,
        ANIM_ATTACK,
        ANIM_PAIN_A,
        ANIM_PAIN_B,
        ANIM_PAIN_C,
        ANIM_JUMP,
        ANIM_FLIP,
        ANIM_SALUTE,
        ANIM_FALLBACK,
        ANIM_WAVE,
        ANIM_POINT,
        ANIM_CROUCH_STAND,
        ANIM_CROUCH_WALK,
        ANIM_CROUCH_ATTACK,
        ANIM_CROUCH_PAIN,
        ANIM_CROUCH_DEATH,
        ANIM_DEATH_FALLBACK,
        ANIM_DEATH_FALLFORWARD,
        ANIM_DEATH_FALLBACKSLOW,
        ANIM_MAX
      };

    protected:

      void onUpdate();
      void onHit( const Hit *hit, float hitVelocity );
      void onDestroy();

    public:

      Mind     *mind;

      int      state;
      AnimEnum anim;

      int      keys, oldKeys;

      float    h, v;
      float    bob;
      Vec3     camPos;

      float    deathTime;
      Weapon   *weapon;

      Vector<Object*> items;

      explicit Bot();

      Quat getRot() const
      {
        return Quat::rotZYX( Math::rad( h ), 0.0f, Math::rad( v ) );
      }

      virtual void readFull( InputStream *istream );
      virtual void writeFull( OutputStream *ostream );
      virtual void readUpdates( InputStream *istream );
      virtual void writeUpdates( OutputStream *ostream );
  };

}
