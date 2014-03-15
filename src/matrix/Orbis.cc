/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file matrix/Orbis.cc
 */

#include <matrix/Orbis.hh>

#include <matrix/Liber.hh>
#include <matrix/LuaMatrix.hh>
#include <matrix/Vehicle.hh>

namespace oz
{

static_assert( Orbis::CELLS * Cell::SIZE == Terra::QUADS * Terra::Quad::SIZE,
               "Orbis and terrain size mismatch" );

/*
 * Index reusing: when an entity (structure, object or fragment) is removed, there may still be
 * references to it from other entities or from render or sound subsystems; that's why every cycle
 * all references must be checked if the target still exists. To make sure all references can be
 * checked, one full world update must pass before a slot is reused. Otherwise an entity may be
 * removed and immediately after that another added into it's slot which might end in an inexpected
 * behaviour or even a crash if the new entity doesn't have the same type.
 *
 * To ensure that one full update passes between the updates we tag slots as 'freeing' when they are
 * freed and 'waiting' in the next world update. In the update after 'waiting' state they can be
 * reused again.
 */

static int freeing = 0;
static int waiting = 1;

static int lastStructIndex = -1;
static int lastObjectIndex = -1;
static int lastFragIndex   = -1;

static SBitset<Orbis::MAX_STRUCTS> pendingStructs[2];
static SBitset<Orbis::MAX_OBJECTS> pendingObjects[2];
static SBitset<Orbis::MAX_FRAGS>   pendingFrags[2];

bool Orbis::position( Struct* str )
{
  Span span = getInters( *str, EPSILON );

  for( int x = span.minX; x <= span.maxX; ++x ) {
    for( int y = span.minY; y <= span.maxY; ++y ) {
      if( cells[x][y].structs.length() == cells[x][y].structs.capacity() ) {
        soft_assert( false );
        return false;
      }
    }
  }

  for( int x = span.minX; x <= span.maxX; ++x ) {
    for( int y = span.minY; y <= span.maxY; ++y ) {
      hard_assert( !cells[x][y].structs.contains( short( str->index ) ) );

      cells[x][y].structs.add( short( str->index ) );
    }
  }

  return true;
}

void Orbis::unposition( Struct* str )
{
  Span span = getInters( *str, EPSILON );

  for( int x = span.minX; x <= span.maxX; ++x ) {
    for( int y = span.minY; y <= span.maxY; ++y ) {
      hard_assert( cells[x][y].structs.contains( short( str->index ) ) );

      cells[x][y].structs.excludeUnordered( short( str->index ) );
    }
  }
}

void Orbis::position( Object* obj )
{
  hard_assert( obj->cell == nullptr );

  Cell* cell = getCell( obj->p );

  obj->cell = cell;
  obj->prev[0] = nullptr;

  if( !cell->objects.isEmpty() ) {
    cell->objects.first()->prev[0] = obj;
  }

  cell->objects.add( obj );
}

void Orbis::unposition( Object* obj )
{
  hard_assert( obj->cell != nullptr );

  Cell* cell = obj->cell;

  obj->cell = nullptr;

  if( obj->next[0] != nullptr ) {
    obj->next[0]->prev[0] = obj->prev[0];
  }

  cell->objects.erase( obj, obj->prev[0] );
}

void Orbis::position( Frag* frag )
{
  hard_assert( frag->cell == nullptr );

  Cell* cell = getCell( frag->p );

  frag->cell = cell;
  frag->prev[0] = nullptr;

  if( !cell->frags.isEmpty() ) {
    cell->frags.first()->prev[0] = frag;
  }

  cell->frags.add( frag );
}

void Orbis::unposition( Frag* frag )
{
  hard_assert( frag->cell != nullptr );

  Cell* cell = frag->cell;

  frag->cell = nullptr;

  if( frag->next[0] != nullptr ) {
    frag->next[0]->prev[0] = frag->prev[0];
  }

  cell->frags.erase( frag, frag->prev[0] );
}

Struct* Orbis::add( const BSP* bsp, const Point& p, Heading heading )
{
  int index = lastStructIndex + 1;

  while( structs[1 + index] != nullptr || pendingStructs[0].get( index ) ||
         pendingStructs[1].get( index ) )
  {
    if( index == lastStructIndex ) {
      // We have wrapped around => no slots available.
      soft_assert( false );
      return nullptr;
    }

    index = ( index + 1 ) % MAX_STRUCTS;
  }

  const_cast<BSP*>( bsp )->request();

  Struct* str = new Struct( bsp, index, p, heading );
  structs[1 + index] = str;

  lastStructIndex = index;
  return str;
}

Object* Orbis::add( const ObjectClass* clazz, const Point& p, Heading heading )
{
  int index = lastObjectIndex + 1;

  while( objects[1 + index] != nullptr || pendingObjects[0].get( index ) ||
         pendingObjects[1].get( index ) )
  {
    if( index == lastObjectIndex ) {
      // We have wrapped around => no slots available.
      soft_assert( false );
      return nullptr;
    }

    index = ( index + 1 ) % MAX_OBJECTS;
  }

  Object* obj = clazz->create( index, p, heading );
  objects[1 + index] = obj;

  if( obj->flags & Object::LUA_BIT ) {
    luaMatrix.registerObject( index );
  }

  lastObjectIndex = index;
  return obj;
}

Frag* Orbis::add( const FragPool* pool, const Point& p, const Vec3& velocity )
{
  int index = lastFragIndex + 1;

  while( frags[1 + index] != nullptr || pendingFrags[0].get( index ) ||
         pendingFrags[1].get( index ) )
  {
    if( index == lastFragIndex ) {
      // We have wrapped around => no slots available.
      soft_assert( false );
      return nullptr;
    }

    index = ( index + 1 ) % MAX_FRAGS;
  }

  Frag* frag = new Frag( pool, index, p, velocity );
  frags[1 + index] = frag;

  lastFragIndex = index;
  return frag;
}

void Orbis::remove( Struct* str )
{
  hard_assert( str->index >= 0 );

  pendingStructs[freeing].set( str->index );
  structs[1 + str->index] = nullptr;

  const_cast<BSP*>( str->bsp )->release();

  delete str;
}

void Orbis::remove( Object* obj )
{
  hard_assert( obj->index >= 0 && obj->cell == nullptr );

  if( obj->flags & Object::LUA_BIT ) {
    luaMatrix.unregisterObject( obj->index );
  }

  pendingObjects[freeing].set( obj->index );
  objects[1 + obj->index] = nullptr;

  delete obj;
}

void Orbis::remove( Frag* frag )
{
  hard_assert( frag->index >= 0 && frag->cell == nullptr );

  pendingFrags[freeing].set( frag->index );
  frags[1 + frag->index] = nullptr;

  delete frag;
}

void Orbis::reposition( Object* obj )
{
  hard_assert( obj->cell != nullptr );

  Cell* oldCell = obj->cell;
  Cell* newCell = getCell( obj->p );

  if( newCell != oldCell ) {
    if( obj->next[0] != nullptr ) {
      obj->next[0]->prev[0] = obj->prev[0];
    }

    oldCell->objects.erase( obj, obj->prev[0] );

    obj->cell = newCell;
    obj->prev[0] = nullptr;

    if( !newCell->objects.isEmpty() ) {
      newCell->objects.first()->prev[0] = obj;
    }

    newCell->objects.add( obj );
  }
}

void Orbis::reposition( Frag* frag )
{
  hard_assert( frag->cell != nullptr );

  Cell* oldCell = frag->cell;
  Cell* newCell = getCell( frag->p );

  if( newCell != oldCell ) {
    if( frag->next[0] != nullptr ) {
      frag->next[0]->prev[0] = frag->prev[0];
    }

    oldCell->frags.erase( frag, frag->prev[0] );

    frag->cell = newCell;
    frag->prev[0] = nullptr;

    if( !newCell->frags.isEmpty() ) {
      newCell->frags.first()->prev[0] = frag;
    }

    newCell->frags.add( frag );
  }
}

void Orbis::resetLastIndices()
{
  lastStructIndex = -1;
  lastObjectIndex = -1;
  lastFragIndex   = -1;
}

void Orbis::update()
{
  pendingStructs[waiting].clearAll();
  pendingObjects[waiting].clearAll();
  pendingFrags[waiting].clearAll();

  swap( freeing, waiting );

  caelum.update();
}

void Orbis::read( InputStream* is )
{
  luaMatrix.read( is );

  caelum.read( is );
  terra.read( is );

  int nStructs = is->readInt();
  int nObjects = is->readInt();
  int nFrags   = is->readInt();

  for( int i = 0; i < nStructs; ++i ) {
    const char* name = is->readString();
    const BSP*  bsp  = liber.bsp( name );

    const_cast<BSP*>( bsp )->request();

    Struct* str = new Struct( bsp, is );

    position( str );
    structs[1 + str->index] = str;
  }

  for( int i = 0; i < nObjects; ++i ) {
    const char*        name  = is->readString();
    const ObjectClass* clazz = liber.objClass( name );
    Object*            obj   = clazz->create( is );
    const Dynamic*     dyn   = static_cast<const Dynamic*>( obj );

    // No need to register objects since Lua state is being deserialised.

    if( !( obj->flags & Object::DYNAMIC_BIT ) || dyn->parent < 0 ) {
      position( obj );
    }
    objects[1 + obj->index] = obj;
  }

  for( int i = 0; i < nFrags; ++i ) {
    const char*     name = is->readString();
    const FragPool* pool = liber.fragPool( name );
    Frag*           frag = new Frag( pool, is );

    position( frag );
    frags[1 + frag->index] = frag;
  }

  lastStructIndex = is->readInt();
  lastObjectIndex = is->readInt();
  lastFragIndex   = is->readInt();

  is->readBitset( pendingStructs[freeing], pendingStructs[freeing].length() );
  is->readBitset( pendingStructs[waiting], pendingStructs[waiting].length() );
  is->readBitset( pendingObjects[freeing], pendingObjects[freeing].length() );
  is->readBitset( pendingObjects[waiting], pendingObjects[waiting].length() );
  is->readBitset( pendingFrags[freeing], pendingFrags[freeing].length() );
  is->readBitset( pendingFrags[waiting], pendingFrags[waiting].length() );
}

void Orbis::read( const JSON& json )
{
  caelum.read( json["caelum"] );
  terra.read( json["terra"] );

  const JSON& structsJSON = json["structs"];

  foreach( i, structsJSON.arrayCIter() ) {
    String name    = ( *i )["bsp"].get( "" );
    const BSP* bsp = liber.bsp( name );

    const_cast<BSP*>( bsp )->request();

    Struct* str = new Struct( bsp, *i );

    position( str );
    structs[1 + str->index] = str;
  }

  const JSON& objectsJSON = json["objects"];

  foreach( i, objectsJSON.arrayCIter() ) {
    String             name  = ( *i )["class"].get( "" );
    const ObjectClass* clazz = liber.objClass( name );
    Object*            obj   = clazz->create( *i );
    Dynamic*           dyn   = static_cast<Dynamic*>( obj );

    if( obj->flags & Object::LUA_BIT ) {
      luaMatrix.registerObject( obj->index );
    }

    if( !( obj->flags & Object::DYNAMIC_BIT ) || dyn->parent < 0 ) {
      position( obj );
    }
    objects[1 + obj->index] = obj;
  }

  const JSON& fragsJSON = json["frags"];

  foreach( i, fragsJSON.arrayCIter() ) {
    String          name = ( *i )["pool"].get( "" );
    const FragPool* pool = liber.fragPool( name );
    Frag*           frag = new Frag( pool, *i );

    position( frag );
    frags[1 + frag->index] = frag;
  }
}

void Orbis::write( OutputStream* os ) const
{
  luaMatrix.write( os );

  caelum.write( os );
  terra.write( os );

  int nStructs = Struct::pool.length();
  int nObjects = Object::pool.length() + Dynamic::pool.length() + Weapon::pool.length() +
                 Bot::pool.length() + Vehicle::pool.length();
  int nFrags   = Frag::mpool.length();

  os->writeInt( nStructs );
  os->writeInt( nObjects );
  os->writeInt( nFrags );

  for( int i = 0; i < MAX_STRUCTS; ++i ) {
    Struct* str = structs[1 + i];

    if( str != nullptr ) {
      os->writeString( str->bsp->name );
      str->write( os );
    }
  }
  for( int i = 0; i < MAX_OBJECTS; ++i ) {
    Object* obj = objects[1 + i];

    if( obj != nullptr ) {
      os->writeString( obj->clazz->name );
      obj->write( os );
    }
  }
  for( int i = 0; i < MAX_FRAGS; ++i ) {
    Frag* frag = frags[1 + i];

    if( frag != nullptr ) {
      os->writeString( frag->pool->name );
      frag->write( os );
    }
  }

  os->writeInt( lastStructIndex );
  os->writeInt( lastObjectIndex );
  os->writeInt( lastFragIndex );

  os->writeBitset( pendingStructs[freeing], pendingStructs[freeing].length() );
  os->writeBitset( pendingStructs[waiting], pendingStructs[waiting].length() );
  os->writeBitset( pendingObjects[freeing], pendingObjects[freeing].length() );
  os->writeBitset( pendingObjects[waiting], pendingObjects[waiting].length() );
  os->writeBitset( pendingFrags[freeing], pendingFrags[freeing].length() );
  os->writeBitset( pendingFrags[waiting], pendingFrags[waiting].length() );
}

JSON Orbis::write() const
{
  JSON json( JSON::OBJECT );

  json.add( "caelum", caelum.write() );
  json.add( "terra", terra.write() );

  JSON& structsJSON = json.add( "structs", JSON::ARRAY );

  for( int i = 0; i < MAX_STRUCTS; ++i ) {
    Struct* str = structs[1 + i];

    if( str != nullptr ) {
      structsJSON.add( str->write() );
    }
  }

  JSON& objectsJSON = json.add( "objects", JSON::ARRAY );

  for( int i = 0; i < MAX_OBJECTS; ++i ) {
    Object* obj = objects[1 + i];

    if( obj != nullptr ) {
      objectsJSON.add( obj->write() );
    }
  }

  JSON& fragsJSON = json.add( "frags", JSON::ARRAY );

  for( int i = 0; i < MAX_FRAGS; ++i ) {
    Frag* frag = frags[1 + i];

    if( frag != nullptr ) {
      fragsJSON.add( frag->write() );
    }
  }

  return json;
}

void Orbis::load()
{}

void Orbis::unload()
{
  for( int i = 0; i < MAX_OBJECTS; ++i ) {
    if( objects[1 + i] != nullptr && ( objects[1 + i]->flags & Object::LUA_BIT ) ) {
      luaMatrix.unregisterObject( i );
    }
  }

  for( int i = 0; i < Orbis::CELLS; ++i ) {
    for( int j = 0; j < Orbis::CELLS; ++j ) {
      cells[i][j].structs.clear();
      cells[i][j].objects.clear();
      cells[i][j].frags.clear();
    }
  }

  hard_assert( structs[0] == nullptr && objects[0] == nullptr && frags[0] == nullptr );

  aFree( &frags[1], MAX_FRAGS );
  aFill( &frags[1], MAX_FRAGS, nullptr );
  aFree( &objects[1], MAX_OBJECTS );
  aFill( &objects[1], MAX_OBJECTS, nullptr );
  aFree( &structs[1], MAX_STRUCTS );
  aFill( &structs[1], MAX_STRUCTS, nullptr );

  terra.reset();
  caelum.reset();

  Frag::mpool.free();

  Object::Event::pool.free();
  Object::pool.free();
  Dynamic::pool.free();
  Weapon::pool.free();
  Bot::pool.free();
  Vehicle::pool.free();

  Struct::pool.free();

  liber.freeBSPs();

  lastStructIndex = -1;
  lastObjectIndex = -1;
  lastFragIndex   = -1;

  pendingStructs[0].clearAll();
  pendingStructs[1].clearAll();
  pendingObjects[0].clearAll();
  pendingObjects[1].clearAll();
  pendingFrags[0].clearAll();
  pendingFrags[1].clearAll();
}

void Orbis::init()
{
  Log::print( "Initialising Orbis ..." );

  mins = Point( -Orbis::DIM, -Orbis::DIM, -Orbis::DIM );
  maxs = Point( +Orbis::DIM, +Orbis::DIM, +Orbis::DIM );

  caelum.reset();
  terra.init();
  terra.reset();

  Log::printEnd( " OK" );
}

void Orbis::destroy()
{
  Log::println( "Destroying Orbis ... OK" );
}

Orbis orbis;

}
