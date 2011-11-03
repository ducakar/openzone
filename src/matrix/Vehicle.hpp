/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file matrix/Vehicle.hpp
 */

#pragma once

#include "matrix/Bot.hpp"

namespace oz
{
namespace matrix
{

class Vehicle : public Dynamic
{
  public:

    enum Type
    {
      STATIC,
      WHEELED,
      TRACKED,
      MECH,
      HOVER,
      AIR,
      TYPE_MAX
    };

    static const int   WEAPONS_MAX            = 8;

    // not in use, used to reserve a slot for engine sound sample
    static const int   EVENT_ENGINE           = 7;
    static const int   EVENT_NEXT_WEAPON      = 8;
    static const int   EVENT_SHOT0            = 9;
    static const int   EVENT_SHOT0_EMPTY      = 10;
    static const int   EVENT_SHOT1            = 11;
    static const int   EVENT_SHOT1_EMPTY      = 12;
    static const int   EVENT_SHOT2            = 13;
    static const int   EVENT_SHOT2_EMPTY      = 14;

    static const int   CREW_VISIBLE_BIT       = 0x00000001;

    static const float AIR_FRICTION;
    static const float EXIT_EPSILON;
    static const float EXIT_MOMENTUM;
    static const float EJECT_MOMENTUM;

    static Pool<Vehicle, 256> pool;

  protected:

    static void ( Vehicle::* handlers[TYPE_MAX] )( const Mat44& rotMat );

    void staticHandler( const Mat44& rotMat );
    void wheeledHandler( const Mat44& rotMat );
    void trackedHandler( const Mat44& rotMat );
    void mechHandler( const Mat44& rotMat );
    void hoverHandler( const Mat44& rotMat );
    void airHandler( const Mat44& rotMat );

    virtual void onDestroy();
    virtual void onUpdate();
    virtual bool onUse( Bot* user );

  public:

    float h, v;
    Quat  rot;
    Vec3  camPos;

    int   state, oldState;
    int   actions, oldActions;

    int   weapon;
    int   nRounds[WEAPONS_MAX];
    float shotTime[WEAPONS_MAX];

    int   pilot;

    Vehicle();

    void service();

    virtual void readFull( InputStream* istream );
    virtual void writeFull( BufferStream* ostream ) const;
    virtual void readUpdate( InputStream* istream );
    virtual void writeUpdate( BufferStream* ostream ) const;

  OZ_STATIC_POOL_ALLOC( pool )

};

}
}
