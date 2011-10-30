/*
 *  Synapse.hpp
 *
 *  Orbis manipulation interface.
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "matrix/Orbis.hpp"

namespace oz
{
namespace matrix
{

class Bot;

class Synapse
{
  public:

    enum Mode
    {
      SINGLE, ///< No need to worry about synchronisation of object creation, deletion,
              ///< interactions ...
      SERVER, ///< Create notification about every object creation/deletion/put/cut/etc that can
              ///< be later serialised and sent to clients.
      CLIENT  ///< Do not create or delete any objects by yourself, every creation/deletion must
              ///< be ordered by the server.
    };

    struct Action
    {
      int user;
      int target;

      Action() = default;

      explicit Action( int user_, int target_ ) : user( user_ ), target( target_ )
      {}
    };

    Vector<Action> actions;

    Vector<int>    putObjects;
    Vector<int>    cutObjects;

    Vector<int>    addedStructs;
    Vector<int>    addedObjects;
    Vector<int>    addedParts;

    Vector<int>    removedStructs;
    Vector<int>    removedObjects;
    Vector<int>    removedParts;

    Mode mode;

    Synapse();


    void use( Bot* user, Object* target );

    // schedule for position in the world
    void put( Dynamic* obj );

    // schedule for unposition from world
    void cut( Dynamic* obj );

    // create an object, schedule for addition in the world and return predicted world index
    int  addStruct( const char* name, const Point3& p, Heading heading );
    int  addObject( const char* name, const Point3& p, Heading heading );
    int  addPart( const Point3& p, const Vec3& velocity, const Vec3& colour,
                  float restitution, float mass, float lifeTime );

    // schedule for removal from physical world and delete it
    void remove( Struct* str );
    void remove( Object* obj );
    void remove( Particle* part );

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
}
