/*
 *  Object.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "bv.h"
#include "io.h"
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

      // which bits are can be set in configuration files, other bits are fixed for the given type
      static const int CONFIG_BITS_MASK   = 0x000000ff;
      // which bits are fixed for a type or temporary (~CONFIG_BITS_MASK)
      static const int INTERNAL_BITS_MASK = 0xffffff00;

      /*
       * FUNCTION FLAGS (0x0f000000)
       */

      // if the onUpdate method should be called each step
      static const int UPDATE_FUNC_BIT    = 0x08000000;

      // if the onHit function should be called on hit
      static const int HIT_FUNC_BIT       = 0x04000000;

      // probably we won't get noticable performance boost if we wrap those virtual fuctions
      /*// if the onUse method is called when we use the object (otherwise, nothing happens)
      static const int USE_FUNC_BIT       = 0x02000000;

      // if the onDestroy method is called when the object is destroyed
      static const int DESTROY_FUNC_BIT   = 0x01000000;*/

      /*
       * FRONTEND OBJECTS (0x00c00000)
       */

      // if the object has a model object in frontend
      static const int MODEL_BIT          = 0x00400000;

      // if the object has an audio object in frontend
      static const int AUDIO_BIT          = 0x00200000;

      /*
       * CUT FLAG (0x00100000)
       */

      // if object is pending for cut/removal; needed to protect against double cuts/removals
      static const int CUT_BIT            = 0x00100000;

      /*
       * TYPE FLAGS (0x000f0000)
       */

      static const int DYNAMIC_BIT        = 0x00080000;
      static const int ITEM_BIT           = 0x00040000;
      static const int BOT_BIT            = 0x00020000;
      static const int VEHICLE_BIT        = 0x00010000;

      /*
       * DYNAMIC OBJECTS' BITS (interal 0x0000ff00, config 0x000000f0)
       */

      // if the object is still and on a still surface, we won't handle physics for it
      static const int DISABLED_BIT       = 0x00008000;

      // if the object collided in the last step
      static const int HIT_BIT            = 0x00004000;

      // if the object is currently fricting
      static const int FRICTING_BIT       = 0x00001000;

      // if the the object lies or moves on a structure, terrain or non-dynamic object
      // (if on another dynamic object, we determine that with "lower" index)
      static const int ON_FLOOR_BIT       = 0x00000800;

      // if the object intersects with water
      static const int IN_WATER_BIT       = 0x00000400;

      // if the object is on ladder
      static const int ON_LADDER_BIT      = 0x00000200;

      // if the object is on ice (slipping surface)
      static const int ON_SLICK_BIT       = 0x00000100;

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
       * RENDER FLAGS (config 0x0000000f)
       */

      // if the object is blended and should be rendered at the end
      static const int BLEND_BIT          = 0x00000002;

      // wide frustum culling: object is represented some times larger to frustum culling
      // system than it really is;
      // how larger it is, is specified by Client::Render::RELEASED_CULL_FACTOR (default 5.0f)
      static const int WIDE_CULL_BIT      = 0x00000001;

      /*
       * STANDARD EVENT IDs
       */

      static const int EVENT_DESTROY      = 0;
      static const int EVENT_HIT          = 1;
      static const int EVENT_SPLASH       = 2;
      // not in use, but reserved for more convenient BasicAudio
      static const int EVENT_FRICTING     = 3;

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
        {}
      };

      /*
       * FIELDS
       */

    private:

      Object         *prev[1];     // the previous object in cell.objects and list
      Object         *next[1];     // the next object in cell.objects and list

    public:

      int            index;        // position in world.objects vector
      Cell           *cell;        // parent cell, null if not positioned in the world

      int            flags;
      int            oldFlags;

      ObjectClass    *type;

      // damage
      float          life;

      // events are used for reporting hits, friction & stuff and are cleared at the beginning of
      // the frame
      List<Event, 0> events;

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

      /**
       * Inflict damage to the object.
       * @param damage
       */
      void damage( float damage )
      {
        damage -= type->damageTreshold;

        if( damage > 0.0f ) {
          life -= damage * type->damageRatio;
        }
      }

      void destroy()
      {
        addEvent( EVENT_DESTROY, 1.0f );
        onDestroy();
      }

      /**
       * Called by physics engine when the object hits something.
       * @param hit Hit struct filled with collision data
       * @param hitMomentum momentum of the object projected to hit normal
       */
      void hit( const Hit *hit, float hitMomentum )
      {
        events << new Event( EVENT_HIT, hitMomentum );
        flags |= HIT_BIT;

        damage( hitMomentum * hitMomentum );

        if( flags & HIT_FUNC_BIT ) {
          onHit( hit, hitMomentum );
        }
      }

      /**
       * Perform object update. One can implement onUpdate function to do some custom stuff.
       */
      void update()
      {
        events.free();

        if( flags & UPDATE_FUNC_BIT ) {
          onUpdate();
        }
        oldFlags = flags;
      }

      void use( Bot *user )
      {
        onUse( user );
      }

    protected:

      virtual void onDestroy();
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
