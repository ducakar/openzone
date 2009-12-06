/*
 *  Synapse.h
 *
 *  World manipulation interface.
 *    World should not be touched directly for adding/removing objects, because objects may have
 *  hold references to each other (by indices). After an object removal a full world update must
 *  pass, so that all references to the removed object are cleared. (On each update every reference
 *  must be checked. If it points to a null object slot, the target object has been removed.
 *  On order for this to work, a full world update must pass before another objects occupies a freed
 *  slot or the reference may point to a new object that may be of a different type. That may result
 *  in a program crash or at least a real mess.)
 *    The second reason is networking. In this mode client should not add/remove object, and,
 *  on server, every object addition/removal must be recorded to be later sent to clients. This
 *  mechanism is also used by nirvana to keep track of added objects.
 *    Third, the objects should not be deleted when removed as they are pointers to them in models
 *  and audio objects in render and sound subsystems. That's why delete operator is actually
 *  called from update(), after render and sound have synchronized and removed models/audio objects
 *  of removed world objects.
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "World.h"
#include "Bot.h"
#include "Collider.h"

namespace oz
{

  namespace nirvana
  {
    class Nirvana;
  }

  class Synapse
  {
    friend class nirvana::Nirvana;

    public:

      struct Action
      {
        int user;
        int target;

        Action() {}
        Action( int user_, int target_ ) : user( user_ ), target( target_ ) {}
      };

    private:

      Vector<Object*> deleteObjects;

      Vector<Action>  actions;

      Vector<int>     putObjects;
      Vector<int>     cutObjects;

      Vector<int>     addedStructs;
      Vector<int>     addedObjects;
      Vector<int>     addedParts;

      Vector<int>     removedStructs;
      Vector<int>     removedObjects;
      Vector<int>     removedParts;

      Vector<int>     putStructsIndices;
      Vector<int>     putObjectsIndices;
      Vector<int>     putPartsIndices;

    public:

      // isSingle XOR isServer XOR isClient

      // Singleplayer. No need to worry about object creation, deletion, synchronization etc.
      bool isSingle;
      // If server, create notification about every object creation/deletion/put/cut/etc. That can
      // be later serialized and sent to clients.
      bool isServer;
      // If client, do not create or delete any objects by yourself. Every creation/deletion must be
      // ordered by server.
      bool isClient;

      explicit Synapse();

      // interactions
      void use( Bot *user, Object *target );

      // schedule for position in the world
      void put( DynObject *obj );

      // schedule for unposition from world
      void cut( DynObject *obj );

      // create an object, schedule for addition in the world and return predicted world index
      int  add( Structure *str );
      int  add( Object *obj );
      int  add( Particle *part );

      // create an object, schedule for addition in the world and return predicted world index
      int  addStruct( const char *name, const Vec3 &p, Structure::Rotation rot );
      int  addObject( const char *name, const Vec3 &p );
      int  addPart( const Vec3 &p, const Vec3 &velocity, float rejection, float mass,
                    float lifeTime, const Vec3 &color );

      // schedule for removal from physical world and delete it
      void remove( Structure *str );
      void remove( Object *obj );
      void remove( Particle *part );

      // for removing inventory (cut) objects
      void removeCut( DynObject *obj );

      // client-initiated actions, returns a ticket that can be used to retrieve index of the
      // added object
      int  globalPut( Structure *str );
      int  globalPut( Object *obj );
      int  globalPut( Particle *part );

      void globalCut( Structure *str );
      void globalCut( Object *obj );
      void globalCut( Particle *part );

      void globalRemove( Structure *str );
      void globalRemove( Object *obj );
      void globalRemove( Particle *part );

      void globalUse( Bot *user, Object *target );

      // indices in World vectors after objects have been remotely added
      // (ticket is the integer returned by globalPut())
      int  getStructIndex( int ticket ) const;
      int  getObjectIndex( int ticket ) const;
      int  getPartIndex( int ticket ) const;

      void genParts( int number, const Vec3 &p,
                     const Vec3 &velocity, float velocitySpread,
                     float rejection, float mass, float lifeTime,
                     const Vec3 &color, float colorSpread );

      // do deletes and clear lists for actions, additions, removals
      void update();

      // clear tickets
      void clearTickets();

  };

  extern Synapse synapse;

  inline void Synapse::use( Bot *user, Object *target )
  {
    if( target->flags & Object::USE_FUNC_BIT ) {
      actions << Action( user->index, target->index );
      target->use( user );
    }
  }

  inline void Synapse::put( DynObject *obj )
  {
    assert( obj->index != -1 && obj->cell == null && obj->parent == -1 );

    putObjects << obj->index;
    world.position( obj );
  }

  inline void Synapse::cut( DynObject *obj )
  {
    assert( obj->index != -1 && obj->cell != null && obj->parent != -1 );

    obj->clearFlags();
    cutObjects << obj->index;
    world.unposition( obj );
  }

  inline int Synapse::add( Structure *str )
  {
    world.add( str );
    world.position( str );
    addedStructs << str->index;
    return str->index;
  }

  inline int Synapse::add( Object *obj )
  {
    world.add( obj );
    world.position( obj );
    addedObjects << obj->index;
    return obj->index;
  }

  inline int Synapse::add( Particle *part )
  {
    world.add( part );
    world.position( part );
    addedParts << part->index;
    return part->index;
  }

  inline int Synapse::addStruct( const char *name, const Vec3 &p, Structure::Rotation rot )
  {
    world.requestBSP( translator.bspIndex( name ) );
    return add( translator.createStruct( name, p, rot ) );
  }

  inline int Synapse::addObject( const char *name, const Vec3 &p )
  {
    return add( translator.createObject( name, p ) );
  }

  inline int Synapse::addPart( const Vec3 &p, const Vec3 &velocity, float rejection, float mass,
                               float lifeTime, const Vec3 &color )
  {
    return add( new Particle( p, velocity, rejection, mass, lifeTime, color ) );
  }

  inline void Synapse::remove( Structure *str )
  {
    assert( str->index != -1 );

    collider.touchOverlaps( str->toAABB(), 4.0f * EPSILON );

    removedStructs << str->index;
    world.unposition( str );
    world.remove( str );
    delete str;
  }

  inline void Synapse::remove( Object *obj )
  {
    assert( obj->index != -1 && obj->cell != null );

    if( ~obj->flags & Object::DYNAMIC_BIT ) {
      collider.touchOverlaps( *obj, 4.0f * EPSILON );
    }

    deleteObjects << obj;
    removedObjects << obj->index;
    world.unposition( obj );
    world.remove( obj );
  }

  inline void Synapse::removeCut( DynObject *obj )
  {
    assert( obj->index != -1 && obj->cell == null );

    deleteObjects << obj;
    removedObjects << obj->index;
    world.remove( obj );
  }

  inline void Synapse::remove( Particle *part )
  {
    assert( part->index != -1 );

    removedParts << part->index;
    world.unposition( part );
    world.remove( part );
    delete part;
  }

}
