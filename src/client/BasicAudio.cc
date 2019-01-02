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

#include <client/BasicAudio.hh>

#include <client/Context.hh>

namespace oz::client
{

Pool<BasicAudio> BasicAudio::pool(2048);

BasicAudio::BasicAudio(const Object* obj)
  : Audio(obj), eventCountdowns{}
{}

Audio* BasicAudio::create(const Object* obj)
{
  return new BasicAudio(obj);
}

void BasicAudio::play(const Object* playAt)
{
  OZ_ASSERT(playAt != nullptr);

  const auto& sounds = obj->clazz->audioSounds;

  for (int& i : eventCountdowns) {
    i = max(i - 1, 0);
  }

  // events
  for (const Object::Event& event : obj->events) {
    OZ_ASSERT(event.id < ObjectClass::MAX_SOUNDS);

    if (event.id >= 0 && sounds[event.id] != -1) {
      if (event.intensity < 0.0f) {
        playContSound(sounds[event.id], -event.intensity, playAt);
      }
      else if (eventCountdowns[event.id] == 0) {
        eventCountdowns[event.id] = COUNTDOWN_TICKS;
        playSound(sounds[event.id], event.intensity, playAt);
      }
    }
  }

  // friction
  const Dynamic* dyn = static_cast<const Dynamic*>(obj);

  if ((obj->flags & Object::DYNAMIC_BIT) && dyn->parent == -1 &&
      sounds[Object::EVENT_FRICTING] != -1)
  {
    if ((dyn->flags & (Object::FRICTING_BIT | Object::ON_SLICK_BIT)) == Object::FRICTING_BIT &&
        ((dyn->flags & Object::ON_FLOOR_BIT) || dyn->lower != -1))
    {
      eventCountdowns[Object::EVENT_FRICTING] = COUNTDOWN_TICKS;
    }

    if (eventCountdowns[Object::EVENT_FRICTING] != 0) {
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
  for (int i : obj->items) {
    const Object* item = orbis.obj(i);

    if (item != nullptr && (item->flags & Object::AUDIO_BIT)) {
      context.playAudio(item, playAt);
    }
  }
}

}
