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
 *  of an update may put it in a sector that is handled by some other thread, which may result in
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
 */

#pragma once

#include "World.h"

namespace oz
{

  class Nirvana;

  class Synapse
  {
    friend class Nirvana;

    public:

      struct Action
      {
        Object *target;
        int    action;

        Action() {}
        Action( Object *target_, int action_ ) : target( target_ ), action( action_ )
        {}
      };

    private:

      Vector<int>        putStructsIndices;
      Vector<int>        putObjectsIndices;
      Vector<int>        putPartsIndices;

    private:

      Vector<Structure*> putStructs;
      Vector<Object*>    putObjects;
      Vector<Particle*>  putParts;

      Vector<Structure*> cutStructs;
      Vector<Object*>    cutObjects;
      Vector<Particle*>  cutParts;

      Vector<Structure*> deleteStructs;
      Vector<Object*>    deleteObjects;
      Vector<Particle*>  deleteParts;

      Vector<Action>     actions;

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

      // schedule for addition in the world and return predicted world index
      int  put( Structure *str );
      int  put( Object *obj );
      int  put( Particle *part );

      // schedule for removal from physical world, but don't delete it
      void cut( Structure *str );
      void cut( Object *obj );
      void cut( Particle *part );

      // create an object, schedule for addition in the world and return predicted world index
      int  addStruct( const char *name, const Vec3 &p, Structure::Rotation rot );
      int  addObject( const char *name, const Vec3 &p );
      int  addPart( const Vec3 &p, const Vec3 &velocity, float rejection, float mass,
                    float lifeTime, float size, const Vec3 &color );

      // schedule for removal from physical world and delete it
      void remove( Structure *str );
      void remove( Object *obj );
      void remove( Particle *part );

      // interactions
      void activate( Object *target, int action );

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

      void globalActivate( Object *target, int action );

      // indices in World vectors after objects have been remotely added
      // (ticket is the integer returned by globalPut())
      int  getStructIndex( int ticket ) const;
      int  getObjectIndex( int ticket ) const;
      int  getPartIndex( int ticket ) const;

      // commit cuts/removals, clear cutXXX vectors
      void commit();
      // do deletes, clear deleteXXX vectors
      void doDeletes();
      // clear putXXX vectors
      void clear();

      // clear tickets
      void clearTickets();

  };

  extern Synapse synapse;

  inline int Synapse::put( Structure *str )
  {
    putStructs << str;

    world.put( str );
    world.position( str );
    return str->index;
  }

  inline int Synapse::put( Object *obj )
  {
    putObjects << obj;

    world.put( obj );
    world.position( obj );
    return obj->index;
  }

  inline int Synapse::put( Particle *part )
  {
    putParts << part;

    world.put( part );
    world.position( part );
    return part->index;
  }

  inline void Synapse::cut( Structure *str )
  {
    cutStructs << str;
  }

  inline void Synapse::cut( Object *obj )
  {
    cutObjects << obj;
  }

  inline void Synapse::cut( Particle *part )
  {
    cutParts << part;
  }

  inline int Synapse::addStruct( const char *name, const Vec3 &p, Structure::Rotation rot )
  {
    Structure *str = new Structure( p, translator.bspIndex( name ), rot );
    return put( str );
  }

  inline int Synapse::addObject( const char *name, const Vec3 &p )
  {
    Object *obj = translator.createObject( name, p );
    return put( obj );;
  }

  inline int Synapse::addPart( const Vec3 &p, const Vec3 &velocity, float rejection, float mass,
                               float lifeTime, float size, const Vec3 &color )
  {
    Particle *part = new Particle( p, velocity, rejection, mass, lifeTime, size, color );
    return put( part );
  }

  inline void Synapse::remove( Structure *str )
  {
    cutStructs << str;
    deleteStructs << str;
  }

  inline void Synapse::remove( Object *obj )
  {
    cutObjects << obj;
    deleteObjects << obj;
  }

  inline void Synapse::remove( Particle *part )
  {
    cutParts << part;
    deleteParts << part;
  }

  inline void Synapse::activate( Object *target, int action )
  {
    actions << Action( target, action );
  }

}
