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
 * @file client/BasicAudio.cc
 */

#include <client/BasicAudio.hh>

#include <client/Context.hh>

namespace oz
{
namespace client
{

Pool<BasicAudio> BasicAudio::pool(2048);

BasicAudio::BasicAudio(const Object* obj) :
  Audio(obj), recent{}
{}

Audio* BasicAudio::create(const Object* obj)
{
  return new BasicAudio(obj);
}

void BasicAudio::play(const Object* playAt)
{
  hard_assert(playAt != nullptr);

  const auto& sounds = obj->clazz->audioSounds;

  for (int i = 0; i < ObjectClass::MAX_SOUNDS; ++i) {
    recent[i] = max(recent[i] - 1, 0);
  }

  // events
  for (const Object::Event& event : obj->events) {
    hard_assert(event.id < ObjectClass::MAX_SOUNDS);

    if (event.id >= 0 && sounds[event.id] >= 0) {
      if (event.intensity < 0.0f) {
        playContSound(sounds[event.id], -event.intensity, playAt);
      }
      else if (recent[event.id] == 0) {
        recent[event.id] = RECENT_TICKS;
        playSound(sounds[event.id], event.intensity, playAt);
      }
    }
  }

  // friction
  const Dynamic* dyn = static_cast<const Dynamic*>(obj);

  if ((obj->flags & Object::DYNAMIC_BIT) && dyn->parent < 0 &&
      sounds[Object::EVENT_FRICTING] >= 0)
  {
    if ((dyn->flags & (Object::FRICTING_BIT | Object::ON_SLICK_BIT)) == Object::FRICTING_BIT &&
        ((dyn->flags & Object::ON_FLOOR_BIT) || dyn->lower >= 0))
    {
      recent[Object::EVENT_FRICTING] = RECENT_TICKS;
    }

    if (recent[Object::EVENT_FRICTING] != 0) {
      float dvx = dyn->velocity.x;
      float dvy = dyn->velocity.y;

      const Dynamic* sDyn = orbis.obj<const Dynamic>(dyn->lower);

      if (sDyn != nullptr) {
        dvx -= sDyn->velocity.x;
        dvy -= sDyn->velocity.y;
      }

      playContSound(sounds[Object::EVENT_FRICTING], Math::sqrt(dvx*dvx + dvy*dvy), dyn);
    }
  }

  // inventory items' events
  for (int i = 0; i < obj->items.length(); ++i) {
    const Object* item = orbis.obj(obj->items[i]);

    if (item != nullptr && (item->flags & Object::AUDIO_BIT)) {
      context.playAudio(item, playAt);
    }
  }
}

}
}
