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
 * @file matrix/Object.hh
 *
 * Base object class, can be used for static objects.
 */

#pragma once

#include <matrix/ObjectClass.hh>

namespace oz
{
namespace matrix
{

struct Cell;
class  Bot;

/**
 * Static object class and base class for other object classes.
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

    union Flags
    {
      struct {
      int dynamic     : 1; ///< Dynamic object.
      int weapon      : 1; ///< Weapon.
      int bot         : 1; ///< Bot.
      int vehicle     : 1; ///< Vehicle.
      int item        : 1; ///< Can be put into inventory.
      int browsable   : 1; ///< Bots can access this object's inventory.

      int lua         : 1; ///< Class has %Lua handlers.
      int destroyFunc : 1; ///< `onDestroy()` handler is present and enabled.
      int useFunc     : 1; ///< `onUse()` handler is present and enabled.
      int updateFunc  : 1; ///< `onUpdate()` handler is present and enabled.

      int device      : 1; ///< The object has a `Device` object bound in nirvana.
      int imago       : 1; ///< The object has an `Imago` object in client.
      int audio       : 1; ///< The object has an `Audio` object in client.

      int destroyed   : 1; ///< When an object's life drops to <= 0.0 it's tagged as destroyed
                           ///< first and kept one more tick in the world, so destruction effects
                           ///< can be processed by client (e.g. destruction sounds) in the next
                           ///< tick the destroyed objects are actually removed.

      int solid       : 1; ///< Other object can collide with the object.
      int cylinder    : 1; ///< Use cylinder collision model if the other object is also cylinder.
      int disabled    : 1; ///< The Object is still (on a still surface), skip physics for it.
      int enable      : 1; ///< Force full physics update in the next tick.
      int fricting    : 1; ///< The object has been sliding on floor or another object in last tick.
      int below       : 1; ///< If an object has collided into another dynamic object from below
                           ///< (to prevent stacked objects from being carried around).
      int onFloor     : 1; ///< The object lies or moves on a structure, terrain or non-dynamic
                           ///< object.
      int onSlick     : 1; ///< The object is on slipping surface.
      int inLiquid    : 1; ///< The object intersects with liquid.
      int inLava      : 1; ///< The liquid object intersects with is lava.
      int onLadder    : 1; ///< The bot is on ladder.

      int wideCull    : 1; ///< Wide frustum culling: object is represented some times larger to
                           ///< frustum culling system than it really is; how larger it is, is
                           ///< specified by `client::Render::WIDE_CULL_FACTOR`.

      Heading heading : 2; ///< Heading.
      };
      int bitfield;

      void tickClear()
      {
        fricting = false;
      }

      void moveClear()
      {
        disabled = false;
        onFloor  = false;
        onSlick  = false;
        inLiquid = false;
        inLava   = false;
        onLadder = false;
      }
    };

    static const int   MAX_DIM          = 5;
    static const float REAL_MAX_DIM;

  protected:

    static const float DAMAGE_BASE_INTENSITY;
    static const float DAMAGE_INTENSITY_COEF;
    static const Vec3  DESTRUCT_FRAG_VELOCITY;

  public:

    /*
     * EVENTS
     */

    // EVENT_CREATE must be invoked manually
    static const int EVENT_CREATE     = 0;
    static const int EVENT_DESTROY    = 1;
    static const int EVENT_DAMAGE     = 2;
    static const int EVENT_HIT        = 3;
    static const int EVENT_LAND       = 4;
    static const int EVENT_SPLASH     = 5;
    // EVENT_FRICTING is not in use, it only reserves a slot for friction sound
    static const int EVENT_FRICTING   = 6;
    static const int EVENT_USE        = 7;
    static const int EVENT_USE_FAILED = 8;

    class Event
    {
      public:

        static Pool<Event, 256> pool;

        int    id;
        float  intensity;
        Event* next[1];

      public:

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

    Cell*              cell;       // parent cell, nullptr if not positioned in the world
    int                index;      // position in world.objects vector

    Flags              flags;

    float              life;
    float              resistance;

    const ObjectClass* clazz;

    // events are used for reporting hits, friction & stuff and are cleared at the beginning of
    // the matrix tick
    Chain<Event>       events;
    // inventory of an object
    List<int>          items;

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
     */
    OZ_ALWAYS_INLINE
    void addEvent( int id, float intensity )
    {
      events.add( new Event( id, intensity ) );
    }

    OZ_ALWAYS_INLINE
    void destroy()
    {
      if( !flags.destroyed ) {
        flags.destroyed = true;
        life = 0.0f;

        if( flags.destroyFunc ) {
          onDestroy();
        }
        addEvent( EVENT_DESTROY, 1.0f );
      }
    }

    /**
     * Inflict damage to the object.
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

    OZ_ALWAYS_INLINE
    void use( Bot* user )
    {
      if( flags.useFunc ) {
        bool success = onUse( user );
        addEvent( EVENT_USE_FAILED - success, 1.0f );
      }
    }

    /**
     * Perform object update. One can implement onUpdate function to do some custom stuff.
     */
    OZ_ALWAYS_INLINE
    void update()
    {
      if( flags.updateFunc ) {
        onUpdate();
      }
    }

  public:

    explicit Object( const ObjectClass* clazz, int index, const Point& p, Heading heading );
    explicit Object( const ObjectClass* clazz, InputStream* istream );
    explicit Object( const ObjectClass* clazz, const JSON& json );

    virtual void write( OutputStream* ostream ) const;
    virtual JSON write() const;

    virtual void readUpdate( InputStream* istream );
    virtual void writeUpdate( OutputStream* ostream ) const;

    OZ_STATIC_POOL_ALLOC( pool )

};

}
}
