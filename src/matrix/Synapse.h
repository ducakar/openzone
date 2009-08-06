/*
 *  Synapse.h
 *
 *  World actions cheduler.
 *    World should not be touched directly for adding/removing objects. Because objects may have
 *  hold references to each other (by indices), after object removal a full world update must pass,
 *  so that all references to the removed object are cleared. (On each update every reference must
 *  be checked. If it points to a null object slot, the target object has been removed. On order for
 *  this to work, a full world update must pass before another objects occupies freed slot,
 *  the reference may point to a new object, that may be of a different type that may result in a
 *  program crash or at least a real mess.)
 *    The second reason is that world update may be multithreaded and adding an object in the middle
 *  of an update may put it in a sector that is handled by some other thread, which may result in
 *  a nasty-and-hard-to-discover-and-hard-to-squash bug.
 *    The third reason is networking. In this mode client should not add/remove objects and similar
 *  mechanism to deliver object additions/removals would be needed anyway.
 *    For obvious reasons (networking support and the first reason), use and take interactions are
 *  exploiting this mechanism as well.
 *    The world object should process synapse before world update in the matrix.
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  class Structure;
  class Object;
  class Particle;
  class Bot;

  namespace client
  {
    class Render;
    class SoundManager;
  }

  class Synapse
  {
    friend class World;
    friend class client::Render;
    friend class client::SoundManager;

    private:

      struct Action
      {
        Bot    *user;
        Object *target;

        Action() {}
        Action( Bot *user_, Object *target_ ) : user( user_ ), target( target_ ) {}
      };

      Vector<Structure*> putStructs;
      Vector<Object*>    putObjects;
      Vector<Particle*>  putParts;

      Vector<Structure*> cutStructs;
      Vector<Object*>    cutObjects;
      Vector<Particle*>  cutParts;

      Vector<Structure*> removeStructs;
      Vector<Object*>    removeObjects;
      Vector<Particle*>  removeParts;

      Vector<Action>     useActions;

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

      void clear();

      // put a created object into the physicsl world
      void put( Structure *str );
      void put( Object *obj );
      void put( Particle *part );

      // remove object from physical world, but don't delete it
      void cut( Structure *str );
      void cut( Object *obj );
      void cut( Particle *part );

      // remove object from physical world and delete it
      void remove( Structure *str );
      void remove( Object *obj );
      void remove( Particle *part );

      void use( Bot *user, Object *target );

  };

  extern Synapse synapse;

  inline void Synapse::put( Structure *str )
  {
    putStructs << str;
  }

  inline void Synapse::put( Object *obj )
  {
    putObjects << obj;
  }

  inline void Synapse::put( Particle *part )
  {
    putParts << part;
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

  inline void Synapse::remove( Structure *str )
  {
    removeStructs << str;
  }

  inline void Synapse::remove( Object *obj )
  {
    removeObjects << obj;
  }

  inline void Synapse::remove( Particle *part )
  {
    removeParts << part;
  }

  inline void Synapse::use( Bot *user, Object *target )
  {
    useActions << Action( user, target );
  }

}
