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
 * @file matrix/VehicleClass.cc
 */

#include <matrix/VehicleClass.hh>

#include <common/Timer.hh>
#include <common/Lingua.hh>
#include <matrix/Vehicle.hh>
#include <matrix/Liber.hh>

namespace oz
{

ObjectClass* VehicleClass::createClass()
{
  return new VehicleClass();
}

void VehicleClass::init(const Json& config, const char* name_)
{
  DynamicClass::init(config, name_);

  flags |= Object::VEHICLE_BIT | Object::CYLINDER_BIT | Object::USE_FUNC_BIT |
           Object::UPDATE_FUNC_BIT | Object::STATUS_FUNC_BIT;

  if (audioType >= 0) {
    const Json& soundsConfig = config["audioSounds"];

    const char* sEventEngine    = soundsConfig["engine"   ].get("");
    const char* sEventShot0     = soundsConfig["shot0"    ].get("");
    const char* sEventShot1     = soundsConfig["shot1"    ].get("");
    const char* sEventShot2     = soundsConfig["shot2"    ].get("");
    const char* sEventShot3     = soundsConfig["shot3"    ].get("");
    const char* sEventShotEmpty = soundsConfig["shotEmpty"].get("");

    audioSounds[Vehicle::EVENT_ENGINE]     = liber.soundIndex(sEventEngine);
    audioSounds[Vehicle::EVENT_SHOT0]      = liber.soundIndex(sEventShot0);
    audioSounds[Vehicle::EVENT_SHOT1]      = liber.soundIndex(sEventShot1);
    audioSounds[Vehicle::EVENT_SHOT2]      = liber.soundIndex(sEventShot2);
    audioSounds[Vehicle::EVENT_SHOT3]      = liber.soundIndex(sEventShot3);
    audioSounds[Vehicle::EVENT_SHOT_EMPTY] = liber.soundIndex(sEventShotEmpty);
  }

  state = 0;

  OZ_CLASS_STATE(Vehicle::CREW_VISIBLE_BIT, "state.crewVisible", false);
  OZ_CLASS_STATE(Vehicle::HAS_EJECT_BIT,    "state.hasEject",    false);
  OZ_CLASS_STATE(Vehicle::AUTO_EJECT_BIT,   "state.autoEject",   false);

  if ((state & Vehicle::AUTO_EJECT_BIT) && !(state & Vehicle::HAS_EJECT_BIT)) {
    OZ_ERROR("%s: Vehicle has state.autoEject but not state.hasEject.", name_);
  }

  bool isTypeDetermined = false;

  if (config.contains("turret")) {
    type = VehicleClass::TURRET;
    isTypeDetermined = true;
  }
  else if (!isTypeDetermined && config.contains("wheeled")) {
    type = VehicleClass::WHEELED;
    isTypeDetermined = true;
  }
  else if (!isTypeDetermined && config.contains("tracked")) {
    type = VehicleClass::TRACKED;
    isTypeDetermined = true;
  }
  else if (!isTypeDetermined && config.contains("mech")) {
    type = VehicleClass::MECH;
    isTypeDetermined = true;
  }
  else if (!isTypeDetermined && config.contains("hover")) {
    type = VehicleClass::HOVER;
    isTypeDetermined = true;
  }
  else if (!isTypeDetermined && config.contains("air")) {
    type = VehicleClass::AIR;
    isTypeDetermined = true;
  }
  else if (!isTypeDetermined && config.contains("sub")) {
    type = VehicleClass::SUB;
    isTypeDetermined = true;
  }

  if (!isTypeDetermined) {
    OZ_ERROR("%s: Vehicle configuration must contain exactly one of the objects 'turret',"
             " 'wheeled', 'tracked', 'mech', 'hover', 'air', 'sub' that contains configuration for"
             " the selected vehicle type.", name_);
  }

  fuel     = config["fuel"].get(100.0f);

  pilotPos = config["pilotPos"].get(Vec3::ZERO);
  lookHMin = config["lookHMin"].get(-120.0f);
  lookHMax = config["lookHMax"].get(+120.0f);
  lookVMin = config["lookVMin"].get(-60.0f);
  lookVMax = config["lookVMax"].get(+60.0f);

  if (lookHMin < -180.0f || lookHMin > 0.0f) {
    OZ_ERROR("%s: lookHMin must lie on interval [-180.0, 0.0]", name_);
  }
  if (lookHMax < 0.0f || lookHMax > 180.0f) {
    OZ_ERROR("%s: lookHMax must lie on interval [0.0, 180.0]", name_);
  }
  if (lookVMin < -90.0f || lookVMin > 90.0f || lookVMin > lookVMax) {
    OZ_ERROR("%s: lookVMin must lie on interval [-90.0, 90.0] and must not be greater than"
             " lookVMax", name_);
  }
  if (lookVMax < -90.0f || lookVMax > 90.0f) {
    OZ_ERROR("%s: lookVMax must lie on interval [-90.0, 90.0] and must not be less than lookVMin",
             name_);
  }

  lookHMin = Math::rad(lookHMin);
  lookHMax = Math::rad(lookHMax);
  lookVMin = Math::rad(lookVMin);
  lookVMax = Math::rad(lookVMax);

  rotVelLimit = Math::rad(config["rotVelLimit"].get(60.0f)) * Timer::TICK_TIME;

  if (fuel <= 0.0f || !Math::isFinite(fuel)) {
    OZ_ERROR("%s: Invalid fuel value. Should be > 0 and finite. If you want infinite fuel rather"
             " set fuelConsumption to zero.", name_);
  }

  const Json& weaponsConfig = config["weapons"];
  nWeapons = max(weaponsConfig.length(), 0);

  if (nWeapons > VehicleClass::MAX_WEAPONS) {
    OZ_ERROR("%s: Vehicle must have between 0 and %d weapons.", name_, VehicleClass::MAX_WEAPONS);
  }

  for (int i = 0; i < VehicleClass::MAX_WEAPONS; ++i) {
    weaponTitles[i]        = "";
    onWeaponShot[i]        = "";
    nWeaponRounds[i]       = 0;
    weaponShotIntervals[i] = 0.0f;
  }

  for (int i = 0; i < nWeapons; ++i) {
    weaponTitles[i]        = lingua.get(weaponsConfig[i]["title"].get(""));
    onWeaponShot[i]        = weaponsConfig[i]["onShot"].get("");
    nWeaponRounds[i]       = weaponsConfig[i]["nRounds"].get(-1);
    weaponShotIntervals[i] = weaponsConfig[i]["shotInterval"].get(0.5f);

    if (weaponTitles[i].isEmpty()) {
      OZ_ERROR("%s: Missing weapon #%d title.", name_, i);
    }
    if (onWeaponShot[i].isEmpty()) {
      OZ_ERROR("%s: Missing weapon #%d handler function.", name_, i);
    }

    flags |= Object::LUA_BIT;
  }

  const Json& engineConfig = config["engine"];

  engine.consumption     = engineConfig["consumption"].get(0.0f) * Timer::TICK_TIME;
  engine.idleConsumption = engineConfig["idleConsumption"].get(0.0f) * Timer::TICK_TIME;
  engine.pitchBias       = engineConfig["pitchBias"].get(1.0f);
  engine.pitchRatio      = engineConfig["pitchRatio"].get(0.001f);
  engine.pitchLimit      = engineConfig["pitchLimit"].get(2.00f);

  switch (type) {
    case VehicleClass::TURRET: {
      break;
    }
    case VehicleClass::WHEELED: {
      const Json& wheeledConfig = config["wheeled"];

      wheeled.moveMomentum = wheeledConfig["moveMomentum"].get(2.0f);
      break;
    }
    case VehicleClass::TRACKED: {
      const Json& trackedConfig = config["tracked"];

      tracked.moveMomentum = trackedConfig["moveMomentum"].get(2.0f);
      break;
    }
    case VehicleClass::MECH: {
      const Json& mechConfig = config["mech"];

      mech.walkMomentum   = mechConfig["walkMomentum"].get(1.0f);
      mech.runMomentum    = mechConfig["runMomentum"].get(2.0f);

      mech.stepWalkInc    = mechConfig["stepWalkInc"].get(6.0f / 6.0f) * Timer::TICK_TIME;
      mech.stepRunInc     = mechConfig["stepRunInc"].get(10.0f / 6.0f) * Timer::TICK_TIME;

      mech.stairInc       = mechConfig["stairInc"].get(11.0f / 64.0f);
      mech.stairMax       = mechConfig["stairMax"].get(22.0f / 64.0f);
      mech.stairRateLimit = mechConfig["stairRateLimit"].get(0.15f);
      mech.stairRateSupp  = mechConfig["stairRateSupp"].get(0.80f);
      break;
    }
    case VehicleClass::HOVER: {
      const Json& hoverConfig = config["hover"];

      hover.moveMomentum      = hoverConfig["moveMomentum"].get(2.0f);
      hover.height            = hoverConfig["height"].get(2.0f);
      hover.heightStiffness   = hoverConfig["heightStiffness"].get(40.0f);
      hover.momentumStiffness = hoverConfig["momentumStiffness"].get(160.0f);
      break;
    }
    case VehicleClass::AIR: {
      const Json& airConfig = config["air"];

      air.moveMomentum = airConfig["moveMomentum"].get(2.0f);
      break;
    }
    case VehicleClass::SUB: {
      const Json& subConfig = config["sub"];

      sub.moveMomentum = subConfig["moveMomentum"].get(2.0f);
      break;
    }
  }
}

Object* VehicleClass::create(int index, const Point& pos, Heading heading) const
{
  return new Vehicle(this, index, pos, heading);
}

Object* VehicleClass::create(int index, const Json& json) const
{
  return new Vehicle(this, index, json);
}

Object* VehicleClass::create(InputStream* is) const
{
  return new Vehicle(this, is);
}

}
