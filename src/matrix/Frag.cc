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

#include <matrix/Frag.hh>

namespace oz
{

Pool<Frag> Frag::mpool(2048);

Frag::Frag(const FragPool* pool_, int index_, const Point& p_, const Vec3& velocity_)
{
  cell       = nullptr;
  index      = index_;
  poolId     = pool_->id;
  pool       = pool_;
  p          = p_;
  velocity   = velocity_;
  life       = pool->life;
  mass       = pool->mass;
  elasticity = pool->elasticity;
}

Frag::Frag(const FragPool* pool_, Stream* is)
{
  cell       = nullptr;
  index      = is->readInt();
  poolId     = pool_->id;
  pool       = pool_;
  p          = is->read<Point>();
  velocity   = is->read<Vec3>();
  life       = is->readFloat();
  mass       = pool->mass;
  elasticity = pool->elasticity;
}

void Frag::write(Stream* os) const
{
  os->writeInt(index);
  os->write<Point>(p);
  os->write<Vec3>(velocity);
  os->writeFloat(life);
}

void Frag::readUpdate(Stream*)
{}

void Frag::writeUpdate(Stream*)
{}

}
