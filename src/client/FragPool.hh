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
 * @file client/FragPool.hh
 */

#pragma once

#include <client/common.hh>

namespace oz::client
{

class Model;

class FragPool
{
public:

  static constexpr int   UPDATED_BIT = 0x01;
  static constexpr int   FADEOUT_BIT = oz::FragPool::FADEOUT_BIT;
  static constexpr float FRAG_RADIUS = 1.0f;

private:

  const oz::FragPool* pool;

  List<Model*> models;

public:

  int flags;

public:

  explicit FragPool(const oz::FragPool* pool_);
  ~FragPool();

  OZ_NO_COPY(FragPool)
  OZ_NO_MOVE(FragPool)

  void draw(const Frag* frag);

};

}
