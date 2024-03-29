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
 * @file client/BSPAudio.hh
 */

#pragma once

#include <client/common.hh>

namespace oz::client
{

class BSPAudio
{
private:

  const BSP* bsp;

private:

  void playDemolish(const Struct* str, int sound) const;
  void playSound(const Entity* entity, int sound) const;
  void playContSound(const Entity* entity, int sound) const;

public:

  explicit BSPAudio(const BSP* bsp_);
  ~BSPAudio();

  OZ_NO_COPY(BSPAudio)
  OZ_NO_MOVE(BSPAudio)

  void play(const Struct* str) const;

};

}
