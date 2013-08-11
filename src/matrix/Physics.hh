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
 * @file matrix/Physics.hh
 *
 * Physics engine.
 */

#pragma once

#include <matrix/Collider.hh>

namespace oz
{

class Physics
{
  public:

    static const float FLOOR_NORMAL_Z;
    static const float MOVE_BOUNCE;
    static const float MAX_HIT_MASS;
    static const float ENTITY_BOND_G_RATIO;
    static const float SIDE_PUSH_RATIO;

    static const float HIT_THRESHOLD;
    static const float HIT_INTENSITY_COEF;
    static const float HIT_ENERGY_COEF;
    static const float SPLASH_THRESHOLD;
    static const float SPLASH_INTENSITY_COEF;

    static const float WEIGHT_DAMAGE_THRESHOLD;
    static const float WEIGHT_DAMAGE_FACTOR;
    static const float SLIDE_DAMAGE_THRESHOLD;
    static const float SLIDE_DAMAGE_COEF;

    static const float STICK_VELOCITY;
    static const float SLICK_STICK_VELOCITY;
    static const float FLOATING_STICK_VELOCITY;
    static const float WATER_FRICTION;
    static const float LADDER_FRICTION;
    static const float FLOOR_FRICTION_COEF;
    static const float SLICK_FRICTION_COEF;

    static const float LAVA_LIFT;
    static const float LAVA_DAMAGE_ABSOLUTE;
    static const float LAVA_DAMAGE_RATIO;
    static const int   LAVA_DAMAGE_INTERVAL;

    static const float FRAG_HIT_VELOCITY2;
    static const float FRAG_DESTROY_VELOCITY2;
    static const float FRAG_DAMAGE_COEF;
    static const float FRAG_FIXED_DAMAGE;

  private:

    Dynamic* dyn;
    Frag*    frag;
    Vec3     move;
    Vec3     lastNormals[2];

  public:

    float    gravity;

  private:

    void handleFragHit();
    void handleFragMove();

    bool handleObjFriction();
    void handleObjHit();
    Vec3 handleObjMove();

  public:

    void updateFrag( Frag* frag );
    void updateObj( Dynamic* dyn );

};

extern Physics physics;

}
