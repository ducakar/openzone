/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 */

/**
 * @file matrix/Synapse.cc
 */

#include <matrix/Synapse.hh>

#include <matrix/Liber.hh>
#include <matrix/Collider.hh>
#include <matrix/Bot.hh>

namespace oz
{

Synapse::Synapse() :
  mode( SINGLE )
{}

void Synapse::use( Bot* user, Object* target )
{
  target->use( user );
}

void Synapse::trigger( Entity* target )
{
  target->trigger();
}

void Synapse::lock( Bot* user, Entity* target )
{
  target->lock( user );
}

void Synapse::put( Dynamic* obj )
{
  hard_assert( obj->index >= 0 && obj->cell == nullptr && obj->parent == -1 );

  orbis.position( obj );

  putObjects.add( obj->index );
}

void Synapse::cut( Dynamic* obj )
{
  hard_assert( obj->index >= 0 && obj->cell != nullptr && obj->parent >= 0 );

  obj->flags   &= ~( Object::TICK_CLEAR_MASK | Object::MOVE_CLEAR_MASK );
  obj->lower    = -1;
  obj->velocity = Vec3::ZERO;
  obj->momentum = Vec3::ZERO;

  orbis.unposition( obj );

  cutObjects.add( obj->index );
}

Struct* Synapse::add( const BSP* bsp, const Point& p, Heading heading, bool empty )
{
  Struct* str = orbis.add( bsp, p, heading );
  if( str == nullptr ) {
    return nullptr;
  }

  if( !orbis.position( str ) ) {
    orbis.remove( str );
    delete str;
    return nullptr;
  }

  addedStructs.add( str->index );

  if( !empty && str->bsp->nBoundObjects != 0 ) {
    str->boundObjects.allocate( bsp->nBoundObjects );

    for( int i = 0; i < str->bsp->nBoundObjects; ++i ) {
      const BSP::BoundObject& boundObj = str->bsp->boundObjects[i];

      Point   pos     = str->toAbsoluteCS( boundObj.pos );
      Heading heading = Heading( ( str->heading + boundObj.heading ) % 4 );

      Object* obj = orbis.add( boundObj.clazz, pos, heading );
      if( obj == nullptr ) {
        continue;
      }

      orbis.position( obj );
      str->boundObjects.add( obj->index );

      addedObjects.add( obj->index );
    }
  }

  return str;
}

Object* Synapse::add( const ObjectClass* clazz, const Point& p, Heading heading, bool empty )
{
  Object* obj = orbis.add( clazz, p, heading );
  if( obj == nullptr ) {
    return nullptr;
  }

  orbis.position( obj );

  addedObjects.add( obj->index );

  if( !empty ) {
    const List<const ObjectClass*>& defaultItems = obj->clazz->defaultItems;

    for( int i = 0; i < defaultItems.length(); ++i ) {
      Heading heading = Heading( Math::rand( 4 ) );
      Dynamic* item = static_cast<Dynamic*>( orbis.add( defaultItems[i], Point::ORIGIN, heading ) );

      if( item == nullptr ) {
        continue;
      }

      obj->items.add( item->index );
      item->parent = obj->index;

      addedObjects.add( item->index );
    }

    if( obj->flags & Object::BOT_BIT ) {
      const BotClass* botClazz = static_cast<const BotClass*>( obj->clazz );
      Bot* bot = static_cast<Bot*>( obj );

      if( uint( botClazz->weaponItem ) < uint( obj->items.length() ) ) {
        bot->weapon = bot->items[botClazz->weaponItem];
      }
    }
  }

  return obj;
}

Frag* Synapse::add( const FragPool* pool, const Point& p, const Vec3& velocity )
{
  Frag* frag = orbis.add( pool, p, velocity );
  if( frag == nullptr ) {
    return nullptr;
  }

  orbis.position( frag );
  addedFrags.add( frag->index );

  return frag;
}

void Synapse::gen( const FragPool* pool, int nFrags, const Bounds& bb, const Vec3& velocity )
{
  for( int i = 0; i < nFrags; ++i ) {
    // spawn the frag somewhere in the upper half of the structure's bounding box
    Point fragPos = Point( bb.mins.x + Math::rand() * ( bb.maxs.x - bb.mins.x ),
                           bb.mins.y + Math::rand() * ( bb.maxs.y - bb.mins.y ),
                           bb.mins.z + Math::rand() * ( bb.maxs.z - bb.mins.z ) );

    Frag*  frag = add( pool, fragPos, velocity );
    if( frag == nullptr ) {
      continue;
    }

    frag->velocity += Vec3( Math::normalRand() * pool->velocitySpread,
                            Math::normalRand() * pool->velocitySpread,
                            Math::normalRand() * pool->velocitySpread );

    frag->life     += Math::centralRand() * pool->lifeSpread;
  }
}

Struct* Synapse::addStruct( const char* bspName, const Point& p, Heading heading, bool empty )
{
  return add( liber.bsp( bspName ), p, heading, empty );
}

Object* Synapse::addObject( const char* className, const Point& p, Heading heading, bool empty )
{
  return add( liber.objClass( className ), p, heading, empty );
}

Frag* Synapse::addFrag( const char* poolName, const Point& p, const Vec3& velocity )
{
  return add( liber.fragPool( poolName ), p, velocity );
}

void Synapse::genFrags( const char* poolName, int nFrags, const Bounds& bb, const Vec3& velocity )
{
  gen( liber.fragPool( poolName ), nFrags, bb, velocity );
}

void Synapse::remove( Struct* str )
{
  hard_assert( str->index >= 0 );

  for( int i = 0; i < str->boundObjects.length(); ++i ) {
    Object* boundObj = orbis.objects[ str->boundObjects[i] ];

    if( boundObj != nullptr ) {
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
  hard_assert( obj->index >= 0 );

  for( int i = 0; i < obj->items.length(); ++i ) {
    Object* item = orbis.objects[ obj->items[i] ];

    if( item != nullptr ) {
      remove( item );
    }
  }

  removedObjects.add( obj->index );

  if( obj->flags & Object::DYNAMIC_BIT ) {
    if( obj->cell != nullptr ) {
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
  hard_assert( frag->index >= 0 );

  removedFrags.add( frag->index );

  orbis.unposition( frag );
  orbis.remove( frag );
}

void Synapse::removeStruct( int index )
{
  hard_assert( index >= 0 );

  Struct* str = orbis.structs[index];

  if( str != nullptr ) {
    remove( str );
  }
}

void Synapse::removeObject( int index )
{
  hard_assert( index >= 0 );

  Object* obj = orbis.objects[index];

  if( obj != nullptr ) {
    remove( obj );
  }
}

void Synapse::removeFrag( int index )
{
  hard_assert( index >= 0 );

  Frag* frag = orbis.frags[index];

  if( frag != nullptr ) {
    remove( frag );
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
  putObjects.allocate( 32 );
  cutObjects.allocate( 32 );

  addedStructs.allocate( 4 * 16 );
  addedObjects.allocate( 64 * 16 );
  addedFrags.allocate( 128 * 8 );

  removedStructs.allocate( 4 );
  removedObjects.allocate( 64 );
  removedFrags.allocate( 128 );
}

void Synapse::unload()
{
  putObjects.clear();
  putObjects.deallocate();
  cutObjects.clear();
  cutObjects.deallocate();

  addedStructs.clear();
  addedStructs.deallocate();
  addedObjects.clear();
  addedObjects.deallocate();
  addedFrags.clear();
  addedFrags.deallocate();

  removedStructs.clear();
  removedStructs.deallocate();
  removedObjects.clear();
  removedObjects.deallocate();
  removedFrags.clear();
  removedFrags.deallocate();
}

Synapse synapse;

}
