/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file matrix/VehicleClass.hh
 */

#pragma once

#include <matrix/DynamicClass.hh>

namespace oz
{

class VehicleClass : public DynamicClass
{
public:

  static const int MAX_WEAPONS = 4;

  enum Type
  {
    TURRET,
    WHEELED,
    TRACKED,
    MECH,
    HOVER,
    AIR,
    SUB
  };

  struct EngineSettings
  {
    float consumption;
    float idleConsumption;

    float pitchBias;
    float pitchRatio;
    float pitchLimit;
  };

  struct TurretSettings
  {};

  struct WheeledSettings
  {
    float moveMomentum;
  };

  struct TrackedSettings
  {
    float moveMomentum;
  };

  struct MechSettings
  {
    float walkMomentum;
    float runMomentum;

    float stepWalkInc;
    float stepRunInc;

    float stairInc;
    float stairMax;
    float stairRateLimit;
    float stairRateSupp;
  };

  struct HoverSettings
  {
    float moveMomentum;

    float height;
    float heightStiffness;
    float momentumStiffness;
  };

  struct AirSettings
  {
    float moveMomentum;
  };

  struct SubSettings
  {
    float moveMomentum;
  };

  Type   type;
  int    state;
  float  fuel;

  Vec3   pilotPos;

  float  lookHMin;
  float  lookHMax;
  float  lookVMin;
  float  lookVMax;

  float  rotVelLimit;

  int    nWeapons;
  String weaponTitles[MAX_WEAPONS];
  String onWeaponShot[MAX_WEAPONS];
  int    nWeaponRounds[MAX_WEAPONS];
  float  weaponShotIntervals[MAX_WEAPONS];

  EngineSettings engine;

  union
  {
    TurretSettings  turret;
    WheeledSettings wheeled;
    TrackedSettings tracked;
    MechSettings    mech;
    HoverSettings   hover;
    AirSettings     air;
    SubSettings     sub;
  };

public:

  static ObjectClass* createClass();

  void init(const JSON& config, const char* name) override;

  Object* create(int index, const Point& pos, Heading heading) const override;
  Object* create(int index, const JSON& json) const override;
  Object* create(InputStream* is) const override;

};

}
