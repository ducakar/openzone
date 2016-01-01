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

#include <matrix/Dynamic.hh>

#include <matrix/LuaMatrix.hh>
#include <matrix/Synapse.hh>
#include <matrix/Collider.hh>

namespace oz
{

Pool<Dynamic> Dynamic::pool(4096);

void Dynamic::onDestroy()
{
  if (!clazz->onDestroy.isEmpty()) {
    luaMatrix.objectCall(clazz->onDestroy, this);
  }

  for (int i : items) {
    Dynamic* item = orbis.obj<Dynamic>(i);

    if (item != nullptr) {
      item->destroy();
    }
  }

  if (cell != nullptr && clazz->fragPool != nullptr) {
    synapse.gen(clazz->fragPool,
                clazz->nFrags,
                Bounds(Point(p.x - dim.x, p.y - dim.y, p.z),
                       Point(p.x + dim.x, p.y + dim.y, p.z + dim.z)),
                velocity + DESTRUCT_FRAG_VELOCITY);
  }
}

bool Dynamic::rotate(int steps)
{
  OZ_ASSERT(steps >= 0);

  int heading = flags & Object::HEADING_MASK;

  if (steps % 2 == 0) {
    flags &= ~Object::HEADING_MASK;
    flags |= (heading + steps) % 4;
    return true;
  }
  else {
    swap(dim.x, dim.y);

    if (collider.overlaps(this)) {
      swap(dim.x, dim.y);
      return false;
    }
    else {
      flags &= ~Object::HEADING_MASK;
      flags |= (heading + steps) % 4;
      return true;
    }
  }
}

Dynamic::Dynamic(const DynamicClass* clazz_, int index_, const Point& p_, Heading heading) :
  Object(clazz_, index_, p_, heading)
{
  velocity = Vec3::ZERO;
  momentum = Vec3::ZERO;
  floor    = Vec3(0.0f, 0.0f, 1.0f);
  parent   = -1;
  lower    = -1;
  depth    = 0.0f;
  mass     = clazz_->mass;
  lift     = clazz_->lift;
}

Dynamic::Dynamic(const DynamicClass* clazz_, int index, const Json& json) :
  Object(clazz_, index, json)
{
  velocity = Vec3::ZERO;
  momentum = Vec3::ZERO;
  floor    = Vec3(0.0f, 0.0f, 1.0f);
  parent   = -1;
  lower    = -1;
  depth    = 0.0f;
  mass     = clazz_->mass;
  lift     = clazz_->lift;
}

Dynamic::Dynamic(const DynamicClass* clazz_, Stream* is) :
  Object(clazz_, is)
{
  velocity = is->read<Vec3>();
  momentum = is->read<Vec3>();
  floor    = is->read<Vec3>();
  parent   = is->readInt();
  lower    = is->readInt();
  depth    = is->readFloat();
  mass     = clazz_->mass;
  lift     = clazz_->lift;
}

Json Dynamic::write() const
{
  return Object::write();
}

void Dynamic::write(Stream* os) const
{
  Object::write(os);

  os->write<Vec3>(velocity);
  os->write<Vec3>(momentum);
  os->write<Vec3>(floor);
  os->writeInt(parent);
  os->writeInt(lower);
  os->writeFloat(depth);
}

void Dynamic::readUpdate(Stream*)
{}

void Dynamic::writeUpdate(Stream*) const
{}

}
