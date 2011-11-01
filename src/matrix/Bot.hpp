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
 * @file matrix/Bot.hpp
 */

#pragma once

#include "matrix/Library.hpp"
#include "matrix/Dynamic.hpp"
#include "matrix/Weapon.hpp"

namespace oz
{
namespace matrix
{

class Mind;

class Bot : public Dynamic
{
  public:

    static const int ACTION_FORWARD           = 0x00000001;
    static const int ACTION_BACKWARD          = 0x00000002;
    static const int ACTION_LEFT              = 0x00000004;
    static const int ACTION_RIGHT             = 0x00000008;
    static const int ACTION_JUMP              = 0x00000010;
    static const int ACTION_CROUCH            = 0x00000020;

    static const int ACTION_USE               = 0x00000040;
    static const int ACTION_TAKE              = 0x00000080;
    static const int ACTION_GRAB              = 0x00000100;
    static const int ACTION_THROW             = 0x00000200;

    static const int ACTION_ATTACK            = 0x00000400;
    static const int ACTION_EXIT              = 0x00000800;
    static const int ACTION_EJECT             = 0x00001000;
    static const int ACTION_SUICIDE           = 0x00002000;

    static const int ACTION_INV_USE           = 0x00004000;
    static const int ACTION_INV_DROP          = 0x00008000;
    static const int ACTION_INV_GRAB          = 0x00010000;
    static const int ACTION_INV_TAKE          = 0x00020000;
    static const int ACTION_INV_GIVE          = 0x00040000;

    static const int ACTION_VEH_UP            = 0x00100000;
    static const int ACTION_VEH_DOWN          = 0x00200000;
    static const int ACTION_VEH_NEXT_WEAPON   = 0x00400000;

    static const int EVENT_HIT_HARD           = 7;
    static const int EVENT_LAND               = 8;
    static const int EVENT_JUMP               = 9;
    static const int EVENT_FLIP               = 10;
    static const int EVENT_DEATH              = 11;

    static const int DEAD_BIT                 = 0x00000001;
    static const int MECHANICAL_BIT           = 0x00000002;
    static const int INCARNATABLE_BIT         = 0x00000004;
    // bot is currently controlled by player, nirvana shouldn't process its mind
    static const int PLAYER_BIT               = 0x00000008;

    // can step over obstacles, e.g. walk up the stairs
    static const int STEPABLE_BIT             = 0x00000100;
    static const int CROUCHING_BIT            = 0x00000200;
    static const int RUNNING_BIT              = 0x00000400;
    static const int SHOOTING_BIT             = 0x00000800;
    static const int MOVING_BIT               = 0x00001000;
    static const int GRAB_BIT                 = 0x00002000;

    // current state bits
    static const int JUMP_SCHED_BIT           = 0x00004000;
    static const int GROUNDED_BIT             = 0x00008000;
    static const int ON_STAIRS_BIT            = 0x00010000;
    static const int CLIMBING_BIT             = 0x00020000;
    static const int SWIMMING_BIT             = 0x00040000;
    static const int SUBMERGED_BIT            = 0x00080000;

    static const int GESTURE0_BIT             = 0x01000000;
    static const int GESTURE1_BIT             = 0x02000000;
    static const int GESTURE2_BIT             = 0x04000000;
    static const int GESTURE3_BIT             = 0x08000000;
    static const int GESTURE4_BIT             = 0x10000000;
    static const int GESTURE_ALL_BIT          = 0x20000000;

    static const float HIT_HARD_THRESHOLD;
    static const float WOUNDED_THRESHOLD;
    static const float INSTRUMENT_DIST_MAX;
    static const float INSTRUMENT_DOT_MIN;
    static const float GRAB_EPSILON;
    static const float GRAB_STRING_RATIO;
    static const float GRAB_HANDLE_TOL;
    static const float GRAB_MOM_RATIO;
    static const float GRAB_MOM_MAX;
    static const float GRAB_MOM_MAX_SQ;
    static const float STEP_MOVE_AHEAD;
    static const float CLIMB_MOVE_AHEAD;
    static const float CORPSE_FADE_FACTOR;

    static Pool<Bot, 1024> pool;

    Object* getTagged( float* hvsc, int mask = Object::SOLID_BIT ) const;

  protected:

    virtual void onDestroy();
    virtual void onHit( const Hit* hit, float hitMomentum );
    virtual void onUpdate();

  public:

    float      h, v;
    int        state, oldState;
    int        actions, oldActions;

    float      stamina;
    float      stepRate;

    int        instrument;
    float      grabHandle;
    int        weapon;
    int        taggedItem;

    String     name;
    String     mindFunc;

    float      camZ;
    Anim::Type anim;

    Bot();

    void heal();
    void rearm();
    void kill();

    void enter( int vehicle );
    void exit();

    virtual void readFull( InputStream* istream );
    virtual void writeFull( OutputStream* ostream ) const;
    virtual void readUpdate( InputStream* istream );
    virtual void writeUpdate( OutputStream* ostream ) const;

  OZ_STATIC_POOL_ALLOC( pool )

};

}
}
