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

#include <client/BotAudio.hh>

#include <client/Context.hh>
#include <client/Camera.hh>

namespace oz::client
{

const float BotAudio::FOOTSTEP_DISTANCE_SQ = 32.0f*32.0f;

Pool<BotAudio> BotAudio::pool(256);

BotAudio::BotAudio(const Object* obj)
  : BasicAudio(obj), prevStep(0)
{}

Audio* BotAudio::create(const Object* obj)
{
  OZ_ASSERT(obj->flags & Object::BOT_BIT);

  return new BotAudio(obj);
}

void BotAudio::play(const Object* playAt)
{
  OZ_ASSERT(playAt != nullptr);

  const Bot*  bot    = static_cast<const Bot*>(obj);
  const auto& sounds = obj->clazz->audioSounds;

  for (int i = 0; i < ObjectClass::MAX_SOUNDS; ++i) {
    recent[i] = max(recent[i] - 1, 0);
  }

  // events
  for (const Object::Event& event : obj->events) {
    OZ_ASSERT(event.id < ObjectClass::MAX_SOUNDS);

    if (event.id >= 0 && sounds[event.id] != -1 &&
        (!(bot->state & Bot::DEAD_BIT) || event.id == Bot::EVENT_DEATH))
    {
      if (event.intensity < 0.0f) {
        playContSound(sounds[event.id], -event.intensity, playAt);
      }
      else if (recent[event.id] == 0) {
        recent[event.id] = RECENT_TICKS;
        playSound(sounds[event.id], event.intensity, playAt);
      }
    }
  }

  // inventory items' events
  for (int i = 0; i < obj->items.size(); ++i) {
    const Object* item = orbis.obj(obj->items[i]);

    if (item != nullptr && (item->flags & Object::AUDIO_BIT)) {
      context.playAudio(item, playAt);
    }
  }

  // footsteps
  if ((bot->state & (Bot::MOVING_BIT | Bot::CROUCHING_BIT)) == Bot::MOVING_BIT &&
      (bot->p - camera.p).sqN() < FOOTSTEP_DISTANCE_SQ)
  {
    if (bot->flags & Object::FRICTING_BIT) {
      recent[Object::EVENT_FRICTING] = RECENT_TICKS;
    }

    int currStep = int(2.0f * bot->step) % 2;

    if (currStep != prevStep) {
      if (bot->state & Bot::SWIMMING_BIT) {
        int sample = Bot::EVENT_SWIM_SURFACE + ((bot->state & Bot::SUBMERGED_BIT) != 0);

        if (sounds[sample] != -1) {
          playSound(sounds[sample], 1.0f, bot);
        }
      }
      else if (recent[Object::EVENT_FRICTING] != 0) {
        OZ_ASSERT(bot->depth >= 0.0f);

        int sample = bot->depth != 0.0f ? Bot::EVENT_WATER_STEP : Bot::EVENT_STEP;
        if (sounds[sample] != -1) {
          playSound(sounds[sample], 1.0f, bot);
        }
      }
    }

    prevStep = currStep;
  }
  else {
    prevStep = 0;
  }

//  if (bot->clazz->name == "bauul") {
//    playSpeak("Take my love, take my land, take me where I cannot stand.\n"
//              "I don't care, I'm still free, you can't take the sky from me.\n"
//              "Take me out to the black, tell 'em I ain't comin' back.\n"
//              "Burn the land, boil the sea, you can't take the sky from me.\n"
//              "There's no place I can be since I found Serenity.\n"
//              "And you can't take the sky from me.\n",
//              1.0f, bot);
//  }
}

}
