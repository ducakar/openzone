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
 * @file matrix/Physics.hpp
 *
 * Physics engine.
 */

#pragma once

#include "matrix/Orbis.hpp"

namespace oz
{
namespace matrix
{

class Physics
{
  public:

    static const float MOVE_BOUNCE;
    static const float HIT_THRESHOLD;
    static const float SPLASH_THRESHOLD;
    static const float FLOOR_NORMAL_Z;
    static const float SIDE_PUSH_RATIO;
    static const float WEIGHT_FACTOR;
    static const float G_ACCEL;

    static const float STICK_VELOCITY;
    static const float SLICK_STICK_VELOCITY;
    static const float FLOAT_STICK_VELOCITY;
    static const float WATER_FRICTION;
    static const float LADDER_FRICTION;
    static const float FLOOR_FRICTION;
    static const float SLICK_FRICTION;

    static const float STRUCT_HIT_MAX_MASS;
    static const float STRUCT_HIT_RATIO;

    static const float PART_HIT_VELOCITY2;
    static const float PART_DESTROY_VELOCITY2;

  private:

    Dynamic*  dyn;
    Particle* part;
    Bounds    trace;
    Vec3      move;
    float     leftRatio;

    Vec3      lastNormals[2];

    void handlePartHit();
    void handlePartMove();

    bool handleObjFriction();
    void handleObjHit();
    void handleObjMove();

  public:

    void updatePart( Particle* part );
    void updateObj( Dynamic* dyn );

};

extern Physics physics;

}
}
