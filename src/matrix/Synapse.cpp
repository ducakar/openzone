/*
 *  Synapse.cpp
 *
 *  Orbis manipulation interface.
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Synapse.hpp"

#include "matrix/Library.hpp"
#include "matrix/Collider.hpp"
#include "matrix/Bot.hpp"

namespace oz
{
namespace matrix
{

Synapse synapse;

Synapse::Synapse() : mode( SINGLE )
{}

void Synapse::use( Bot* user, Object* target )
{
  if( target->flags & Object::USE_FUNC_BIT ) {
    actions.add( Action( user->index, target->index ) );
    target->use( user );
  }
}

void Synapse::put( Dynamic* obj )
{
  hard_assert( obj->index != -1 && obj->cell == null && obj->parent == -1 );

  putObjects.add( obj->index );
  orbis.position( obj );
}

void Synapse::cut( Dynamic* obj )
{
  hard_assert( obj->index != -1 && obj->cell != null && obj->parent != -1 );

  obj->flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT | Object::IN_WATER_BIT |
      Object::ON_LADDER_BIT | Object::ON_SLICK_BIT | Object::FRICTING_BIT | Object::HIT_BIT );
  obj->lower = -1;

  cutObjects.add( obj->index );
  orbis.unposition( obj );
}

int Synapse::addStruct( const char* name, const Point3& p, Heading heading )
{
  int index = orbis.addStruct( name, p, heading );
  Struct* str = orbis.structs[index];

  if( !orbis.position( str ) ) {
    orbis.remove( str );
    delete str;
    return -1;
  }

  addedStructs.add( index );
  return index;
}

int Synapse::addObject( const char* name, const Point3& p, Heading heading )
{
  int index = orbis.addObject( name, p, heading );
  Object* obj = orbis.objects[index];
  hard_assert( obj->cell == null );

  orbis.position( obj );

  addedObjects.add( index );
  return index;
}

int Synapse::addPart( const Point3& p, const Vec3& velocity, const Vec3& colour,
                             float restitution, float mass, float lifeTime )
{
  int index = orbis.addPart( p, velocity, colour, restitution, mass, lifeTime );
  Particle* part = orbis.parts[index];

  orbis.position( part );

  addedParts.add( index );
  return index;
}

void Synapse::remove( Struct* str )
{
  hard_assert( str->index != -1 );

  collider.touchOverlaps( str->toAABB(), 4.0f * EPSILON );

  removedStructs.add( str->index );
  orbis.unposition( str );
  orbis.remove( str );
}

void Synapse::remove( Object* obj )
{
  hard_assert( obj->index != -1 );

  removedObjects.add( obj->index );

  if( obj->flags & Object::DYNAMIC_BIT ) {
    if( obj->cell != null ) {
      orbis.unposition( obj );
    }
  }
  else {
    collider.touchOverlaps( *obj, 4.0f * EPSILON );
    orbis.unposition( obj );
  }
  orbis.remove( obj );
}

void Synapse::remove( Particle* part )
{
  hard_assert( part->index != -1 );

  removedParts.add( part->index );
  orbis.unposition( part );
  orbis.remove( part );
}

void Synapse::genParts( int number, const Point3& p,
                        const Vec3& velocity, float velocitySpread,
                        const Vec3& colour, float colourSpread,
                        float restitution, float mass, float lifeTime )
{
  float velocitySpread2 = velocitySpread / 2.0f;
  float colourSpread2 = colourSpread / 2.0f;

  for( int i = 0; i < number; ++i ) {
    Vec3 velDisturb = Vec3( velocitySpread * Math::rand() - velocitySpread2,
                            velocitySpread * Math::rand() - velocitySpread2,
                            velocitySpread * Math::rand() - velocitySpread2 );
    Vec3 colourDisturb = Vec3( colourSpread * Math::rand() - colourSpread2,
                               colourSpread * Math::rand() - colourSpread2,
                               colourSpread * Math::rand() - colourSpread2 );
    float timeDisturb = lifeTime * Math::rand();

    addPart( p, velocity + velDisturb, colour + colourDisturb,
             restitution, mass, 0.5f * lifeTime + timeDisturb );
  }
}

void Synapse::update()
{
  actions.clear();

  putObjects.clear();
  cutObjects.clear();

  addedStructs.clear();
  addedObjects.clear();
  addedParts.clear();

  removedStructs.clear();
  removedObjects.clear();
  removedParts.clear();
}

void Synapse::load()
{
  actions.alloc( 256 );

  putObjects.alloc( 32 );
  cutObjects.alloc( 32 );

  addedStructs.alloc( 4 * 16 );
  addedObjects.alloc( 64 * 16 );
  addedParts.alloc( 128 * 8 );

  removedStructs.alloc( 4 );
  removedObjects.alloc( 64 );
  removedParts.alloc( 128 );
}

void Synapse::unload()
{
  actions.clear();
  actions.dealloc();

  putObjects.clear();
  putObjects.dealloc();
  cutObjects.clear();
  cutObjects.dealloc();

  addedStructs.clear();
  addedStructs.dealloc();
  addedObjects.clear();
  addedObjects.dealloc();
  addedParts.clear();
  addedParts.dealloc();

  removedStructs.clear();
  removedStructs.dealloc();
  removedObjects.clear();
  removedObjects.dealloc();
  removedParts.clear();
  removedParts.dealloc();
}

}
}
