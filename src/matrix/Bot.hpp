/*
 *  Bot.hpp
 *
 *  Bot class
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "matrix/Dynamic.hpp"
#include "matrix/Weapon.hpp"

namespace oz
{

  class Mind;

  class Bot : public Dynamic
  {
    public:

      static const int ACTION_FORWARD  = 0x00000001;
      static const int ACTION_BACKWARD = 0x00000002;
      static const int ACTION_LEFT     = 0x00000004;
      static const int ACTION_RIGHT    = 0x00000008;
      static const int ACTION_JUMP     = 0x00000010;
      static const int ACTION_CROUCH   = 0x00000020;
      static const int ACTION_USE      = 0x00000040;
      static const int ACTION_TAKE     = 0x00000080;
      static const int ACTION_GRAB     = 0x00000100;
      static const int ACTION_THROW    = 0x00000200;
      static const int ACTION_ATTACK   = 0x00000400;
      static const int ACTION_EXIT     = 0x00000800;
      static const int ACTION_EJECT    = 0x00001000;
      static const int ACTION_SUICIDE  = 0x00002000;
      static const int ACTION_INV_USE  = 0x00004000;
      static const int ACTION_INV_GRAB = 0x00008000;

      static const int EVENT_HIT_HARD  = 7;
      static const int EVENT_LAND      = 8;
      static const int EVENT_JUMP      = 9;
      static const int EVENT_FLIP      = 10;
      static const int EVENT_DEATH     = 11;

      static const int PLAYER_BIT      = 0x00000001;
      static const int DEATH_BIT       = 0x00000002;

      // can step over obstacles, e.g. walk up the stairs
      static const int STEPPING_BIT    = 0x00000004;
      static const int CROUCHING_BIT   = 0x00000008;
      static const int JUMP_SCHED_BIT  = 0x00000010;
      static const int RUNNING_BIT     = 0x00000020;
      static const int SHOOTING_BIT    = 0x00000040;
      static const int MOVING_BIT      = 0x00000080;

      static const int GESTURE0_BIT    = 0x00000100;
      static const int GESTURE1_BIT    = 0x00000200;
      static const int GESTURE2_BIT    = 0x00000400;
      static const int GESTURE3_BIT    = 0x00000800;
      static const int GESTURE4_BIT    = 0x00001000;
      static const int GESTURE_ALL_BIT = 0x00002000;
      // bot is controlled by a player, nirvana shouldn't bind a mind to it

      static const float HIT_HARD_THRESHOLD;
      static const float GRAB_EPSILON;
      static const float GRAB_STRING_RATIO;
      static const float GRAB_MOM_RATIO;
      static const float GRAB_MOM_MAX;
      static const float GRAB_MOM_MAX_SQ;
      static const float DEAD_BODY_LIFT;
      static const float BODY_FADEOUT_FACTOR;

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

      static Pool<Bot> pool;

    protected:

      virtual void onDestroy();
      virtual void onHit( const Hit* hit, float hitMomentum );
      virtual void onUpdate();
      virtual void onAct();

    public:

      float       v;
      int         state, oldState;
      int         actions, oldActions;

      float       stamina;
      float       stepRate;

      int         iGrabObj;
      float       grabHandle;

      Vector<int> items;
      int         iTaggedItem;
      int         iWeaponItem;

      String      name;

      float       camZ;
      AnimEnum    anim;

      explicit Bot();

      void take( Dynamic* item );
      void enter( int iVehicle );
      void exit();
      void kill();

      virtual void readFull( InputStream* istream );
      virtual void writeFull( OutputStream* ostream ) const;
      virtual void readUpdate( InputStream* istream );
      virtual void writeUpdate( OutputStream* ostream ) const;

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
