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
#include "matrix/Bot.hpp"
#include "matrix/Collider.hpp"

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

        explicit Action( int user_, int target_ ) : user( user_ ), target( target_ ) {}
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

      // Singleplayer. No need to worry about object creation, deletion, synchronisation etc.
      bool isSingle;
      // If server, create notification about every object creation/deletion/put/cut/etc. That can
      // be later serialised and sent to clients.
      bool isServer;
      // If client, do not create or delete any objects by yourself. Every creation/deletion must be
      // ordered by server.
      bool isClient;

      explicit Synapse();

      // interactions
      void use( Bot* user, Object* target );

      // schedule for position in the world
      void put( Dynamic* obj );

      // schedule for unposition from world
      void cut( Dynamic* obj );

      // create an object, schedule for addition in the world and return predicted world index
      int  addStruct( const char* name, const Point3& p, Structure::Rotation rot );
      int  addObject( const char* name, const Point3& p );
      int  addPart( const Point3& p, const Vec3& velocity, const Vec3& colour,
                    float restitution, float mass, float lifeTime );

      // schedule for removal from physical world and delete it
      void remove( Structure* str );
      void remove( Object* obj );
      void remove( Particle* part );

      // for removing inventory (cut) objects
      void removeCut( Dynamic* obj );

      // client-initiated actions, returns a ticket that can be used to retrieve index of the
      // added object
      int  globalPut( Structure* str );
      int  globalPut( Object* obj );
      int  globalPut( Particle* part );

      void globalCut( Structure* str );
      void globalCut( Object* obj );
      void globalCut( Particle* part );

      void globalRemove( Structure* str );
      void globalRemove( Object* obj );
      void globalRemove( Particle* part );

      void globalUse( Bot* user, Object* target );

      // indices in Orbis vectors after objects have been remotely added
      // (ticket is the integer returned by globalPut())
      int  getStructIndex( int ticket ) const;
      int  getObjectIndex( int ticket ) const;
      int  getPartIndex( int ticket ) const;

      void genParts( int number, const Point3& p,
                     const Vec3& velocity, float velocitySpread,
                     const Vec3& colour, float colourSpread,
                     float restitution, float mass, float lifeTime );

      // do deletes and clear lists for actions, additions, removals
      void update();

      // clear tickets
      void clearTickets();

  };

  extern Synapse synapse;

  inline void Synapse::use( Bot* user, Object* target )
  {
    if( target->flags & Object::USE_FUNC_BIT ) {
      actions.add( Action( user->index, target->index ) );
      target->use( user );
    }
  }

  inline void Synapse::put( Dynamic* obj )
  {
    assert( obj->index != -1 && obj->cell == null && obj->parent == -1 );

    putObjects.add( obj->index );
    orbis.position( obj );
  }

  inline void Synapse::cut( Dynamic* obj )
  {
    assert( obj->index != -1 && obj->cell != null && obj->parent != -1 );

    obj->flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT | Object::IN_WATER_BIT |
        Object::ON_LADDER_BIT | Object::ON_SLICK_BIT | Object::FRICTING_BIT | Object::HIT_BIT );
    obj->lower = -1;

    cutObjects.add( obj->index );
    orbis.unposition( obj );
  }

  inline int Synapse::addStruct( const char* name, const Point3& p, Structure::Rotation rot )
  {
    orbis.requestBSP( translator.bspIndex( name ) );

    int index = orbis.addStruct( name, p, rot );
    Structure* str = orbis.structs[index];

    if( !orbis.position( str ) ) {
      orbis.remove( str );
      delete str;
      return -1;
    }

    addedStructs.add( index );
    return index;
  }

  inline int Synapse::addObject( const char* name, const Point3& p )
  {
    int index = orbis.addObject( name, p );
    Object* obj = orbis.objects[index];
    assert( obj->cell == null );

    orbis.position( obj );
    obj->addEvent( Object::EVENT_CREATE, 1.0f );

    addedObjects.add( index );
    return index;
  }

  inline int Synapse::addPart( const Point3& p, const Vec3& velocity, const Vec3& colour,
                               float restitution, float mass, float lifeTime )
  {
    int index = orbis.addPart( p, velocity, colour, restitution, mass, lifeTime );
    Particle* part = orbis.parts[index];

    orbis.position( part );

    addedParts.add( index );
    return index;
  }

  inline void Synapse::remove( Structure* str )
  {
    assert( str->index != -1 );

    collider.touchOverlaps( str->toAABB(), 2.0f * EPSILON );

    removedStructs.add( str->index );
    orbis.unposition( str );
    orbis.remove( str );
    delete str;
  }

  inline void Synapse::remove( Object* obj )
  {
    assert( obj->index != -1 && obj->cell != null );

    if( !( obj->flags & Object::DYNAMIC_BIT ) ) {
      collider.touchOverlaps( *obj, 2.0f * EPSILON );
    }

    removedObjects.add( obj->index );
    deleteObjects.add( obj );
    orbis.unposition( obj );
    orbis.remove( obj );
  }

  inline void Synapse::removeCut( Dynamic* obj )
  {
    assert( obj->index != -1 && obj->cell == null );

    removedObjects.add( obj->index );
    deleteObjects.add( obj );
    orbis.remove( obj );
  }

  inline void Synapse::remove( Particle* part )
  {
    assert( part->index != -1 );

    removedParts.add( part->index );
    orbis.unposition( part );
    orbis.remove( part );
    delete part;
  }

}
