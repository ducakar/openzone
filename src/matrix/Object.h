/*
 *  Object.h
 *
 *  Base object class, can be used for static objects
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "ObjectClass.h"

namespace oz
{

  struct Cell;
  struct Hit;
  struct Bot;

  // static object abstract class
  class Object : public AABB
  {
    friend class DList<Object, 0>;

    /*
     * Here various flag bits are set; the higher bits are used for flags that are internal flags
     * and should only be hardcoded in the engine and cannot be set in object class's configuration
     * file. The lower bits are set for object class in the corresponding configuration file. The
     * lower bits are used for that because of easier calculation.
     */

    public:

      /*
       * TYPE FLAGS
       */

      // DynObject
      static const int DYNAMIC_BIT        = 0x80000000;
      // can be put into inventory
      static const int ITEM_BIT           = 0x40000000;
      // Weapon
      static const int WEAPON_BIT         = 0x20000000;
      // Bot
      static const int BOT_BIT            = 0x10000000;
      // Vehicle
      static const int VEHICLE_BIT        = 0x08000000;

      /*
       * FUNCTION FLAGS
       */

      // if object has Lua handlers
      static const int LUA_BIT            = 0x04000000;

      // if the onCreate function should be called when object is added into world
      static const int CREATE_FUNC_BIT    = 0x02000000;

      // if the onDestroy function should be called on destruction
      static const int DESTROY_FUNC_BIT   = 0x01000000;

      // if the onDamage function should be called on damage received
      static const int DAMAGE_FUNC_BIT    = 0x00800000;

      // if the onHit function should be called on hit
      static const int HIT_FUNC_BIT       = 0x00400000;

      // if the onUpdate method should be called each step
      static const int UPDATE_FUNC_BIT    = 0x00200000;

      // if the onUse function should be called when object is used
      static const int USE_FUNC_BIT       = 0x00100000;

      /*
       * FRONTEND OBJECTS
       */

      // if the object has a model object in frontend
      static const int MODEL_BIT          = 0x00080000;

      // if the object has an audio object in frontend
      static const int AUDIO_BIT          = 0x00040000;

      /*
       * STATE FLAGS
       */

      // if object is not positioned in the world (used only when loading/saving the world and to
      // distinguish between removed objects and cut objects that have been removed with removeCut)
      static const int CUT_BIT            = 0x00020000;

      // when object's life drops to <= 0.0f it's tagged as destroyed first and kept one more tick
      // in the world, so destruction effects can be processed by frontend (e.g. destruction sounds)
      // in the next tick the destroyed objects are actually removed
      static const int DESTROYED_BIT      = 0x00010000;

      /*
       * DYNAMIC OBJECTS' BITS
       */

      // if the object is still and on a still surface, we won't handle physics for it
      static const int DISABLED_BIT       = 0x00008000;

      // if the object collided in the last step
      static const int HIT_BIT            = 0x00004000;

      // if the object is currently fricting
      static const int FRICTING_BIT       = 0x00002000;

      // if an another dynamic object has set it for lower object in last step
      static const int UPPER_BIT          = 0x00001000;

      // if the the object lies or moves on a structure, terrain or non-dynamic object
      // (if on another dynamic object, we determine that with "lower" index)
      static const int ON_FLOOR_BIT       = 0x00000800;

      // if the object is on ice (slipping surface)
      static const int ON_SLICK_BIT       = 0x00000400;

      // if the object intersects with water
      static const int IN_WATER_BIT       = 0x00000200;

      // if the object is on ladder
      static const int ON_LADDER_BIT      = 0x00000100;

      // handle collisions for this object
      static const int CLIP_BIT           = 0x00000080;

      // If the object is climber it is tested against ladder brushes and gains ON_LADDER_BIT if it
      // intersects with a ladder brush. Otherwise object is not affected by ladders.
      static const int CLIMBER_BIT        = 0x00000040;

      // if the object is meant to push itself and other objects around  (e.g. Bot), turn on
      // some physics hacks (to prevent continuous hits) and enable pushing to side directions
      static const int PUSHER_BIT         = 0x00000020;

      // if the object is immune to gravity
      static const int HOVER_BIT          = 0x00000010;

      /*
       * RENDER FLAGS
       */

      // don't render object (it will be rendered via another path, e.g. bots in a vehicle)
      static const int NO_DRAW_BIT        = 0x00000008;

      // render after other objects (for large blended objects, e.g. explosions)
      static const int DELAYED_DRAW_BIT   = 0x00000004;

      // wide frustum culling: object is represented some times larger to frustum culling
      // system than it really is;
      // how larger it is, is specified by Client::Render::RELEASED_CULL_FACTOR (default 5.0f)
      static const int WIDE_CULL_BIT      = 0x00000002;

      /*
       * STANDARD EVENT IDs
       */

      static const int EVENT_CREATE       = 0;
      static const int EVENT_DESTROY      = 1;
      static const int EVENT_DAMAGE       = 2;
      static const int EVENT_HIT          = 3;
      static const int EVENT_SPLASH       = 4;
      // EVENT_FRICTING not in use, but reserved for more convenient BasicAudio (reserves a slot for
      // friction sound)
      static const int EVENT_FRICTING     = 5;
      static const int EVENT_USE          = 6;

      static const float MOMENTUM_INTENSITY_COEF = -0.1f;
      static const float DAMAGE_INTENSITY_COEF   = 0.02f;

      struct Event : PoolAlloc<Event, 0>
      {
        int   id;
        float intensity;
        Event *next[1];

        // exactly events with negative IDs are ignored by BasicAudio, so if ID is nonzero we don't
        // want to use this ctor as we need to set the intensity
        explicit Event( int id_ ) : id( id_ )
        {
          assert( id < 0 );
        }

        explicit Event( int id_, float intensity_ ) : id( id_ ), intensity( intensity_ )
        {
          assert( id < 0 || intensity >= 0.0f );
        }
      };

      /*
       * FIELDS
       */

    private:

      Object            *prev[1];     // the previous object in cell.objects and list
      Object            *next[1];     // the next object in cell.objects and list

    public:

      int               index;        // position in world.objects vector
      Cell              *cell;        // parent cell, null if not positioned in the world

      int               flags;
      int               oldFlags;

      const ObjectClass *type;

      // damage
      float             life;

      // events are used for reporting hits, friction & stuff and are cleared at the beginning of
      // the frame
      List<Event, 0>    events;

    public:

      explicit Object() : index( -1 ), cell( null )
      {}

      virtual ~Object();

      /**
       * Add an event to the object. Events can be used for reporting collisions, sounds etc.
       * @param id
       */
      void addEvent( int id )
      {
        events << new Event( id );
      }

      /**
       * Add an event to the object. Events can be used for reporting collisions, sounds etc.
       * @param id
       * @param intensity
       */
      void addEvent( int id, float intensity )
      {
        events << new Event( id, intensity );
      }

      void destroy()
      {
        if( ~flags & DESTROYED_BIT ) {
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
       * @param damage
       */
      void damage( float damage )
      {
        damage -= type->damageThreshold;

        if( damage > 0.0f ) {
          life -= damage;
          addEvent( EVENT_DAMAGE, damage * DAMAGE_INTENSITY_COEF );

          if( flags & DAMAGE_FUNC_BIT ) {
            onDamage( damage );
          }
        }
      }

      /**
       * Called by physics engine when the object hits something.
       * @param hit Hit struct filled with collision data
       * @param hitMomentum momentum of the object projected to hit normal
       */
      void hit( const Hit *hit, float hitMomentum )
      {
        flags |= HIT_BIT;
        addEvent( EVENT_HIT, hitMomentum * MOMENTUM_INTENSITY_COEF );
        damage( hitMomentum * hitMomentum );

        if( flags & HIT_FUNC_BIT ) {
          onHit( hit, hitMomentum );
        }
      }

      void splash( float momentum )
      {
        addEvent( EVENT_SPLASH, momentum * MOMENTUM_INTENSITY_COEF );
      }

      /**
       * Perform object update. One can implement onUpdate function to do some custom stuff.
       */
      void update()
      {
        if( flags & UPDATE_FUNC_BIT ) {
          onUpdate();
        }
        oldFlags = flags;
      }

      void use( Bot *user )
      {
        if( flags & USE_FUNC_BIT ) {
          addEvent( EVENT_USE, 1.0f );
          onUse( user );
        }
      }

    protected:

      virtual void onCreate();
      virtual void onDestroy();
      virtual void onDamage( float damage );
      virtual void onHit( const Hit *hit, float momentum );
      virtual void onUpdate();
      virtual void onUse( Bot *user );

    public:

      /*
       * SERIALIZATION
       */

      virtual void readFull( InputStream *istream );
      virtual void writeFull( OutputStream *ostream ) const;
      virtual void readUpdate( InputStream *istream );
      virtual void writeUpdate( OutputStream *ostream ) const;

  };

}
