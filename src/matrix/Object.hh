/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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

#include "matrix/ObjectClass.hh"

namespace oz
{
namespace matrix
{

struct Cell;
class  Bot;

/**
 * Static object class and base class for other object classes.
 *
 * @ingroup matrix
 */
class Object : public AABB
{
  /*
   * Here various flag bits are set; the higher bits are used for flags that are internal flags
   * and should only be hard-coded in the engine and cannot be set in object class's configuration
   * file. The lower bits are set for object class in the corresponding configuration file. The
   * lower bits are used for that because of easier calculation.
   */

  public:

    /*
     * TYPE FLAGS
     */

    // Dynamic object
    static const int DYNAMIC_BIT        = 0x40000000;

    // Weapon
    static const int WEAPON_BIT         = 0x20000000;

    // Bot
    static const int BOT_BIT            = 0x10000000;

    // Vehicle
    static const int VEHICLE_BIT        = 0x08000000;

    // can be put into inventory
    static const int ITEM_BIT           = 0x04000000;

    // bots have access to this object's inventory
    static const int BROWSABLE_BIT      = 0x02000000;

    /*
     * FUNCTION FLAGS
     */

    // if object has Lua handlers
    static const int LUA_BIT            = 0x00800000;

    // if the onDestroy method should be called on destruction
    static const int DESTROY_FUNC_BIT   = 0x00400000;

    // if the onUse function should be called when object is used
    static const int USE_FUNC_BIT       = 0x00200000;

    // if the onUpdate method should be called on each tick
    static const int UPDATE_FUNC_BIT    = 0x00100000;

    /*
     * BOUND OBJECTS
     */

    // if the object has an Device object in nirvana
    static const int DEVICE_BIT         = 0x00040000;

    // if the object has an Imago object in frontend
    static const int IMAGO_BIT          = 0x00020000;

    // if the object has an Audio object in frontend
    static const int AUDIO_BIT          = 0x00010000;

    /*
     * STATE FLAGS
     */

    // when object's life drops to <= 0.0f it's tagged as destroyed first and kept one more tick
    // in the world, so destruction effects can be processed by frontend (e.g. destruction sounds)
    // in the next tick the destroyed objects are actually removed
    static const int DESTROYED_BIT      = 0x00008000;

    /*
     * COLLISION & PHYSICS FLAGS
     */

    // other object can collide with the object
    static const int SOLID_BIT          = 0x00004000;

    // use cylinder model for collision between objects when both are flagged as cylinder
    static const int CYLINDER_BIT       = 0x00002000;

    // if the object is still and on a still surface, we won't handle physics for it
    static const int DISABLED_BIT       = 0x00001000;

    // force full physics update in the next step
    static const int ENABLE_BIT         = 0x00000800;

    // if the object is has been sliding on a floor or on another object in last step
    static const int FRICTING_BIT       = 0x00000400;

    // if the object has collided into another dynamic object from below (to prevent stacked
    // objects from being carried around)
    static const int BELOW_BIT          = 0x00000200;

    // if the object lies or moves on a structure, terrain or non-dynamic object
    // (if on another dynamic object, we determine that with "lower" index)
    static const int ON_FLOOR_BIT       = 0x00000100;

    // if the object is on slipping surface (not cleared if disabled)
    static const int ON_SLICK_BIT       = 0x00000080;

    // if the object intersects with a liquid (not cleared if disabled)
    static const int IN_LIQUID_BIT      = 0x00000040;

    // if the object is in lava
    static const int IN_LAVA_BIT        = 0x00000020;

    // if the object (bot in this case) is on ladder (not cleared if disabled)
    static const int ON_LADDER_BIT      = 0x00000010;

    /*
     * RENDER FLAGS
     */

    // wide frustum culling: object is represented some times larger to frustum culling system than
    // it really is; how larger it is, is specified by client::Render::WIDE_CULL_FACTOR
    static const int WIDE_CULL_BIT      = 0x00000008;

    /*
     * FLAG MASKS
     */

    // masks Heading enum number
    static const int HEADING_MASK       = 0x00000003;

    // those flags are cleared by Physics on each tick
    static const int TICK_CLEAR_MASK    = FRICTING_BIT;

    // those flags are cleared by Physics on each update when an object moves (plus lower = -1)
    static const int MOVE_CLEAR_MASK    = DISABLED_BIT | ON_FLOOR_BIT | IN_LIQUID_BIT |
                                          IN_LAVA_BIT | ON_LADDER_BIT | ON_SLICK_BIT;

    static const int   MAX_DIM          = 4;
    static const float REAL_MAX_DIM;

  protected:

    static const float MOMENTUM_DAMAGE_COEF;
    static const float MOMENTUM_INTENSITY_COEF;
    static const float DAMAGE_BASE_INTENSITY;
    static const float DAMAGE_INTENSITY_COEF;
    static const Vec3  DESTRUCT_FRAG_VELOCITY;

  public:

    /*
     * EVENTS
     */

    // EVENT_CREATE must be invoked manually
    static const int EVENT_CREATE   = 0;
    static const int EVENT_DESTROY  = 1;
    static const int EVENT_DAMAGE   = 2;
    static const int EVENT_HIT      = 3;
    static const int EVENT_LAND     = 4;
    static const int EVENT_SPLASH   = 5;
    // EVENT_FRICTING is not in use, it only reserves a slot for friction sound
    static const int EVENT_FRICTING = 6;
    static const int EVENT_USE      = 7;

    struct Event
    {
      static Pool<Event, 256> pool;

      int    id;
      float  intensity;
      Event* next[1];

      OZ_ALWAYS_INLINE
      explicit Event( int id_, float intensity_ ) :
        id( id_ ), intensity( intensity_ )
      {
        hard_assert( intensity >= 0.0f );
      }

      OZ_STATIC_POOL_ALLOC( pool )
    };

  public:

    static Pool<Object, 16384> pool;

    /*
     * FIELDS
     */

    Object*            prev[1];    // the previous object in cell.objects and list
    Object*            next[1];    // the next object in cell.objects and list

    Cell*              cell;       // parent cell, null if not positioned in the world
    int                index;      // position in world.objects vector

    int                flags;

    float              life;
    float              resistance;

    const ObjectClass* clazz;

    // events are used for reporting hits, friction & stuff and are cleared at the beginning of
    // the matrix tick
    List<Event>        events;
    // inventory of an object
    Vector<int>        items;

  protected:

    /*
     * EVENT HANDLERS
     */

    virtual void onDestroy();
    virtual bool onUse( Bot* user );
    virtual void onUpdate();

  public:

    virtual ~Object();

    // no copying
    Object( const Object& ) = delete;
    Object& operator = ( const Object& ) = delete;

    /**
     * Add an event to the object. Events can be used for reporting collisions, sounds etc.
     *
     * @param id
     * @param intensity
     */
    OZ_ALWAYS_INLINE
    void addEvent( int id, float intensity = 1.0f )
    {
      events.add( new Event( id, intensity ) );
    }

    OZ_ALWAYS_INLINE
    void destroy()
    {
      if( !( flags & DESTROYED_BIT ) ) {
        life = 0.0f;
        flags |= DESTROYED_BIT;
        addEvent( EVENT_DESTROY, 1.0f );

        if( flags & DESTROY_FUNC_BIT ) {
          onDestroy();
        }
      }
    }

    /**
     * Inflict damage to the object.
     *
     * @param damage
     */
    OZ_ALWAYS_INLINE
    void damage( float damage )
    {
      damage -= resistance;

      if( damage > 0.0f ) {
        life -= damage;
        addEvent( EVENT_DAMAGE, DAMAGE_BASE_INTENSITY + damage * DAMAGE_INTENSITY_COEF );
      }
    }

    /**
     * Called by physics engine when object hits something.
     *
     * @param energy momentum square (usually projected to the hit normal).
     * @param hasLanded whether EVENT_LAND should be issued instead of EVENT_HIT.
     */
    OZ_ALWAYS_INLINE
    void hit( float energy, bool hasLanded = false )
    {
      addEvent( EVENT_HIT + hasLanded, energy * MOMENTUM_INTENSITY_COEF );
      damage( energy * MOMENTUM_DAMAGE_COEF );
    }

    OZ_ALWAYS_INLINE
    void splash( float energy )
    {
      addEvent( EVENT_SPLASH, energy * MOMENTUM_INTENSITY_COEF );
    }

    OZ_ALWAYS_INLINE
    void use( Bot* user )
    {
      if( flags & USE_FUNC_BIT ) {
        if( onUse( user ) ) {
          addEvent( EVENT_USE, 1.0f );
        }
      }
    }

    /**
     * Perform object update. One can implement onUpdate function to do some custom stuff.
     */
    OZ_ALWAYS_INLINE
    void update()
    {
      if( flags & UPDATE_FUNC_BIT ) {
        onUpdate();
      }
    }

  public:

    explicit Object( const ObjectClass* clazz, int index, const Point& p, Heading heading );
    explicit Object( const ObjectClass* clazz, InputStream* istream );

    virtual void write( BufferStream* ostream ) const;

    virtual void readUpdate( InputStream* istream );
    virtual void writeUpdate( BufferStream* ostream ) const;

    OZ_STATIC_POOL_ALLOC( pool )

};

}
}
