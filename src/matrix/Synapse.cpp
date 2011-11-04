/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file matrix/Synapse.cpp
 */

#include "stable.hpp"

#include "matrix/Synapse.hpp"

#include "matrix/Library.hpp"
#include "matrix/Collider.hpp"
#include "matrix/Bot.hpp"
#include "matrix/BotClass.hpp"

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
    target->use( user );
  }
}

void Synapse::put( Dynamic* obj )
{
  hard_assert( obj->index != -1 && obj->cell == null && obj->parent == -1 );

  orbis.position( obj );

  putObjects.add( obj->index );
}

void Synapse::cut( Dynamic* obj )
{
  hard_assert( obj->index != -1 && obj->cell != null && obj->parent != -1 );

  obj->flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT | Object::IN_WATER_BIT |
      Object::ON_LADDER_BIT | Object::ON_SLICK_BIT | Object::FRICTING_BIT | Object::HIT_BIT );
  obj->lower = -1;

  orbis.unposition( obj );

  cutObjects.add( obj->index );
}

int Synapse::addStruct( const char* name, const Point3& p, Heading heading )
{
  int id      = library.bspIndex( name );
  int index   = orbis.addStruct( id, p, heading );
  Struct* str = orbis.structs[index];

  if( !orbis.position( str ) ) {
    orbis.remove( str );
    delete str;
    return -1;
  }

  for( int i = 0; i < str->bsp->nBoundObjects; ++i ) {
    const BSP::BoundObject& boundObj = str->bsp->boundObjects[i];

    int objIndex = orbis.addObject( boundObj.clazz, str->toAbsoluteCS( boundObj.pos ),
                                    Heading( ( str->heading + boundObj.heading ) % 4 ) );
    Object* obj  = orbis.objects[objIndex];

    orbis.position( obj );

    str->boundObjects.add( objIndex );

    addedObjects.add( objIndex );
  }

  addedStructs.add( index );
  return index;
}

int Synapse::addObject( const char* name, const Point3& p, Heading heading )
{
  const ObjectClass* clazz = library.clazz( name );
  int index   = orbis.addObject( clazz, p, heading );
  Object* obj = orbis.objects[index];

  orbis.position( obj );

  int nItems = obj->clazz->items.length();
  if( nItems != 0 ) {
    for( int i = 0; i < nItems; ++i ) {
      const ObjectClass* clazz = library.clazz( obj->clazz->items[i] );
      int itemIndex = orbis.addObject( clazz, Point3::ORIGIN, NORTH );
      Dynamic* item = static_cast<Dynamic*>( orbis.objects[itemIndex] );

      obj->items.add( itemIndex );
      item->parent = obj->index;

      addedObjects.add( itemIndex );
    }

    if( obj->flags & Object::BOT_BIT ) {
      const BotClass* botClazz = static_cast<const BotClass*>( obj->clazz );
      Bot* bot = static_cast<Bot*>( obj );

      if( botClazz->weaponItem != -1 ) {
        bot->weapon = bot->items[botClazz->weaponItem];
      }
    }
  }

  addedObjects.add( index );
  return index;
}

int Synapse::addFrag( const Point3& p, const Vec3& velocity, const Vec3& colour,
                      float restitution, float mass, float lifeTime )
{
  int index = orbis.addFrag( p, velocity, colour, restitution, mass, lifeTime );
  Frag* frag = orbis.frags[index];

  orbis.position( frag );

  addedFrags.add( index );
  return index;
}

void Synapse::remove( Struct* str )
{
  hard_assert( str->index != -1 );

  removedStructs.add( str->index );

  collider.touchOverlaps( str->toAABB(), 4.0f * EPSILON );

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

void Synapse::remove( Frag* frag )
{
  hard_assert( frag->index != -1 );

  removedFrags.add( frag->index );

  orbis.unposition( frag );
  orbis.remove( frag );
}

void Synapse::genFrags( int number, const Point3& p,
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

    addFrag( p, velocity + velDisturb, colour + colourDisturb,
             restitution, mass, 0.5f * lifeTime + timeDisturb );
  }
}

void Synapse::update()
{
  putObjects.clear();
  cutObjects.clear();

  addedStructs.clear();
  addedObjects.clear();
  addedFrags.clear();

  removedStructs.clear();
  removedObjects.clear();
  removedFrags.clear();
}

void Synapse::load()
{
  putObjects.alloc( 32 );
  cutObjects.alloc( 32 );

  addedStructs.alloc( 4 * 16 );
  addedObjects.alloc( 64 * 16 );
  addedFrags.alloc( 128 * 8 );

  removedStructs.alloc( 4 );
  removedObjects.alloc( 64 );
  removedFrags.alloc( 128 );
}

void Synapse::unload()
{
  putObjects.clear();
  putObjects.dealloc();
  cutObjects.clear();
  cutObjects.dealloc();

  addedStructs.clear();
  addedStructs.dealloc();
  addedObjects.clear();
  addedObjects.dealloc();
  addedFrags.clear();
  addedFrags.dealloc();

  removedStructs.clear();
  removedStructs.dealloc();
  removedObjects.clear();
  removedObjects.dealloc();
  removedFrags.clear();
  removedFrags.dealloc();
}

}
}
