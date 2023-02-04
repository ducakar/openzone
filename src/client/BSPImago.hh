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
 * @file client/BSPImago.hh
 */

#pragma once

#include <client/Model.hh>

namespace oz::client
{

class BSPImago
{
private:

  Model     model;

  List<int> leafClusters;
  int       nClusters;
  int       nClusterBits;
  Bitset    clusters;

public:

  Vec4      waterFogColour;
  Vec4      lavaFogColour;

public:

  explicit BSPImago(const BSP* bsp);
  ~BSPImago();

  OZ_NO_COPY(BSPImago)
  OZ_NO_MOVE(BSPImago)

  Vec3 dim() const
  {
    return model.dim;
  }

  bool isPreloaded() const
  {
    return model.isPreloaded();
  }

  bool isLoaded() const
  {
    return model.isLoaded();
  }

  void schedule(const Struct* str, Model::QueueType queue);

  void preload();
  void load();

};

}
