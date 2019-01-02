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
 * @file matrix/BotClass.hh
 */

#pragma once

#include <matrix/DynamicClass.hh>

namespace oz
{

class BotClass : public DynamicClass
{
public:

  static constexpr Mat4 INJURY_COLOUR = Mat4(1.00f, 0.00f, 0.00f, 0.00f,
                                             1.00f, 0.00f, 0.00f, 0.00f,
                                             1.00f, 0.00f, 0.00f, 0.00f,
                                             0.20f, 0.05f, 0.05f, 1.00f);

  static constexpr Mat4 NV_COLOUR     = Mat4(0.25f, 2.00f, 0.25f, 0.00f,
                                             0.25f, 2.00f, 0.25f, 0.00f,
                                             0.25f, 2.00f, 0.25f, 0.00f,
                                             0.00f, 0.00f, 0.00f, 1.00f);

  int    state;

  Vec3   crouchDim;
  Vec3   corpseDim;

  float  camZ;
  float  crouchCamZ;

  float  walkMomentum;
  float  runMomentum;
  float  jumpMomentum;

  float  airControl;
  float  ladderControl;
  float  waterControl;
  float  slickControl;

  float  stepWalkInc;
  float  stepRunInc;

  float  stairInc;
  float  stairMax;
  float  stairRateLimit;
  float  stairRateSupp;

  float  climbInc;
  float  climbMax;
  float  climbMomentum;

  float  stamina;
  float  staminaGain;
  float  staminaRunDrain;
  float  staminaWaterDrain;
  float  staminaClimbDrain;
  float  staminaJumpDrain;
  float  staminaThrowDrain;

  float  regeneration;

  float  reachDist;
  float  grabWeight;
  float  throwMomentum;

  int    weaponItem;
  float  meleeInterval;
  String onMelee;

  String nameFunc;
  String mind;
  int    mindAutomaton;

  float  bobRotation;
  float  bobAmplitude;
  float  bobSwimAmplitude;

  Mat4   baseColour;
  Mat4   injuryColour;
  Mat4   nvColour;

public:

  static ObjectClass* createClass();

  void init(const Json& config, const char* name_) override;

  Object* create(int index, const Point& pos, Heading heading) const override;
  Object* create(int index, const Json& json) const override;
  Object* create(Stream* is) const override;

};

}
