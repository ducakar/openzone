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
 * @file matrix/DynamicClass.cc
 */

#include <matrix/DynamicClass.hh>

#include <matrix/Dynamic.hh>
#include <matrix/Liber.hh>

namespace oz
{

ObjectClass* DynamicClass::createClass()
{
  return new DynamicClass();
}

void DynamicClass::init(const JSON& config, const char* name_)
{
  ObjectClass::init(config, name_);

  flags |= Object::DYNAMIC_BIT;

  OZ_CLASS_FLAG(Object::ITEM_BIT, "flag.item", false);

  if ((flags & Object::ITEM_BIT) && nItems != 0) {
    OZ_ERROR("%s: Items cannot have their own inventory. nItems should be 0.", name_);
  }

  if (audioType >= 0) {
    const JSON& soundsConfig = config["audioSounds"];

    const char* sEventLand     = soundsConfig["land"    ].get("");
    const char* sEventSplash   = soundsConfig["splash"  ].get("");
    const char* sEventFricting = soundsConfig["fricting"].get("");

    audioSounds[Object::EVENT_LAND]     = liber.soundIndex(sEventLand    );
    audioSounds[Object::EVENT_SPLASH]   = liber.soundIndex(sEventSplash  );
    audioSounds[Object::EVENT_FRICTING] = liber.soundIndex(sEventFricting);
  }

  mass = config["mass"].get(0.0f);
  lift = config["lift"].get(-1.0f);

  if (mass < 1.0f) {
    OZ_ERROR("%s: Invalid object mass. Should be >= 1 kg.", name_);
  }
  if (lift < 0.0f) {
    OZ_ERROR("%s: Invalid object lift. Should be >= 0.", name_);
  }
}

Object* DynamicClass::create(int index, const Point& pos, Heading heading) const
{
  return new Dynamic(this, index, pos, heading);
}

Object* DynamicClass::create(int index, const JSON& json) const
{
  return new Dynamic(this, index, json);
}

Object* DynamicClass::create(InputStream* is) const
{
  return new Dynamic(this, is);
}

}
