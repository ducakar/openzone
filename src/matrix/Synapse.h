/*
 *  Synapse.h
 *
 *  World actions scheduler.
 *    World should not be touched directly for adding/removing objects. Because objects may have
 *  hold references to each other (by indices), after object removal a full world update must pass,
 *  so that all references to the removed object are cleared. (On each update every reference must
 *  be checked. If it points to a null object slot, the target object has been removed. On order for
 *  this to work, a full world update must pass before another objects occupies a freed slot or
 *  the reference may point to a new object, that may be of a different type that may result in a
 *  program crash or at least a real mess.)
 *    The second reason is that world update may be multithreaded and adding an object in the middle
 *  of an update may put it in a cell that is handled by some other thread, which may result in
 *  a nasty-and-hard-to-discover-and-hard-to-squash bug.
 *    The third reason is networking. In this mode client should not add/remove objects and similar
 *  mechanism to deliver object additions/removals would be needed anyway.
 *    For obvious reasons (networking support and the first reason), use and take interactions are
 *  exploiting this mechanism as well.
 *    When an object addition is scheduled in Synapse, a ticket is returned that can be used in the
 *  next cycle to retrieve index of added object. The index could also be predicted at schdule time,
 *  but it's not the case for client adds in a networkin game, so ticket system is mandatory.
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "World.h"

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

      Vector<int>        putStructsIndices;
      Vector<int>        putObjectsIndices;
      Vector<int>        putPartsIndices;

    private:

      Vector<Action>     actions;

      Vector<Object*>    addedObjects;
      Vector<Object*>    removedObjects;

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

      // re-position removed object that they can be seen by Render and Sound
      void reposition();
      // commit object removals
      void commit();
      // do deletes and clear lists for actions, additions, removals
      void clean();

      // clear tickets
      void clearTickets();

  };

  extern Synapse synapse;

  inline void Synapse::use( Bot *user, Object *target )
  {
    if( target->flags & Object::USE_FUNC_BIT ) {
      target->use( user );
      actions << Action( user->index, target->index );
    }
  }

  inline void Synapse::put( DynObject *obj )
  {
    assert( obj->parent == -1 );

    obj->flags &= ~Object::DISABLED_BIT;
    world.position( obj );
  }

  inline void Synapse::cut( DynObject *obj )
  {
    assert( obj->parent != -1 );

    world.unposition( obj );
  }

  inline int Synapse::add( Structure *str )
  {
    world.add( str );
    world.position( str );
    return str->index;
  }

  inline int Synapse::add( Object *obj )
  {
    world.add( obj );
    world.position( obj );
    addedObjects << obj;
    return obj->index;
  }

  inline int Synapse::add( Particle *part )
  {
    world.add( part );
    world.position( part );
    return part->index;
  }

  inline int Synapse::addStruct( const char *name, const Vec3 &p, Structure::Rotation rot )
  {
    return add( new Structure( p, translator.bspIndex( name ), rot ) );
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
    world.unposition( str );
    world.remove( str );
    delete str;
  }

  inline void Synapse::remove( Object *obj )
  {
    world.unposition( obj );
    obj->flags |= Object::REMOVED_BIT;
    removedObjects << obj;
  }

  inline void Synapse::removeCut( DynObject *obj )
  {
    assert( obj->cell == null );

    if( ~obj->flags & Object::REMOVED_BIT ) {
      obj->flags |= Object::REMOVED_BIT | Object::CUT_BIT;
      removedObjects << obj;
    }
  }

  inline void Synapse::remove( Particle *part )
  {
    world.unposition( part );
    world.remove( part );
    delete part;
  }

}
