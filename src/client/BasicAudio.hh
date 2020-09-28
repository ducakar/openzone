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
 * @file client/BasicAudio.hh
 */

#pragma once

#include <client/Audio.hh>

namespace oz::client
{

class BasicAudio : public Audio
{
protected:

  static constexpr uint COUNTDOWN_TICKS = Timer::TICKS_PER_SEC / 12;

  // Recent event countdowns. This is to prevent event spawning rafales of same event sounds when
  // some event is occurring every few frames. The same event sound can only be triggered every
  // COUNTDOWN_TICKS on the same Audio.
  // The other usage is for friction. This is a continuous sound, but friction will likely not occur
  // on every tick (because of bouncing away on hits, when moving downhill ...), so friction sound
  // would keep start/stop/restart-ing every few frames. This is is compensated by the countdowns,
  // so friction sound only stops when not detected for COUNTDOWN_TICKS.
  int eventCountdowns[ObjectClass::MAX_SOUNDS];

  explicit BasicAudio(const Object* obj_);

public:

  static Pool<BasicAudio> pool;

  static Audio* create(const Object* obj_);

  void play(const Object* playAt) override;

  OZ_STATIC_POOL_ALLOC(pool)
};

}
