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
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file matrix/Synapse.cpp
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
  target->use( user );
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

  obj->flags &= ~( Object::TICK_CLEAR_MASK | Object::MOVE_CLEAR_MASK );
  obj->lower = -1;

  orbis.unposition( obj );

  cutObjects.add( obj->index );
}

int Synapse::addStruct( const char* bspName, const Point3& p, Heading heading )
{
  return addStruct( library.bsp( bspName ), p, heading );
}

int Synapse::addObject( const char* className, const Point3& p, Heading heading )
{
  return addObject( library.objClass( className ), p, heading );
}

int Synapse::addFrag( const char* poolName, const Point3& p, const Vec3& velocity )
{
  return addFrag( library.fragPool( poolName ), p, velocity );
}

void Synapse::genFrags( const char* poolName, int nFrags, const Bounds& bb, const Vec3& velocity )
{
  genFrags( library.fragPool( poolName ), nFrags, bb, velocity );
}

int Synapse::addStruct( const BSP* bsp, const Point3& p, Heading heading )
{
  int     index = orbis.addStruct( bsp, p, heading );
  Struct* str   = orbis.structs[index];

  if( !orbis.position( str ) ) {
    orbis.remove( str );
    delete str;
    return -1;
  }
  addedStructs.add( index );

  for( int i = 0; i < str->bsp->nBoundObjects; ++i ) {
    const BSP::BoundObject& boundObj = str->bsp->boundObjects[i];

    Point3  pos      = str->toAbsoluteCS( boundObj.pos );
    Heading heading  = Heading( ( str->heading + boundObj.heading ) % 4 );

    int     objIndex = orbis.addObject( boundObj.clazz, pos, heading );
    Object* obj      = orbis.objects[objIndex];

    orbis.position( obj );

    str->boundObjects.add( objIndex );

    addedObjects.add( objIndex );
  }
  return index;
}

int Synapse::addObject( const ObjectClass* clazz, const Point3& p, Heading heading )
{
  int     index = orbis.addObject( clazz, p, heading );
  Object* obj   = orbis.objects[index];

  orbis.position( obj );
  addedObjects.add( index );

  const Vector<const ObjectClass*>& defaultItems = obj->clazz->defaultItems;

  if( !defaultItems.isEmpty() ) {
    for( int i = 0; i < defaultItems.length(); ++i ) {
      int      itemIndex = orbis.addObject( defaultItems[i], Point3::ORIGIN, NORTH );
      Dynamic* item      = static_cast<Dynamic*>( orbis.objects[itemIndex] );

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
  return index;
}

int Synapse::addFrag( const FragPool* pool, const Point3& p, const Vec3& velocity )
{
  int   index = orbis.addFrag( pool, p, velocity );
  Frag* frag  = orbis.frags[index];

  orbis.position( frag );
  addedFrags.add( index );

  return index;
}

void Synapse::genFrags( const FragPool* pool, int nFrags, const Bounds& bb, const Vec3& velocity )
{
  for( int i = 0; i < nFrags; ++i ) {
    // spawn the frag somewhere in the upper half of the structure's bounding box
    Point3 fragPos = Point3( bb.mins.x + Math::rand() * ( bb.maxs.x - bb.mins.x ),
                             bb.mins.y + Math::rand() * ( bb.maxs.y - bb.mins.y ),
                             bb.mins.z + Math::rand() * ( bb.maxs.z - bb.mins.z ) );

    int    index   = synapse.addFrag( pool, fragPos, velocity );
    Frag*  frag    = orbis.frags[index];

    frag->velocity += Vec3( Math::normalRand() * pool->velocitySpread,
                            Math::normalRand() * pool->velocitySpread,
                            Math::normalRand() * pool->velocitySpread );

    frag->life     += Math::centralRand() * pool->lifeSpread;
  }
}

void Synapse::remove( Struct* str )
{
  hard_assert( str->index != -1 );

  for( int i = 0; i < str->boundObjects.length(); ++i ) {
    Object* boundObj = orbis.objects[ str->boundObjects[i] ];

    if( boundObj != null ) {
      remove( boundObj );
    }
  }

  removedStructs.add( str->index );

  collider.touchOverlaps( str->toAABB(), 4.0f * EPSILON );
  orbis.unposition( str );
  orbis.remove( str );
}

void Synapse::remove( Object* obj )
{
  hard_assert( obj->index != -1 );

  for( int i = 0; i < obj->items.length(); ++i ) {
    Object* item = orbis.objects[ obj->items[i] ];

    if( item != null ) {
      remove( item );
    }
  }

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
