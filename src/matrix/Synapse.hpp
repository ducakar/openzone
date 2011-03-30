/*
 *  Synapse.hpp
 *
 *  Orbis manipulation interface.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Orbis.hpp"

namespace oz
{

  class Bot;

  class Synapse
  {
    public:

      struct Action
      {
        int user;
        int target;

        explicit Action( int user_, int target_ ) : user( user_ ), target( target_ )
        {}
      };

      Vector<Action>  actions;

      Vector<int>     putObjects;
      Vector<int>     cutObjects;

      Vector<int>     addedStructs;
      Vector<int>     addedObjects;
      Vector<int>     addedParts;

      Vector<int>     removedStructs;
      Vector<int>     removedObjects;
      Vector<int>     removedParts;

      // isSingle XOR isServer XOR isClient

      // Singleplayer. No need to worry about object creation, deletion, synchronisation etc.
      bool isSingle;
      // If server, create notification about every object creation/deletion/put/cut/etc. That can
      // be later serialised and sent to clients.
      bool isServer;
      // If client, do not create or delete any objects by yourself. Every creation/deletion must be
      // ordered by server.
      bool isClient;

      Synapse();

      // interactions
      void use( Bot* user, Object* target );

      // schedule for position in the world
      void put( Dynamic* obj );

      // schedule for unposition from world
      void cut( Dynamic* obj );

      // create an object, schedule for addition in the world and return predicted world index
      int  addStruct( const char* name, const Point3& p, Struct::Rotation rot );
      int  addObject( const char* name, const Point3& p );
      int  addPart( const Point3& p, const Vec3& velocity, const Vec3& colour,
                    float restitution, float mass, float lifeTime );

      // schedule for removal from physical world and delete it
      void remove( Struct* str );
      void remove( Object* obj );
      void remove( Particle* part );

      // for removing inventory (cut) objects
      void removeCut( Dynamic* obj );

      void genParts( int number, const Point3& p,
                     const Vec3& velocity, float velocitySpread,
                     const Vec3& colour, float colourSpread,
                     float restitution, float mass, float lifeTime );

      // clear lists for actions, additions, removals
      void update();

      void load();
      void unload();

  };

  extern Synapse synapse;

}
