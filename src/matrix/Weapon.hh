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
 * @file matrix/Weapon.hh
 */

#pragma once

#include <matrix/Dynamic.hh>
#include <matrix/WeaponClass.hh>

namespace oz
{

class Weapon : public Dynamic
{
public:

  static constexpr int EVENT_SHOT_EMPTY = 9;
  static constexpr int EVENT_SHOT       = 10;

  static Pool<Weapon> pool;

  // -1: unlimited
  int   nRounds;
  float shotTime;

  void trigger(Bot* user);

protected:

  bool onUse(Bot* user) override;
  void onUpdate() override;
  float getStatus() const override;

public:

  explicit Weapon(const WeaponClass* clazz_, int index_, const Point& p_, Heading heading);
  explicit Weapon(const WeaponClass* clazz_, int index_, const Json& json);
  explicit Weapon(const WeaponClass* clazz_, Stream* is);

  Json write() const override;
  void write(Stream* os) const override;

  void readUpdate(Stream* is) override;
  void writeUpdate(Stream* os) const override;

  OZ_STATIC_POOL_ALLOC(pool)
};

}
