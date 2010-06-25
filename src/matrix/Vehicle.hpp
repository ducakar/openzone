/*
 *  Vehicle.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "matrix/Bot.hpp"

namespace oz
{

  class Vehicle : public Dynamic
  {
    public:

      static const int PILOT      = 0;
      static const int GUNNER     = 1;
      static const int COMMANDER  = 2;
      static const int PASSENGER0 = 3;
      static const int PASSENGER1 = 4;
      static const int PASSENGER2 = 5;
      static const int PASSENGER3 = 6;
      static const int PASSENGER4 = 7;
      static const int PASSENGER5 = 8;
      static const int CREW_MAX   = 9;

      static const float EJECT_MOVE;
      static const float EJECT_MOMENTUM;

      static Pool<Vehicle> pool;

    protected:

      virtual void onDestroy();
      virtual void onUpdate();
      virtual void onUse( Bot* user );

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
