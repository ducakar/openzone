/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
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
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file matrix/Physics.hh
 *
 * Physics engine.
 */

#pragma once

#include "matrix/Collider.hh"

namespace oz
{
namespace matrix
{

class Physics
{
  public:

    static constexpr float MOVE_BOUNCE             =  1.5f * EPSILON;
    static constexpr float HIT_THRESHOLD           = -3.0f;
    static constexpr float SPLASH_THRESHOLD        = -2.0f;
    static constexpr float FLOOR_NORMAL_Z          =  0.60f;
    static constexpr float SIDE_PUSH_RATIO         =  0.5f;
    static constexpr float WEIGHT_DAMAGE_THRESHOLD =  1000.0f;
    static constexpr float WEIGHT_DAMAGE_FACTOR    =  20.0f;
    static constexpr float G_ACCEL                 = -9.81f;

    static constexpr float SLIDE_DAMAGE_THRESHOLD  =  25.0f;
    static constexpr float SLIDE_DAMAGE_COEF       =  0.65f;
    static constexpr float STICK_VELOCITY          =  0.015f;
    static constexpr float SLICK_STICK_VELOCITY    =  0.001f;
    static constexpr float FLOAT_STICK_VELOCITY    =  0.0002f;
    static constexpr float WATER_FRICTION          =  0.08f;
    static constexpr float LADDER_FRICTION         =  0.50f;
    static constexpr float FLOOR_FRICTION          =  0.25f;
    static constexpr float SLICK_FRICTION          =  0.02f;

    static constexpr float FRAG_HIT_VELOCITY2      =  100.0f;
    static constexpr float FRAG_DESTROY_VELOCITY2  =  300.0f;
    static constexpr float FRAG_STR_DAMAGE_COEF    =  0.05f;
    static constexpr float FRAG_OBJ_DAMAGE_COEF    =  0.05f;
    static constexpr float FRAG_FIXED_DAMAGE       =  0.75f;

  private:

    Dynamic* dyn;
    Frag*    frag;
    Bounds   trace;
    Vec3     move;
    float    leftRatio;

    Vec3     lastNormals[2];

    void handleFragHit();
    void handleFragMove();

    bool handleObjFriction();
    void handleObjHit();
    void handleObjMove();

  public:

    void updateFrag( Frag* frag );
    void updateObj( Dynamic* dyn );

};

extern Physics physics;

}
}
