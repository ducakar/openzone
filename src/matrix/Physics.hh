/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2019 Davorin Učakar
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

  static constexpr float FLOOR_NORMAL_Z          =  0.60f;
  static constexpr float MOVE_BOUNCE             =  EPSILON;
  static constexpr float ENTITY_BOND_G_RATIO     =  0.10f;
  static constexpr float SIDE_PUSH_RATIO         =  0.40f;

  static constexpr float HIT_MAX_MASS            =  100.0f;
  static constexpr float HIT_THRESHOLD           = -3.0f;
  static constexpr float HIT_INTENSITY_COEF      =  0.02f;
  static constexpr float HIT_ENERGY_COEF         =  0.01f;
  static constexpr float SPLASH_THRESHOLD        = -2.0f;
  static constexpr float SPLASH_INTENSITY_COEF   =  0.02f;

  static constexpr float SLIDE_DAMAGE_THRESHOLD  =  50.0f;
  static constexpr float SLIDE_DAMAGE_COEF       = -2.5f;

  static constexpr float STICK_VELOCITY          =  0.03f;
  static constexpr float SLICK_STICK_VELOCITY    =  0.003f;
  static constexpr float FLOATING_STICK_VELOCITY =  0.0005f;
  static constexpr float WATER_FRICTION          =  0.10f;
  static constexpr float LADDER_FRICTION         =  0.15f;
  static constexpr float FLOOR_FRICTION_COEF     =  0.30f;
  static constexpr float SLICK_FRICTION_COEF     =  0.03f;

  static constexpr float LAVA_LIFT               =  1.2f;
  static constexpr float LAVA_DAMAGE_ABSOLUTE    =  175.0f;
  static constexpr float LAVA_DAMAGE_RATIO       =  0.25f;
  static constexpr int   LAVA_DAMAGE_INTERVAL    =  Timer::TICKS_PER_SEC / 2;

  static constexpr float FRAG_HIT_VELOCITY2      =  100.0f;
  static constexpr float FRAG_DESTROY_VELOCITY2  =  300.0f;
  static constexpr float FRAG_DAMAGE_COEF        =  0.05f;
  static constexpr float FRAG_FIXED_DAMAGE       =  0.75f;

private:

  Dynamic* dyn;
  Frag*    frag;
  Vec3     move;
  Vec3     lastNormals[2];

public:

  float    gravity = -9.81f;

private:

  bool handleObjFriction();
  void handleObjHit();
  Vec3 handleObjMove();

  void handleFragHit();
  void handleFragMove();

public:

  void updateEnt(Entity* ent, const Vec3& localMove);
  void updateObj(Dynamic* dyn_);
  void updateFrag(Frag* frag_);

};

extern Physics physics;

}
