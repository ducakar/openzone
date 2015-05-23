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
 * @file matrix/Struct.hh
 */

#pragma once

#include <matrix/BSP.hh>

namespace oz
{

class Struct;
class Object;
class Bot;

class Entity
{
  friend class Struct;

private:

  typedef void (Entity::* Handler)();

  static const Handler HANDLERS[];

public:

  enum State
  {
    CLOSED,
    OPENING,
    OPENED,
    CLOSING
  };

  const EntityClass* clazz;
  const Struct*      str;

  int                key;

  State              state;
  float              ratio;
  float              time;

  Vec3               offset;
  Vec3               velocity;

public:

  int index() const;
  bool trigger();
  bool lock(Bot* user);

private:

  void staticHandler();
  void manualDoorHandler();
  void autoDoorHandler();
  void ignoringBlockHandler();
  void crushingBlockHandler();
  void elevatorHandler();

};

class Struct : public Bounds
{
private:

  static const Vec3  DESTRUCT_FRAG_VELOCITY;
  static const float DEMOLISH_SPEED;

public:

  static const int MAX_ENT_SHIFT = 16;
  static const int MAX_ENTITIES  = 1 << MAX_ENT_SHIFT;

public:

  static List<Object*> overlappingObjs;
  static Pool<Struct>  pool;

private:

  Mat4         transf;
  Mat4         invTransf;

public:

  const BSP*   bsp;

  Point        p;
  Heading      heading;

  int          index;

  float        life;
  float        resistance;
  float        demolishing;

  List<Entity> entities;
  List<int>    boundObjects;

private:

  void onDemolish();
  void onUpdate();

public:

  // no copying
  Struct(const Struct&) = delete;
  Struct& operator = (const Struct&) = delete;

  /**
   * Rotate vector from structure coordinate system to absolute coordinate system.
   */
  OZ_ALWAYS_INLINE
  Vec3 toAbsoluteCS(const Vec3& v) const
  {
    return transf * v;
  }

  /**
   * Rotate vector from absolute coordinate system to structure coordinate system.
   */
  OZ_ALWAYS_INLINE
  Vec3 toStructCS(const Vec3& v) const
  {
    return invTransf * v;
  }

  /**
   * Rotate point from structure coordinate system to absolute coordinate system.
   */
  OZ_ALWAYS_INLINE
  Point toAbsoluteCS(const Point& point) const
  {
    return transf * point;
  }

  /**
   * Rotate point from absolute coordinate system to structure coordinate system.
   */
  OZ_ALWAYS_INLINE
  Point toStructCS(const Point& point) const
  {
    return invTransf * point;
  }

  /**
   * Rotate Bounds from structure coordinate system to absolute coordinate system.
   */
  Bounds toAbsoluteCS(const Bounds& bb) const;

  /**
   * Rotate Bounds from absolute coordinate system to structure coordinate system.
   */
  Bounds toStructCS(const Bounds& bb) const;

  /**
   * Rotate AABB::dim between structure and absolute coordinate system.
   */
  OZ_ALWAYS_INLINE
  Vec3 swapDimCS(const Vec3& dim) const
  {
    return heading & WEST_EAST_MASK ? Vec3(dim.y, dim.x, dim.z) : dim;
  }

  void destroy();

  OZ_ALWAYS_INLINE
  void damage(float damage)
  {
    damage -= resistance;

    if (damage > 0.0f) {
      life = max(0.0f, life - damage);
    }
  }

  OZ_ALWAYS_INLINE
  void update()
  {
    if (!entities.isEmpty() || !boundObjects.isEmpty() || life == 0.0f) {
      onUpdate();
    }
  }

public:

  explicit Struct(const BSP* bsp, int index, const Point& p, Heading heading);
  explicit Struct(const BSP* bsp, int index, const Json& json);
  explicit Struct(const BSP* bsp, Stream* is);

  Json write() const;
  void write(Stream* os) const;

  OZ_STATIC_POOL_ALLOC(pool)
};

OZ_ALWAYS_INLINE
inline int Entity::index() const
{
  return (str->index << Struct::MAX_ENT_SHIFT) | int(this - str->entities.begin());
}

}
