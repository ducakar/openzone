/*
 *  Synapse.hpp
 *
 *  World manipulation interface.
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "matrix/World.hpp"
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
      void use( Bot* user, Object* target );

      // schedule for position in the world
      void put( Dynamic* obj );

      // schedule for unposition from world
      void cut( Dynamic* obj );

      // create an object, schedule for addition in the world and return predicted world index
      int  addStruct( const char* name, const Vec4& p, Structure::Rotation rot );
      int  addObject( const char* name, const Vec4& p );
      int  addPart( const Vec4& p, const Vec4& velocity, const Vec4& color,
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

      // indices in World vectors after objects have been remotely added
      // (ticket is the integer returned by globalPut())
      int  getStructIndex( int ticket ) const;
      int  getObjectIndex( int ticket ) const;
      int  getPartIndex( int ticket ) const;

      void genParts( int number, const Vec4& p,
                     const Vec4& velocity, float velocitySpread,
                     const Vec4& color, float colorSpread,
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
      actions << Action( user->index, target->index );
      target->use( user );
    }
  }

  inline void Synapse::put( Dynamic* obj )
  {
    assert( obj->index != -1 && obj->cell == null && obj->parent == -1 );

    putObjects << obj->index;
    world.position( obj );
  }

  inline void Synapse::cut( Dynamic* obj )
  {
    assert( obj->index != -1 && obj->cell != null && obj->parent != -1 );

    obj->flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT | Object::IN_WATER_BIT |
        Object::ON_LADDER_BIT | Object::ON_SLICK_BIT | Object::FRICTING_BIT | Object::HIT_BIT );
    obj->lower = -1;

    cutObjects << obj->index;
    world.unposition( obj );
  }

  inline int Synapse::addStruct( const char* name, const Vec4& p, Structure::Rotation rot )
  {
    world.requestBSP( translator.bspIndex( name ) );

    int index = world.addStruct( name, p, rot );
    Structure* str = world.structs[index];

    if( !world.position( str ) ) {
      world.remove( str );
      delete str;
      return -1;
    }

    addedStructs << index;
    return index;
  }

  inline int Synapse::addObject( const char* name, const Vec4& p )
  {
    int index = world.addObject( name, p );
    Object* obj = world.objects[index];
    assert( obj->cell == null );

    world.position( obj );
    obj->addEvent( Object::EVENT_CREATE, 1.0f );

    addedObjects << index;
    return index;
  }

  inline int Synapse::addPart( const Vec4& p, const Vec4& velocity, const Vec4& color,
                               float restitution, float mass, float lifeTime )
  {
    int index = world.addPart( p, velocity, color, restitution, mass, lifeTime );
    Particle* part = world.parts[index];

    world.position( part );

    addedParts << index;
    return index;
  }

  inline void Synapse::remove( Structure* str )
  {
    assert( str->index != -1 );

    collider.touchOverlaps( str->toAABB(), 4.0f * EPSILON );

    removedStructs << str->index;
    world.unposition( str );
    world.remove( str );
    delete str;
  }

  inline void Synapse::remove( Object* obj )
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

  inline void Synapse::removeCut( Dynamic* obj )
  {
    assert( obj->index != -1 && obj->cell == null );

    deleteObjects << obj;
    removedObjects << obj->index;
    world.remove( obj );
  }

  inline void Synapse::remove( Particle* part )
  {
    assert( part->index != -1 );

    removedParts << part->index;
    world.unposition( part );
    world.remove( part );
    delete part;
  }

}
