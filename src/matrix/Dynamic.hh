/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file matrix/Dynamic.hh
 */

#pragma once

#include <matrix/Object.hh>
#include <matrix/DynamicClass.hh>

namespace oz
{

class Dynamic : public Object
{
public:

  static Pool<Dynamic, 4096> pool; ///< Memory pool.

  Vec3  velocity;
  Vec3  momentum; ///< Desired velocity
  Vec3  floor;    ///< Floor normal.

  int   parent;   ///< Index of container object or -1 if object in positioned in the world.
  int   lower;    ///< Index of structure entity or object below this object, -1 otherwise.
  float depth;    ///< Depth of object's lower bound when intersecting liquid, 0 otherwise.

  float mass;     ///< Mass.
  float lift;     ///< Lift / weight (water only, for lava Physics::LAVA_LIFT is used).

protected:

  void onDestroy() override;

public:

  explicit Dynamic( const DynamicClass* clazz, int index, const Point& p, Heading heading );
  explicit Dynamic( const DynamicClass* clazz, InputStream* is );
  explicit Dynamic( const DynamicClass* clazz, const JSON& json );

  void write( OutputStream* os ) const override;
  JSON write() const override;

  void readUpdate( InputStream* is ) override;
  void writeUpdate( OutputStream* os ) const override;

  OZ_STATIC_POOL_ALLOC( pool )
};

}
