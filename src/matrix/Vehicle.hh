/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 */

/**
 * @file matrix/Vehicle.hh
 */

#pragma once

#include "matrix/Bot.hh"
#include "matrix/VehicleClass.hh"

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

    static const int   MAX_WEAPONS       = 4;

    // not in use, used to reserve a slot for engine sound sample
    static const int   EVENT_ENGINE      = 7;
    static const int   EVENT_NEXT_WEAPON = 8;
    static const int   EVENT_SHOT0       = 9;
    static const int   EVENT_SHOT1       = 10;
    static const int   EVENT_SHOT2       = 11;
    static const int   EVENT_SHOT3       = 12;
    static const int   EVENT_SHOT_EMPTY  = 13;

    static const int   CREW_VISIBLE_BIT  = 0x0001;
    static const int   HAS_EJECT_BIT     = 0x0004;
    static const int   AUTO_EJECT_BIT    = 0x0008;

  protected:

    static const float ROT_DIFF_LIMIT;
    static const float ROT_VEL_DIFF_RATIO;
    static const float AIR_FRICTION;
    static const float EXIT_EPSILON;
    static const float EXIT_MOMENTUM;
    static const float EJECT_MOMENTUM;

  public:

    static Pool<Vehicle, 256> pool;

    float h, v;
    float rotVelH, rotVelV;
    int   actions, oldActions;

    Quat  rot;
    int   state, oldState;
    float fuel;

    int   pilot;

    int   weapon;
    int   nRounds[MAX_WEAPONS];
    float shotTime[MAX_WEAPONS];

  protected:

    static void ( Vehicle::* handlers[TYPE_MAX] )( const Mat44& rotMat );

    void exit();
    void eject();

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

    void service();

  public:

    explicit Vehicle( const VehicleClass* clazz, int index, const Point3& p, Heading heading );
    explicit Vehicle( const VehicleClass* clazz, InputStream* istream );

    virtual void write( BufferStream* ostream ) const;

    virtual void readUpdate( InputStream* istream );
    virtual void writeUpdate( BufferStream* ostream ) const;

  OZ_STATIC_POOL_ALLOC( pool )

};

}
}
