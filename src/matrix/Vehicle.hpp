/*
 *  Vehicle.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
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

      enum Role : int
      {
        PILOT,
        GUNNER,
        COMMANDER,
        PASSENGER0,
        PASSENGER1,
        PASSENGER2,
        PASSENGER3,
        PASSENGER4,
        PASSENGER5,
        CREW_MAX
      };

      static const float EJECT_MOVE;
      static const float EJECT_MOMENTUM;

      static Pool<Vehicle> pool;

    protected:

      virtual void onDestroy();
      virtual void onUpdate();
      virtual void onUse( Bot* user );

    protected:

      float oldH;
      float oldV;

    public:

      Quat rot;
      int  state, oldState;
      int  actions, oldActions;
      Vec3 camPos;

      int  crew[CREW_MAX];

      explicit Vehicle();

      void exit( Bot* bot );

      virtual void readFull( InputStream* istream );
      virtual void writeFull( OutputStream* ostream ) const;
      virtual void readUpdate( InputStream* istream );
      virtual void writeUpdate( OutputStream* ostream ) const;

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
