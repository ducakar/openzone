/*
 *  Vehicle.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Bot.hpp"

namespace oz
{

  class Vehicle : public Dynamic
  {
    public:

      enum Type
      {
        WHEELED,
        TRACKED,
        HOVER,
        AIR
      };

      static const int   WEAPONS_MAX            = 8;

      // not used, to reserve slot for engine sound sample
      static const int   EVENT_ENGINE           = 7;
      static const int   EVENT_SERVICE          = 8;
      static const int   EVENT_NEXT_WEAPON      = 9;
      static const int   EVENT_SHOT0            = 10;
      static const int   EVENT_SHOT0_EMPTY      = 11;
      static const int   EVENT_SHOT1            = 12;
      static const int   EVENT_SHOT1_EMPTY      = 13;
      static const int   EVENT_SHOT2            = 14;
      static const int   EVENT_SHOT2_EMPTY      = 15;

      static const int   CREW_VISIBLE_BIT       = 0x00000001;

      static const float EXIT_EPSILON;
      static const float EXIT_MOMENTUM;
      static const float EJECT_MOMENTUM;

      static Pool<Vehicle> pool;

    protected:

      static void ( Vehicle::* handlers[4] )( const Mat44& rotMat );

      void wheeledHandler( const Mat44& rotMat );
      void trackedHandler( const Mat44& rotMat );
      void hoverHandler( const Mat44& rotMat );
      void airHandler( const Mat44& rotMat );

      virtual void onDestroy();
      virtual void onUpdate();
      virtual void onUse( Bot* user );

    public:

      float h, v;
      Quat  rot;
      Vec3  camPos;

      int   state, oldState;
      int   actions, oldActions;

      int   weapon;
      int   nShots[WEAPONS_MAX];
      float shotTime[WEAPONS_MAX];

      int   pilot;

      Vehicle();

      void service();

      virtual void readFull( InputStream* istream );
      virtual void writeFull( OutputStream* ostream ) const;
      virtual void readUpdate( InputStream* istream );
      virtual void writeUpdate( OutputStream* ostream ) const;

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
