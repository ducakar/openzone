/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2016 Davorin Učakar
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
 * @file matrix/Vehicle.hh
 */

#pragma once

#include <matrix/Bot.hh>
#include <matrix/VehicleClass.hh>

namespace oz
{

class Vehicle : public Dynamic
{
public:

  static constexpr int   MAX_WEAPONS        = 4;

  // not in use, used to reserve a slot for engine sound sample
  static constexpr int   EVENT_ENGINE       = 9;
  static constexpr int   EVENT_SHOT0        = 10;
  static constexpr int   EVENT_SHOT1        = 11;
  static constexpr int   EVENT_SHOT2        = 12;
  static constexpr int   EVENT_SHOT3        = 13;
  static constexpr int   EVENT_SHOT_EMPTY   = 14;

  static constexpr int   CREW_VISIBLE_BIT   = 0x0001;
  static constexpr int   HAS_EJECT_BIT      = 0x0004;
  static constexpr int   AUTO_EJECT_BIT     = 0x0008;

  static constexpr int   MOVING_BIT         = 0x0010;
  static constexpr int   WALKING_BIT        = 0x0020;

private:

  static constexpr float ROT_DIFF_LIMIT     = 0.50f;
  static constexpr float ROT_VEL_DIFF_RATIO = 0.10f;
  static constexpr float AIR_FRICTION       = 0.02f;
  static constexpr float EXIT_EPSILON       = 0.20f;
  static constexpr float EXIT_MOMENTUM      = 1.00f;
  static constexpr float EJECT_EPSILON      = 0.80f;
  static constexpr float EJECT_MOMENTUM     = 15.0f;

  using Handler = void (Vehicle::*)();

  static const Handler HANDLERS[];

public:

  static Pool<Vehicle> pool;

  float h, v, w;
  float rotVelH, rotVelV;
  int   actions, oldActions;

  Mat4  rot;
  int   state, oldState;
  float fuel;

  float step;
  float stairRate;

  int   pilot;

  int   weapon;
  int   nRounds[MAX_WEAPONS];
  float shotTime[MAX_WEAPONS];

public:

  void exit();
  void eject();
  void service();

  void staticHandler();
  void wheeledHandler();
  void trackedHandler();
  void mechHandler();
  void hoverHandler();
  void airHandler();

protected:

  void onDestroy() override;
  bool onUse(Bot* user) override;
  void onUpdate() override;
  float getStatus() const override;

public:

  explicit Vehicle(const VehicleClass* clazz_, int index_, const Point& p_, Heading heading);
  explicit Vehicle(const VehicleClass* clazz_, int index, const Json& json);
  explicit Vehicle(const VehicleClass* clazz_, Stream* is);

  Json write() const override;
  void write(Stream* os) const override;

  void readUpdate(Stream* is) override;
  void writeUpdate(Stream* os) const override;

  OZ_STATIC_POOL_ALLOC(pool)
};

}
