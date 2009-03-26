/*
 *  Object.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "bv.h"
#include "Event.h"
#include "Effect.h"
#include "Hit.h"

namespace oz
{

  struct Sector;
  struct ObjectClass;

  // static object abstract class
  class Object : public AABB
  {
    protected:

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

    public:

      // handle collisions for this object
      static const int CLIP_BIT = 0x00800000;

      /*
       *  DYNAMIC OBJECTS' BITS
       */

      // if the object is dynamic
      static const int DYNAMIC_BIT = 0x00400000;

      // if object is still and on a still surface, we won't handle physics for it
      static const int DISABLED_BIT = 0x00200000;

      // if the object is immune to gravity
      static const int HOVER_BIT = 0x00100000;

      // if object can push other objects in orthogonal direction of collision normal
      static const int PUSHING_BIT = 0x00080000;

      // if object is currently fricting
      static const int FRICTING_BIT = 0x00040000;

      // if object collided in last step
      static const int HIT_BIT = 0x00020000;

      // if the object lies or moves on a structure, terrain or non-dynamic object
      // (if on another dynamic object, we determine that with "lower" index)
      static const int ON_FLOOR_BIT = 0x00010000;

      // if object intersects with water
      static const int ON_WATER_BIT = 0x00008000;

      // if object center is in water
      static const int UNDER_WATER_BIT = 0x00004000;

      // if object can climb
      static const int CLIMBER_BIT = 0x00002000;

      // if object is on ladder
      static const int ON_LADDER_BIT = 0x00001000;

      /*
       *  RENDER FLAGS
       */

      // released frustum culling: object is represented some times larger to frustum culling
      // system than it really is;
      // how larger it is, is specified by Client::Render::RELEASED_CULL_FACTOR
      static const int RELEASED_CULL_BIT = 0x00000800;

      // determines visibility of the object for rendering
      // (e.g. ghosts are visible by other spirits, but not by alive units)
      static const int SPIRIT_BIT = 0x00000400;

      // if object is blended (then it should be rendered at the end)
      static const int BLEND_BIT = 0x00000200;

    public:

      static const int BOT_BIT = 0x00000080;
      static const int WATER_BIT = 0x00000040;

      /*
       *  FIELDS
       */

      int     index;        // position in world.objects vector
      Sector  *sector;      // parent sector, null if not positioned in the world

      Object  *prev[1];     // previous object in sector.objects list
      Object  *next[1];     // next object in sector.objects list

      int     flags;
      int     oldFlags;

      ObjectClass *type;

      // damage
      float   damage;

      // rotation around z axis
      float   rotZ;

      // events are cleared at the beginning of next update (used for non-continuous sounds)
      List<Event, 0> events;
      // effects are similar to events, but must be manually creared (used for continuous sounds)
      List<Effect, 0> effects;

    public:

      Object() : index( -1 ), sector( null ), rotZ( 0.0f )
      {}

      virtual ~Object();

      void addEffect( int id )
      {
        effects << new Effect( id );
      }

      void cleanEffects()
      {
        effects.free();
      }

      void update()
      {
        events.free();

        if( flags & UPDATE_FUNC_BIT ) {
          onUpdate();
        }
      }

      void hit( const Hit *hit )
      {
        if( flags & HIT_FUNC_BIT ) {
          onHit( hit );
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

      /**
       * Create a proxy object that handles graphic rendering of this object.
       * This function should be implemented in proxy class implementation in client part of code.
       * @return
       */
      virtual void createModel()
      {
        assert( false );
      }

    protected:

      virtual void onUpdate()
      {}

      virtual void onHit( const Hit* )
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

      virtual void load( FILE *stream );
      virtual void save( FILE *stream );
  };

}
