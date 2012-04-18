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

#include "matrix/Struct.hh"
#include "matrix/Weapon.hh"
#include "matrix/BotClass.hh"

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

    static const int EVENT_JUMP             = 8;
    static const int EVENT_FLIP             = 9;
    static const int EVENT_DEATH            = 10;

    // Step and swim events are not in use but merely reserve sound slots.
    static const int EVENT_STEP             = 11;
    static const int EVENT_WATERSTEP        = 12;
    static const int EVENT_SWIM             = 13;

    /*
     * ACTIONS
     */

    static const int ACTION_FORWARD         = 0x00000001;
    static const int ACTION_BACKWARD        = 0x00000002;
    static const int ACTION_RIGHT           = 0x00000004;
    static const int ACTION_LEFT            = 0x00000008;
    static const int ACTION_JUMP            = 0x00000010;
    static const int ACTION_CROUCH          = 0x00000020;

    static const int ACTION_ATTACK          = 0x00000040;
    static const int ACTION_EXIT            = 0x00000080;
    static const int ACTION_EJECT           = 0x00000100;
    static const int ACTION_SUICIDE         = 0x00000200;

    static const int ACTION_VEH_UP          = 0x00000400;
    static const int ACTION_VEH_DOWN        = 0x00000800;
    static const int ACTION_VEH_NEXT_WEAPON = 0x00001000;

    static const int ACTION_POINT           = 0x00002000;
    static const int ACTION_BACK            = 0x00004000;
    static const int ACTION_SALUTE          = 0x00008000;
    static const int ACTION_WAVE            = 0x00010000;
    static const int ACTION_FLIP            = 0x00020000;

    static const int ACTION_TRIGGER         = 0x00040000;
    static const int ACTION_LOCK            = 0x00080000;
    static const int ACTION_USE             = 0x00100000;
    static const int ACTION_TAKE            = 0x00200000;
    static const int ACTION_GRAB            = 0x00400000;
    static const int ACTION_ROTATE          = 0x00800000;
    static const int ACTION_THROW           = 0x01000000;

    static const int ACTION_INV_USE         = 0x02000000;
    static const int ACTION_INV_TAKE        = 0x04000000;
    static const int ACTION_INV_GIVE        = 0x08000000;
    static const int ACTION_INV_DROP        = 0x10000000;
    static const int ACTION_INV_GRAB        = 0x20000000;

    static const int ACTION_GESTURE_MASK    = ACTION_POINT | ACTION_BACK | ACTION_SALUTE |
                                              ACTION_WAVE | ACTION_FLIP;

    static const int INSTRUMENT_ACTIONS     = ACTION_TRIGGER | ACTION_LOCK | ACTION_USE |
                                              ACTION_TAKE | ACTION_GRAB | ACTION_ROTATE |
                                              ACTION_THROW | ACTION_INV_USE | ACTION_INV_TAKE |
                                              ACTION_INV_GIVE | ACTION_INV_DROP | ACTION_INV_GRAB;

    /*
     * STATE
     */

    static const int DEAD_BIT               = 0x00000001;
    static const int MECHANICAL_BIT         = 0x00000002;
    static const int INCARNATABLE_BIT       = 0x00000004;
    // bot is currently controlled by player, nirvana shouldn't process its mind
    static const int PLAYER_BIT             = 0x00000008;

    static const int MOVING_BIT             = 0x00000010;
    static const int CROUCHING_BIT          = 0x00000020;
    static const int RUNNING_BIT            = 0x00000040;
    static const int ATTACKING_BIT          = 0x00000080;

    // current state bits
    static const int JUMP_SCHED_BIT         = 0x00000100;
    // on floor or an another object
    static const int GROUNDED_BIT           = 0x00000200;
    static const int ON_STAIRS_BIT          = 0x00000400;
    // climbing a ledge or a ladder
    static const int CLIMBING_BIT           = 0x00001000;
    static const int SWIMMING_BIT           = 0x00002000;
    static const int SUBMERGED_BIT          = 0x00004000;

    static const int GESTURE_POINT_BIT      = 0x01000000;
    static const int GESTURE_BACK_BIT       = 0x02000000;
    static const int GESTURE_SALUTE_BIT     = 0x04000000;
    static const int GESTURE_WAVE_BIT       = 0x08000000;
    static const int GESTURE_FLIP_BIT       = 0x10000000;

    static const int GESTURE_MASK           = GESTURE_POINT_BIT | GESTURE_BACK_BIT |
                                              GESTURE_SALUTE_BIT | GESTURE_WAVE_BIT |
                                              GESTURE_FLIP_BIT;

    static const float AIR_FRICTION;

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
    float  step;
    float  stairRate;

    int    cargo;
    int    weapon;
    float  grabHandle;

    float  camZ;

    String name;
    String mindFunc;

  protected:

    virtual void onDestroy();
    virtual void onUpdate();

  public:

    bool hasAttribute( int attribute ) const;

    bool canReach( const Entity* ent ) const;
    bool canReach( const Object* obj ) const;

    bool invUse( const Dynamic* item, const Object* source );
    bool invTake( const Dynamic* item, const Object* source );
    bool invGive( const Dynamic* item, const Object* target );
    bool invDrop( const Dynamic* item );
    bool invGrab( const Dynamic* item );
    bool trigger( const Entity* entity );
    bool lock( const Entity* entity );
    bool use( const Object* object );
    bool take( const Dynamic* item );
    bool grab( const Dynamic* dynamic = null );
    bool rotateCargo();
    bool throwCargo();

    void heal();
    void rearm();
    void kill();

    void enter( int vehicle );
    void exit();

  public:

    explicit Bot( const BotClass* clazz, int index, const Point& p, Heading heading );
    explicit Bot( const BotClass* clazz, InputStream* istream );

    virtual void write( BufferStream* ostream ) const;

    virtual void readUpdate( InputStream* istream );
    virtual void writeUpdate( BufferStream* ostream ) const;

    OZ_STATIC_POOL_ALLOC( pool )

};

}
}
