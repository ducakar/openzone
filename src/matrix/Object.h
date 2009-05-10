/*
 *  Object.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "bv.h"
#include "Net.h"

namespace oz
{

  struct Sector;
  struct ObjectClass;
  struct Hit;

  // static object abstract class
  class Object : public AABB
  {
    /*
     * Here various flag bits are set; the higher bits are used for flags that are internal flags
     * and should only be hardcoded in the engine and cannot be set in object class's configuration
     * file. The lower bits are set for object class in the corresponding configuration file. The
     * lower bits are used for that because of easier calculation.
     */

    public:

      // which bits are treated as "lower bits", all other are "higher bits"
      static const int CONFIG_BITS_MASK = 0x000000ff;

      /*
       *  FUNCTION FLAGS
       */

      // if the update method should be called each step
      static const int UPDATE_FUNC_BIT = 0x80000000;

      // if the onHit function should be called on hit
      static const int HIT_FUNC_BIT = 0x40000000;

      // if the onDestroy method is called when object is destroyed
      static const int DESTROY_FUNC_BIT = 0x20000000;

      // if the onUse method is called when we use the object (otherwise, nothing happens)
      static const int USE_FUNC_BIT = 0x01000000;

      // if the onPut method is called whed object is added to 3D world (ie. taken from inventory)
      static const int PUT_FUNC_BIT = 0x08000000;

      // if the onCut method is called when object is removed from 3D world (ie. put into inventory)
      static const int CUT_FUNC_BIT = 0x04000000;

      /*
       *  DYNAMIC OBJECTS' BITS
       */

      // if the object is dynamic
      static const int DYNAMIC_BIT = 0x00800000;

      // if the object is still and on a still surface, we won't handle physics for it
      static const int DISABLED_BIT = 0x00400000;

      // if the object collided in the last step
      static const int HIT_BIT = 0x00200000;

      // if the object is currently fricting
      static const int FRICTING_BIT = 0x00100000;

      // if the the object lies or moves on a structure, terrain or non-dynamic object
      // (if on another dynamic object, we determine that with "lower" index)
      static const int ON_FLOOR_BIT = 0x00080000;

      // if the object intersects with water
      static const int IN_WATER_BIT = 0x00040000;

      // if the object center is in water
      static const int UNDER_WATER_BIT = 0x00020000;

      // if the object is on ladder
      static const int ON_LADDER_BIT = 0x00010000;

      // if the object is on ice (slipping surface)
      static const int ON_SLICK_BIT = 0x00008000;

      // handle collisions for this object
      static const int CLIP_BIT = 0x00000001;

      // If the object is climber it is tested against ladder brushes and gains ON_LADDER_BIT if it
      // intersects with a ladder brush. Otherwise object is not affected by ladders.
      static const int CLIMBER_BIT = 0x00000002;

      // if the object can push other objects in orthogonal direction of collision normal
      static const int PUSHING_BIT = 0x00000004;

      // if the object is immune to gravity
      static const int HOVER_BIT = 0x00000008;

      /*
       *  TYPE FLAGS
       */

      // the object is derived from bot
      static const int BOT_BIT = 0x00000800;

      /*
       *  RENDER FLAGS
       */

      // released frustum culling: object is represented some times larger to frustum culling
      // system than it really is;
      // how larger it is, is specified by Client::Render::RELEASED_CULL_FACTOR
      static const int RELEASED_CULL_BIT = 0x00000010;

      // determines visibility of the object for rendering
      // (e.g. ghosts are visible by other spirits, but not by alive units)
      static const int SPIRIT_BIT = 0x00000020;

      // if the object is blended (then it should be rendered at the end)
      static const int BLEND_BIT = 0x00000040;

    private:

      struct Event : PoolAlloc<Event, 0>
      {
        int   id;
        Event *next[1];

        Event() {}
        explicit Event( int id_ ) : id( id_ ) {}
      };

      struct Effect : PoolAlloc<Effect, 0>
      {
        int    id;
        Effect *next[1];

        Effect() {}
        explicit Effect( int id_ ) : id( id_ ) {}
      };

    public:

      /*
       *  FIELDS
       */

      int    index;        // position in world.objects vector
      Sector *sector;      // parent sector, null if not positioned in the world

      Object *prev[1];     // previous object in sector.objects list
      Object *next[1];     // next object in sector.objects list

      int    flags;
      int    oldFlags;

      ObjectClass *type;

      // damage
      float   damage;

      // rotation around z axis
      float   rotZ;

      // events are cleared at the beginning of next update (used for non-continuous sounds)
      List<Event, 0> events;
      // effects are similar to events, but must be manually cleared (used for continuous sounds)
      List<Effect, 0> effects;

    public:

      explicit Object() : index( -1 ), sector( null ), rotZ( 0.0f )
      {}

      virtual ~Object();

      void addEvent( int id )
      {
        events << new Event( id );
      }

      void addEffect( int id )
      {
        effects << new Effect( id );
      }

      void update()
      {
        events.free();

        if( flags & UPDATE_FUNC_BIT ) {
          onUpdate();
        }
        oldFlags = flags;
      }

      void hit( const Hit *hit, float hitVelocity )
      {
        if( flags & HIT_FUNC_BIT ) {
          onHit( hit, hitVelocity );
        }
      }

      void destroy()
      {
        if( flags & DESTROY_FUNC_BIT ) {
          onDestroy();
        }
      }

      void use( Object *user )
      {
        if( flags & USE_FUNC_BIT ) {
          onUse( user );
        }
      }

      void put()
      {
        if( flags & PUT_FUNC_BIT ) {
          onPut();
        }
      }

      void cut()
      {
        if( flags & CUT_FUNC_BIT ) {
          onCut();
        }
      }

    protected:

      virtual void onUpdate()
      {}

      virtual void onHit( const Hit*, float )
      {}

      virtual void onDestroy()
      {}

      virtual void onUse( Object* )
      {}

      virtual void onPut()
      {}

      virtual void onCut()
      {}

    public:

      /*
       *  SERIALIZATION
       */

      virtual void readUpdate( Net::Packet *packet );
      virtual void writeUpdate( Net::Packet *packet );

  };

}
