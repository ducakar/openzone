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

#include <matrix/Weapon.hh>

#include <matrix/Bot.hh>
#include <matrix/LuaMatrix.hh>
#include <matrix/Synapse.hh>

namespace oz
{

Pool<Weapon> Weapon::pool(2048);

bool Weapon::onUse(Bot* user)
{
  OZ_ASSERT(user->canEquip(this));

  if (parent == user->index) {
    user->weapon = user->weapon == index ? -1 : index;
    return true;
  }
  else if (user->items.length() < user->clazz->nItems) {
    user->items.add(index);
    user->weapon = index;

    if (parent < 0) {
      parent = user->index;
      synapse.cut(this);

      user->addEvent(Bot::EVENT_TAKE, 1.0f);
    }
    else {
      Object* container = orbis.obj(parent);

      OZ_ASSERT(container->items.contains(index));

      parent = user->index;
      container->items.exclude(index);
    }
    return true;
  }
  return false;
}

void Weapon::onUpdate()
{
  if (shotTime > 0.0f) {
    shotTime = max(shotTime - Timer::TICK_TIME, 0.0f);
  }

  if ((flags & LUA_BIT) && !clazz->onUpdate.isEmpty()) {
    luaMatrix.objectCall(clazz->onUpdate, this);
  }

  if (!(flags & Object::UPDATE_FUNC_BIT)) {
    // actually a hack, if Lua handler disables update
    shotTime = 0.0f;
  }
}

float Weapon::getStatus() const
{
  const WeaponClass* clazz = static_cast<const WeaponClass*>(this->clazz);

  return float(nRounds) / float(clazz->nRounds);
}

void Weapon::trigger(Bot* user)
{
  OZ_ASSERT(user != nullptr);

  const WeaponClass* clazz = static_cast<const WeaponClass*>(this->clazz);

  if (shotTime == 0.0f) {
    bool success = false;

    shotTime = clazz->shotInterval;

    if (nRounds != 0 && luaMatrix.objectCall(clazz->onShot, this, user)) {
      nRounds = max(-1, nRounds - 1);
      success = true;
    }
    addEvent(EVENT_SHOT_EMPTY + success, 1.0f);
  }
}

Weapon::Weapon(const WeaponClass* clazz_, int index_, const Point& p_, Heading heading) :
  Dynamic(clazz_, index_, p_, heading)
{
  nRounds  = clazz_->nRounds;
  shotTime = 0.0f;
}

Weapon::Weapon(const WeaponClass* clazz_, int index, const Json& json) :
  Dynamic(clazz_, index, json)
{
  nRounds  = json["nRounds"].get(0);
  shotTime = 0.0f;
}

Weapon::Weapon(const WeaponClass* clazz_, Stream* is) :
  Dynamic(clazz_, is)
{
  nRounds  = is->readInt();
  shotTime = is->readFloat();
}

Json Weapon::write() const
{
  Json json = Dynamic::write();

  json.add("nRounds", nRounds);

  return json;
}

void Weapon::write(Stream* os) const
{
  Dynamic::write(os);

  os->writeInt(nRounds);
  os->writeFloat(shotTime);
}

void Weapon::readUpdate(Stream*)
{}

void Weapon::writeUpdate(Stream*) const
{}

}
