/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#include <matrix/Bot.hh>
#include <matrix/VehicleClass.hh>

namespace oz
{

class Vehicle : public Dynamic
{
  public:

    static const int   MAX_WEAPONS       = 4;

    // not in use, used to reserve a slot for engine sound sample
    static const int   EVENT_ENGINE      = 9;
    static const int   EVENT_NEXT_WEAPON = 10;
    static const int   EVENT_SHOT0       = 11;
    static const int   EVENT_SHOT1       = 12;
    static const int   EVENT_SHOT2       = 13;
    static const int   EVENT_SHOT3       = 14;
    static const int   EVENT_SHOT_EMPTY  = 15;

    static const int   CREW_VISIBLE_BIT  = 0x0001;
    static const int   HAS_EJECT_BIT     = 0x0004;
    static const int   AUTO_EJECT_BIT    = 0x0008;

  protected:

    static const float ROT_DIFF_LIMIT;
    static const float ROT_VEL_DIFF_RATIO;
    static const float AIR_FRICTION;
    static const float EXIT_EPSILON;
    static const float EXIT_MOMENTUM;
    static const float EJECT_EPSILON;
    static const float EJECT_MOMENTUM;

    typedef void ( Vehicle::* Handler )();

    static const Handler HANDLERS[];

  public:

    static Pool<Vehicle, 256> pool;

    float h, v, w;
    float rotVelH, rotVelV;
    int   actions, oldActions;

    Mat44 rot;
    int   state, oldState;
    float fuel;

    int   pilot;

    int   weapon;
    int   nRounds[MAX_WEAPONS];
    float shotTime[MAX_WEAPONS];

  public:

    void exit();
    void eject();
    void service();

    void staticHandler();
    void wheeledHandler();
    void trackedHandler();
    void mechHandler();
    void hoverHandler();
    void airHandler();

    void onDestroy() override;
    void onUpdate() override;
    bool onUse( Bot* user ) override;

  public:

    explicit Vehicle( const VehicleClass* clazz, int index, const Point& p, Heading heading );
    explicit Vehicle( const VehicleClass* clazz, InputStream* istream );
    explicit Vehicle( const VehicleClass* clazz, const JSON& json );

    void write( OutputStream* ostream ) const override;
    JSON write() const override;

    void readUpdate( InputStream* istream ) override;
    void writeUpdate( OutputStream* ostream ) const override;

    OZ_STATIC_POOL_ALLOC( pool )

};

}
