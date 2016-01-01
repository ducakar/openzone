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

#include <matrix/WeaponClass.hh>

#include <matrix/Weapon.hh>
#include <matrix/Liber.hh>

namespace oz
{

ObjectClass* WeaponClass::createClass()
{
  return new WeaponClass();
}

void WeaponClass::init(const Json& config, const char* name_)
{
  DynamicClass::init(config, name_);

  flags |= Object::WEAPON_BIT | Object::ITEM_BIT | Object::USE_FUNC_BIT | Object::UPDATE_FUNC_BIT |
           Object::STATUS_FUNC_BIT;

  if (audioType >= 0) {
    const Json& soundsConfig = config["audioSounds"];

    const char* sEventShotEmpty = soundsConfig["shotEmpty"].get("");
    const char* sEventShot      = soundsConfig["shot"    ].get("");

    audioSounds[Weapon::EVENT_SHOT_EMPTY] = liber.soundIndex(sEventShotEmpty);
    audioSounds[Weapon::EVENT_SHOT]       = liber.soundIndex(sEventShot);
  }

  int dollar = name.index('$');
  if (dollar < 0) {
    OZ_ERROR("%s: Weapon name should be of the form botPrefix$weaponName", name_);
  }

  userBase     = name.substring(0, dollar);

  nRounds      = config["nRounds"].get(-1);
  shotInterval = config["shotInterval"].get(0.5f);

  onShot       = config["onShot"].get("");

  if (!String::isEmpty(onShot)) {
    flags |= Object::LUA_BIT;
  }
}

Object* WeaponClass::create(int index, const Point& pos, Heading heading) const
{
  return new Weapon(this, index, pos, heading);
}

Object* WeaponClass::create(int index, const Json& json) const
{
  return new Weapon(this, index, json);
}

Object* WeaponClass::create(Stream* is) const
{
  return new Weapon(this, is);
}

}
