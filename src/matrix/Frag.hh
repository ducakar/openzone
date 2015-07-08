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
 * @file matrix/Frag.hh
 */

#pragma once

#include <matrix/FragPool.hh>

namespace oz
{

struct Cell;

class Frag : public DChainNode<Frag>
{
private:

  static const float DAMAGE_THRESHOLD;

public:

  static Pool<Frag> mpool;

  /*
   *  FIELDS
   */

  Cell*           cell;
  int             index;         // position in world.objects vector

  int             poolId;
  const FragPool* pool;

  Point           p;
  Vec3            velocity;

  float           life;
  float           mass;
  float           elasticity;

  // no copying
  Frag(const Frag&) = delete;
  Frag& operator = (const Frag&) = delete;

  void damage(float damage)
  {
    damage -= DAMAGE_THRESHOLD;

    if (damage > 0.0f) {
      life = 0.0f;
    }
  }

public:

  explicit Frag(const FragPool* pool, int index, const Point& p, const Vec3& velocity);
  explicit Frag(const FragPool* pool, Stream* is);

  void write(Stream* os) const;

  void readUpdate(Stream* is);
  void writeUpdate(Stream* os);

  OZ_STATIC_POOL_ALLOC(mpool)

};

}
