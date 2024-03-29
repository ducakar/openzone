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
 * @file matrix/Object.hh
 *
 * Base object class, can be used for static objects.
 */

#pragma once

#include <matrix/ObjectClass.hh>

namespace oz
{

struct Cell;
class Bot;

/**
 * Static object class and base class for other object classes.
 */
class Object : public AABB, public DChainNode<Object>
{
public:

  /*
   * TYPE FLAGS
   */

  // Dynamic object
  static constexpr int DYNAMIC_BIT        = 0x40000000;

  // Weapon
  static constexpr int WEAPON_BIT         = 0x20000000;

  // Bot
  static constexpr int BOT_BIT            = 0x10000000;

  // Vehicle
  static constexpr int VEHICLE_BIT        = 0x08000000;

  // can be put into inventory
  static constexpr int ITEM_BIT           = 0x04000000;

  // bots have access to this object's inventory
  static constexpr int BROWSABLE_BIT      = 0x02000000;

  /*
   * FUNCTION FLAGS
   */

  // if object has Lua handlers
  static constexpr int LUA_BIT            = 0x01000000;

  // if the onDestroy function should be called on destruction
  static constexpr int DESTROY_FUNC_BIT   = 0x00800000;

  // if the onUse function should be called when object is used
  static constexpr int USE_FUNC_BIT       = 0x00400000;

  // if the onUpdate function should be called on each tick
  static constexpr int UPDATE_FUNC_BIT    = 0x00200000;

  // if the getStatus function should be called to retrieve status bar
  static constexpr int STATUS_FUNC_BIT    = 0x00100000;

  /*
   * BOUND OBJECTS
   */

  // if the object has an Device object in nirvana
  static constexpr int DEVICE_BIT         = 0x00080000;

  // if the object has an Imago object in frontend
  static constexpr int IMAGO_BIT          = 0x00040000;

  // if the object has an Audio object in frontend
  static constexpr int AUDIO_BIT          = 0x00020000;

  /*
   * STATE FLAGS
   */

  // when object's life drops to <= 0.0f it's tagged as destroyed first and kept one more tick
  // in the world, so destruction effects can be processed by frontend (e.g. destruction sounds)
  // in the next tick the destroyed objects are actually removed
  static constexpr int DESTROYED_BIT      = 0x00010000;

  /*
   * COLLISION & PHYSICS FLAGS
   */

  // other object can collide with the object
  static constexpr int SOLID_BIT          = 0x00008000;

  // use cylinder model for collision between objects when both are flagged as cylinder
  static constexpr int CYLINDER_BIT       = 0x00004000;

  // if the object is still and on a still surface, we won't handle physics for it
  static constexpr int DISABLED_BIT       = 0x00002000;

  // force full physics update in the next step even if the object should be disabled
  static constexpr int ENABLE_BIT         = 0x00001000;

  // if the object is has been sliding on a floor or on another object in last step
  static constexpr int FRICTING_BIT       = 0x00000800;

  // if the object has collided into another dynamic object from below (to prevent stacked
  // objects from being carried around)
  static constexpr int BELOW_BIT          = 0x00000400;

  // if the object lies or moves on a structure, terrain or non-dynamic object
  // (if on another dynamic object, we determine that with "lower" index)
  static constexpr int ON_FLOOR_BIT       = 0x00000200;

  // if the object is on slipping surface (not cleared if disabled)
  static constexpr int ON_SLICK_BIT       = 0x00000100;

  // if the object intersects with a liquid (not cleared if disabled)
  static constexpr int IN_LIQUID_BIT      = 0x00000080;

  // if the object is in lava
  static constexpr int IN_LAVA_BIT        = 0x00000040;

  // if the object (bot in this case) is on ladder (not cleared if disabled)
  static constexpr int ON_LADDER_BIT      = 0x00000020;

  /*
   * RENDER FLAGS
   */

  // wide frustum culling: object is represented some times larger to frustum culling system than
  // it really is; how larger it is, is specified by client::Render::WIDE_CULL_FACTOR
  static constexpr int WIDE_CULL_BIT      = 0x00000008;

  /*
   * FLAG MASKS
   */

  // masks Heading enum number
  static constexpr int HEADING_MASK       = 0x00000003;

  // those flags are cleared by Physics on each tick
  static constexpr int TICK_CLEAR_MASK    = FRICTING_BIT | BELOW_BIT;

  // those flags are cleared by Physics on each update when an object moves (plus lower = -1)
  static constexpr int MOVE_CLEAR_MASK    = DISABLED_BIT | ON_FLOOR_BIT | IN_LIQUID_BIT |
                                            IN_LAVA_BIT | ON_LADDER_BIT | ON_SLICK_BIT;

  static constexpr int   MAX_DIM          = 4;
  static constexpr float REAL_MAX_DIM     = float(MAX_DIM) - 0.01f;

protected:

  static constexpr float DAMAGE_BASE_INTENSITY  = 0.30f;
  static constexpr float DAMAGE_INTENSITY_COEF  = 0.01f;
  static constexpr Vec3  DESTRUCT_FRAG_VELOCITY = Vec3(0.0f, 0.0f, 2.0f);

public:

  /*
   * EVENTS
   */

  // Non-negative events are for sounds, negative for other effects.
  static constexpr int EVENT_FLASH    = -2;
  static constexpr int EVENT_SHAKE    = -1;
  // EVENT_CREATE must be invoked manually.
  static constexpr int EVENT_CREATE   = 0;
  static constexpr int EVENT_DESTROY  = 1;
  static constexpr int EVENT_DAMAGE   = 2;
  static constexpr int EVENT_HIT      = 3;
  static constexpr int EVENT_LAND     = 4;
  static constexpr int EVENT_SPLASH   = 5;
  // EVENT_FRICTING is not in use, it only reserves a slot for friction sound.
  static constexpr int EVENT_FRICTING = 6;
  static constexpr int EVENT_USE      = 7;
  static constexpr int EVENT_FAIL     = 8;

  class Event : public ChainNode<Event>
  {
  public:

    static Pool<Event> pool;

    int    id;
    float  intensity;

  public:

    OZ_ALWAYS_INLINE
    explicit Event(int id_, float intensity_)
      : id(id_), intensity(intensity_)
    {}

    OZ_STATIC_POOL_ALLOC(pool)
  };

public:

  static Pool<Object> pool;

  /*
   * FIELDS
   */

  Cell*              cell       = nullptr;     // parent cell, nullptr if not positioned in world
  int                index      = -1;          // index in orbis.objects

  int                flags;

  float              life;
  float              resistance;

  const ObjectClass* clazz;

  // events are used for reporting hits, friction & stuff and are cleared at the beginning of a
  // matrix update
  Chain<Event>       events;
  // inventory
  List<int>          items;

protected:

  /*
   * EVENT HANDLERS
   */

  virtual void onDestroy();
  virtual bool onUse(Bot* user);
  virtual void onUpdate();

  /*
   * INFORMATION
   */

  virtual String getTitle() const;
  virtual float getStatus() const;

public:

  virtual ~Object();

  OZ_NO_COPY(Object)
  OZ_NO_MOVE(Object)

  /**
   * Add an event to the object. Events can be used for reporting collisions, sounds etc.
   */
  OZ_ALWAYS_INLINE
  void addEvent(int id, float intensity)
  {
    events.add(new Event(id, intensity));
  }

  OZ_ALWAYS_INLINE
  void destroy()
  {
    if (!(flags & DESTROYED_BIT)) {
      life = 0.0f;
      flags |= DESTROYED_BIT;

      if (flags & DESTROY_FUNC_BIT) {
        onDestroy();
      }
      addEvent(EVENT_DESTROY, 1.0f);
    }
  }

  /**
   * Inflict damage to the object.
   */
  OZ_ALWAYS_INLINE
  void damage(float damage)
  {
    damage -= resistance;

    if (damage > 0.0f) {
      life = max(0.0f, life - damage);
      addEvent(EVENT_DAMAGE, DAMAGE_BASE_INTENSITY + damage * DAMAGE_INTENSITY_COEF);
    }
  }

  OZ_ALWAYS_INLINE
  bool use(Bot* user)
  {
    if (flags & USE_FUNC_BIT) {
      bool success = onUse(user);
      addEvent(EVENT_FAIL - success, 1.0f);
      return success;
    }
    return false;
  }

  /**
   * Perform object update. One can implement onUpdate function to do some custom stuff.
   */
  OZ_ALWAYS_INLINE
  void update()
  {
    if (flags & UPDATE_FUNC_BIT) {
      onUpdate();
    }
  }

  OZ_ALWAYS_INLINE
  String title() const
  {
    return getTitle();
  }

  OZ_ALWAYS_INLINE
  float status() const
  {
    return flags & STATUS_FUNC_BIT ? getStatus() : -1.0f;
  }

public:

  explicit Object(const ObjectClass* clazz_, int index_, const Point& p_, Heading heading);
  explicit Object(const ObjectClass* clazz_, int index_, const Json& json);
  explicit Object(const ObjectClass* clazz_, Stream* is);

  virtual Json write() const;
  virtual void write(Stream* os) const;

  virtual void readUpdate(Stream* is);
  virtual void writeUpdate(Stream* os) const;

  OZ_STATIC_POOL_ALLOC(pool)

};

}
