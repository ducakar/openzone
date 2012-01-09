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
 * @file matrix/Bot.hh
 */

#pragma once

#include "matrix/Weapon.hh"
#include "matrix/BotClass.hh"
#include "matrix/Library.hh"

namespace oz
{
namespace matrix
{

class Bot : public Dynamic
{
  public:

    /**
     * EVENTS
     */

    static const int EVENT_HIT_HARD           = 7;
    static const int EVENT_LAND               = 8;
    static const int EVENT_JUMP               = 9;
    static const int EVENT_FLIP               = 10;
    static const int EVENT_DEATH              = 11;

    /*
     * ACTIONS
     */

    static const int ACTION_FORWARD           = 0x00000001;
    static const int ACTION_BACKWARD          = 0x00000002;
    static const int ACTION_LEFT              = 0x00000004;
    static const int ACTION_RIGHT             = 0x00000008;
    static const int ACTION_JUMP              = 0x00000010;
    static const int ACTION_CROUCH            = 0x00000020;

    static const int ACTION_USE               = 0x00000040;
    static const int ACTION_TAKE              = 0x00000080;
    static const int ACTION_GRAB              = 0x00000100;
    static const int ACTION_ROTATE            = 0x00000200;
    static const int ACTION_THROW             = 0x00000400;

    static const int ACTION_INV_DROP          = 0x00000800;
    static const int ACTION_INV_GRAB          = 0x00001000;
    static const int ACTION_INV_TAKE          = 0x00002000;
    static const int ACTION_INV_GIVE          = 0x00004000;

    static const int ACTION_ATTACK            = 0x00008000;
    static const int ACTION_EXIT              = 0x00010000;
    static const int ACTION_EJECT             = 0x00020000;
    static const int ACTION_SUICIDE           = 0x00040000;

    static const int ACTION_VEH_UP            = 0x00100000;
    static const int ACTION_VEH_DOWN          = 0x00200000;
    static const int ACTION_VEH_NEXT_WEAPON   = 0x00400000;

    static const int ACTION_GESTURE0          = 0x01000000;
    static const int ACTION_GESTURE1          = 0x02000000;
    static const int ACTION_GESTURE2          = 0x04000000;
    static const int ACTION_GESTURE3          = 0x08000000;
    static const int ACTION_GESTURE4          = 0x10000000;

    static const int INSTRUMENT_ACTIONS       = ACTION_USE | ACTION_TAKE | ACTION_GRAB |
        ACTION_ROTATE | ACTION_THROW | ACTION_INV_DROP | ACTION_INV_GRAB | ACTION_INV_TAKE |
        ACTION_INV_GIVE;

    /*
     * STATE
     */

    static const int DEAD_BIT                 = 0x00000001;
    static const int MECHANICAL_BIT           = 0x00000002;
    static const int INCARNATABLE_BIT         = 0x00000004;
    // bot is currently controlled by player, nirvana shouldn't process its mind
    static const int PLAYER_BIT               = 0x00000008;

    static const int CROUCHING_BIT            = 0x00000010;
    static const int RUNNING_BIT              = 0x00000020;
    static const int SHOOTING_BIT             = 0x00000040;
    static const int MOVING_BIT               = 0x00000080;
    static const int CARGO_BIT                = 0x00000100;

    // current state bits
    static const int JUMP_SCHED_BIT           = 0x00000200;
    // on floor or an another object
    static const int GROUNDED_BIT             = 0x00000400;
    static const int ON_STAIRS_BIT            = 0x00000800;
    // climbing a ledge or a ladder
    static const int CLIMBING_BIT             = 0x00001000;
    static const int SWIMMING_BIT             = 0x00002000;
    static const int SUBMERGED_BIT            = 0x00004000;
    static const int ATTACKING_BIT            = 0x00008000;

    static const int GESTURE0_BIT             = 0x01000000;
    static const int GESTURE1_BIT             = 0x02000000;
    static const int GESTURE2_BIT             = 0x04000000;
    static const int GESTURE3_BIT             = 0x08000000;
    static const int GESTURE4_BIT             = 0x10000000;

    static const float AIR_FRICTION;
    static const float HIT_HARD_THRESHOLD;

    static const float WOUNDED_THRESHOLD;
    static const float CORPSE_FADE_FACTOR;

    static const float INSTRUMENT_DIST;
    static const float INSTRUMENT_DOT_MIN;

    static const float GRAB_EPSILON;
    static const float GRAB_STRING_RATIO;
    static const float GRAB_HANDLE_TOL;
    static const float GRAB_MOM_RATIO;
    static const float GRAB_MOM_MAX;
    static const float GRAB_MOM_MAX_SQ;

    static const float STEP_MOVE_AHEAD;
    static const float CLIMB_MOVE_AHEAD;

  public:

    static Pool<Bot, 1024> pool;

    float  h, v;
    int    actions, oldActions;
    int    instrument;
    int    container;

    int    state, oldState;
    float  stamina;
    float  stepRate;
    int    cargo;
    int    weapon;
    float  grabHandle;
    float  camZ;

    String name;
    String mindFunc;

  protected:

    virtual void onDestroy();
    virtual void onHit( const Hit* hit, float hitMomentum );
    virtual void onUpdate();

    void updateReferences();

  public:

    Object* getTagged( const Vec3& at, int mask ) const;
    Object* getTagged( int mask ) const;

    void heal();
    void rearm();
    void kill();

    void enter( int vehicle );
    void exit();

  public:

    explicit Bot( const BotClass* clazz, int index, const Point3& p, Heading heading );
    explicit Bot( const BotClass* clazz, InputStream* istream );

    virtual void write( BufferStream* ostream ) const;

    virtual void readUpdate( InputStream* istream );
    virtual void writeUpdate( BufferStream* ostream ) const;

  OZ_STATIC_POOL_ALLOC( pool )

};

}
}
