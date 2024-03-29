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
 * @file client/Audio.hh
 *
 * Audio class.
 */

#pragma once

#include <client/common.hh>

namespace oz::client
{

class Audio
{
public:

  static constexpr int   UPDATED_BIT          = 0x00000001;

  static constexpr float ROLLOFF_FACTOR       = 0.50f;
  static constexpr float COCKPIT_GAIN_FACTOR  = 0.35f;
  static constexpr float COCKPIT_PITCH_FACTOR = 0.95f;

public:

  using CreateFunc = Audio* (const Object* object);

protected:

  // obj: source object of the effect, parent: object at which the effect is played
  // obj != parent: e.g. an object obj in the inventory of bot parent plays a sound
  void playSound(int sound, float volume, const Object* parent) const;
  void playContSound(int sound, float volume, const Object* parent) const;
  void playEngineSound(int sound, float volume, float pitch, const Object* parent) const;

  explicit Audio(const Object* obj_);

public:

  static Collider    collider;

  const Object*      obj;
  const ObjectClass* clazz;
  int                flags;

  virtual ~Audio();

  OZ_NO_COPY(Audio)
  OZ_NO_MOVE(Audio)

  virtual void play(const Object* playAt) = 0;

};

}
