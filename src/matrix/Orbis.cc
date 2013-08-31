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
 * Index reusing: when an entity is removed, there may still be references to it (from other
 * models or from render or sound subsystems); that's why every cycle all references must
 * be checked if the slot they're pointing at (all references should be indices of a slot
 * in Orbis::structures/objects/fragments vectors). If the target slot is nullptr, the
 * referenced entity doesn't exist any more, so reference must be cleared. To make sure all
 * references can be checked that way, a full world update must pass before a slot is reused.
 * Otherwise an entity may be removed and immediately after that another added into it's slot;
 * when an another entity would retrieve the target entity via the reference: 1) it wouldn't get
 * the expected entity but a new one; that may result in program crash if the new one is not of
 * the same type, 2) it wouldn't detect the old entity has been removed/destroyed/whatever;
 * that may pose a big problem to rendering and audio subsystems as those must clear
 * models/audio objects of removed world objects.
 */

static int freeing;
static int waiting;

// [freeing]: vector for indices that are currently being freed
// [waiting]: indices that have been freed previous cycle; those can be reused next time
static List<int> strFreedIndices[2];
static List<int> objFreedIndices[2];
static List<int> fragFreedIndices[2];

// indices of slots that can be reused
static List<int> strAvailableIndices;
static List<int> objAvailableIndices;
static List<int> fragAvailableIndices;

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
  const_cast<BSP*>( bsp )->request();

  Struct* str;
  int     index;

  if( strAvailableIndices.isEmpty() ) {
    if( strLength == MAX_STRUCTS ) {
      soft_assert( false );
      return nullptr;
    }

    index = strLength;
    ++strLength;
  }
  else {
    index = strAvailableIndices.popLast();
  }

  str = new Struct( bsp, index, p, heading );
  structs[1 + index] = str;
  return str;
}

Object* Orbis::add( const ObjectClass* clazz, const Point& p, Heading heading )
{
  Object* obj;
  int     index;

  if( objAvailableIndices.isEmpty() ) {
    if( objLength == MAX_OBJECTS ) {
      soft_assert( false );
      return nullptr;
    }

    index = objLength;
    ++objLength;
  }
  else {
    index = objAvailableIndices.popLast();
  }

  obj = clazz->create( index, p, heading );
  objects[1 + index] = obj;

  if( obj->flags & Object::LUA_BIT ) {
    luaMatrix.registerObject( obj->index );
  }
  return obj;
}

Frag* Orbis::add( const FragPool* pool, const Point& p, const Vec3& velocity )
{
  Frag* frag;
  int   index;

  if( fragAvailableIndices.isEmpty() ) {
    if( fragLength == MAX_FRAGS ) {
      soft_assert( false );
      return nullptr;
    }

    index = fragLength;
    ++fragLength;
  }
  else {
    index = fragAvailableIndices.popLast();
  }

  frag = new Frag( pool, index, p, velocity );
  frags[1 + index] = frag;
  return frag;
}

void Orbis::remove( Struct* str )
{
  hard_assert( str->index >= 0 );

  strFreedIndices[freeing].add( str->index );
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

  objFreedIndices[freeing].add( obj->index );
  objects[1 + obj->index] = nullptr;

  delete obj;
}

void Orbis::remove( Frag* frag )
{
  hard_assert( frag->index >= 0 && frag->cell == nullptr );

  fragFreedIndices[freeing].add( frag->index );
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

void Orbis::update()
{
  strAvailableIndices.addAll( strFreedIndices[waiting].begin(), strFreedIndices[waiting].length() );
  strFreedIndices[waiting].clear();

  objAvailableIndices.addAll( objFreedIndices[waiting].begin(), objFreedIndices[waiting].length() );
  objFreedIndices[waiting].clear();

  fragAvailableIndices.addAll( fragFreedIndices[waiting].begin(),
                               fragFreedIndices[waiting].length() );
  fragFreedIndices[waiting].clear();

  freeing = !freeing;
  waiting = !waiting;

  caelum.update();
}

void Orbis::read( InputStream* istream )
{
  luaMatrix.read( istream );

  caelum.read( istream );
  terra.read( istream );

  int nStructs = istream->readInt();
  int nObjects = istream->readInt();
  int nFrags   = istream->readInt();

  for( int i = 0; i < nStructs; ++i ) {
    const char* name = istream->readString();
    const BSP*  bsp  = liber.bsp( name );

    const_cast<BSP*>( bsp )->request();

    Struct* str = new Struct( bsp, istream );

    position( str );
    structs[1 + str->index] = str;
  }

  for( int i = 0; i < nObjects; ++i ) {
    const char*        name  = istream->readString();
    const ObjectClass* clazz = liber.objClass( name );
    Object*            obj   = clazz->create( istream );
    const Dynamic*     dyn   = static_cast<const Dynamic*>( obj );

    // No need to register objects since Lua state is being deserialised.

    if( !( obj->flags & Object::DYNAMIC_BIT ) || dyn->parent < 0 ) {
      position( obj );
    }
    objects[1 + obj->index] = obj;
  }

  for( int i = 0; i < nFrags; ++i ) {
    const char*     name = istream->readString();
    const FragPool* pool = liber.fragPool( name );
    Frag*           frag = new Frag( pool, istream );

    position( frag );
    frags[1 + frag->index] = frag;
  }

  strLength  = istream->readInt();
  objLength  = istream->readInt();
  fragLength = istream->readInt();

  int n;

  n = istream->readInt();
  for( int i = 0; i < n; ++i ) {
    strFreedIndices[freeing].add( istream->readInt() );
  }
  n = istream->readInt();
  for( int i = 0; i < n; ++i ) {
    objFreedIndices[freeing].add( istream->readInt() );
  }
  n = istream->readInt();
  for( int i = 0; i < n; ++i ) {
    fragFreedIndices[freeing].add( istream->readInt() );
  }

  n = istream->readInt();
  for( int i = 0; i < n; ++i ) {
    strFreedIndices[waiting].add( istream->readInt() );
  }
  n = istream->readInt();
  for( int i = 0; i < n; ++i ) {
    objFreedIndices[waiting].add( istream->readInt() );
  }
  n = istream->readInt();
  for( int i = 0; i < n; ++i ) {
    fragFreedIndices[waiting].add( istream->readInt() );
  }

  n = istream->readInt();
  for( int i = 0; i < n; ++i ) {
    strAvailableIndices.add( istream->readInt() );
  }
  n = istream->readInt();
  for( int i = 0; i < n; ++i ) {
    objAvailableIndices.add( istream->readInt() );
  }
  n = istream->readInt();
  for( int i = 0; i < n; ++i ) {
    fragAvailableIndices.add( istream->readInt() );
  }
}

void Orbis::read( const JSON& json )
{
  caelum.read( json["caelum"] );
  terra.read( json["terra"] );

  const JSON& structsJSON = json["structs"];

  foreach( i, structsJSON.arrayCIter() ) {
    String name    = ( *i )["bsp"].asString();
    const BSP* bsp = liber.bsp( name );

    const_cast<BSP*>( bsp )->request();

    Struct* str = new Struct( bsp, *i );

    position( str );
    structs[1 + str->index] = str;

    strLength = max( strLength, 1 + str->index );
  }

  const JSON& objectsJSON = json["objects"];

  foreach( i, objectsJSON.arrayCIter() ) {
    String             name  = ( *i )["class"].asString();
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

    objLength = max( objLength, 1 + obj->index );
  }

  const JSON& fragsJSON = json["frags"];

  foreach( i, fragsJSON.arrayCIter() ) {
    String          name = ( *i )["pool"].asString();
    const FragPool* pool = liber.fragPool( name );
    Frag*           frag = new Frag( pool, *i );

    position( frag );
    frags[1 + frag->index] = frag;

    fragLength = max( fragLength, 1 + frag->index );
  }

  for( int i = 0; i < strLength; ++i ) {
    Struct* str = structs[1 + i];

    if( str == nullptr ) {
      strAvailableIndices.add( i );
    }
  }
  for( int i = 0; i < objLength; ++i ) {
    Object* obj = objects[1 + i];

    if( obj == nullptr ) {
      objAvailableIndices.add( i );
    }
  }
  for( int i = 0; i < fragLength; ++i ) {
    Frag* frag = frags[1 + i];

    if( frag == nullptr ) {
      fragAvailableIndices.add( i );
    }
  }
}

void Orbis::write( OutputStream* ostream ) const
{
  luaMatrix.write( ostream );

  caelum.write( ostream );
  terra.write( ostream );

  int nStructs = Struct::pool.length();
  int nObjects = Object::pool.length() + Dynamic::pool.length() + Weapon::pool.length() +
                 Bot::pool.length() + Vehicle::pool.length();
  int nFrags   = Frag::mpool.length();

  ostream->writeInt( nStructs );
  ostream->writeInt( nObjects );
  ostream->writeInt( nFrags );

  for( int i = 0; i < strLength; ++i ) {
    Struct* str = structs[1 + i];

    if( str != nullptr ) {
      ostream->writeString( str->bsp->name );
      str->write( ostream );
    }
  }
  for( int i = 0; i < objLength; ++i ) {
    Object* obj = objects[1 + i];

    if( obj != nullptr ) {
      ostream->writeString( obj->clazz->name );
      obj->write( ostream );
    }
  }
  for( int i = 0; i < fragLength; ++i ) {
    Frag* frag = frags[1 + i];

    if( frag != nullptr ) {
      ostream->writeString( frag->pool->name );
      frag->write( ostream );
    }
  }

  ostream->writeInt( strLength );
  ostream->writeInt( objLength );
  ostream->writeInt( fragLength );

  ostream->writeInt( strFreedIndices[freeing].length() );
  foreach( i, strFreedIndices[freeing].citer() ) {
    ostream->writeInt( *i );
  }
  ostream->writeInt( objFreedIndices[freeing].length() );
  foreach( i, objFreedIndices[freeing].citer() ) {
    ostream->writeInt( *i );
  }
  ostream->writeInt( fragFreedIndices[freeing].length() );
  foreach( i, fragFreedIndices[freeing].citer() ) {
    ostream->writeInt( *i );
  }

  ostream->writeInt( strFreedIndices[waiting].length() );
  foreach( i, strFreedIndices[waiting].citer() ) {
    ostream->writeInt( *i );
  }
  ostream->writeInt( objFreedIndices[waiting].length() );
  foreach( i, objFreedIndices[waiting].citer() ) {
    ostream->writeInt( *i );
  }
  ostream->writeInt( fragFreedIndices[waiting].length() );
  foreach( i, fragFreedIndices[waiting].citer() ) {
    ostream->writeInt( *i );
  }

  ostream->writeInt( strAvailableIndices.length() );
  foreach( i, strAvailableIndices.citer() ) {
    ostream->writeInt( *i );
  }
  ostream->writeInt( objAvailableIndices.length() );
  foreach( i, objAvailableIndices.citer() ) {
    ostream->writeInt( *i );
  }
  ostream->writeInt( fragAvailableIndices.length() );
  foreach( i, fragAvailableIndices.citer() ) {
    ostream->writeInt( *i );
  }
}

JSON Orbis::write() const
{
  JSON json( JSON::OBJECT );

  json.add( "caelum", caelum.write() );
  json.add( "terra", terra.write() );

  JSON& structsJSON = json.add( "structs", JSON::ARRAY );

  for( int i = 0; i < strLength; ++i ) {
    Struct* str = structs[1 + i];

    if( str != nullptr ) {
      structsJSON.add( str->write() );
    }
  }

  JSON& objectsJSON = json.add( "objects", JSON::ARRAY );

  for( int i = 0; i < objLength; ++i ) {
    Object* obj = objects[1 + i];

    if( obj != nullptr ) {
      objectsJSON.add( obj->write() );
    }
  }

  JSON& fragsJSON = json.add( "frags", JSON::ARRAY );

  for( int i = 0; i < fragLength; ++i ) {
    Frag* frag = frags[1 + i];

    if( frag != nullptr ) {
      fragsJSON.add( frag->write() );
    }
  }

  return json;
}

void Orbis::load()
{
  strFreedIndices[0].allocate( 4 );
  strFreedIndices[1].allocate( 4 );
  objFreedIndices[0].allocate( 64 );
  objFreedIndices[1].allocate( 64 );
  fragFreedIndices[0].allocate( 128 );
  fragFreedIndices[1].allocate( 128 );

  strAvailableIndices.allocate( 16 );
  objAvailableIndices.allocate( 256 );
  fragAvailableIndices.allocate( 512 );
}

void Orbis::unload()
{
  for( int i = 0; i < objLength; ++i ) {
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

  strLength  = 0;
  objLength  = 0;
  fragLength = 0;

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

  fragFreedIndices[0].clear();
  fragFreedIndices[0].deallocate();
  fragFreedIndices[1].clear();
  fragFreedIndices[1].deallocate();
  objFreedIndices[0].clear();
  objFreedIndices[0].deallocate();
  objFreedIndices[1].clear();
  objFreedIndices[1].deallocate();
  strFreedIndices[0].clear();
  strFreedIndices[0].deallocate();
  strFreedIndices[1].clear();
  strFreedIndices[1].deallocate();

  fragAvailableIndices.clear();
  fragAvailableIndices.deallocate();
  objAvailableIndices.clear();
  objAvailableIndices.deallocate();
  strAvailableIndices.clear();
  strAvailableIndices.deallocate();
}

void Orbis::init()
{
  Log::print( "Initialising Orbis ..." );

  freeing = 0;
  waiting = 1;

  mins = Point( -Orbis::DIM, -Orbis::DIM, -Orbis::DIM );
  maxs = Point( +Orbis::DIM, +Orbis::DIM, +Orbis::DIM );

  caelum.reset();
  terra.init();
  terra.reset();

  aFill( structs, 1 + MAX_STRUCTS, nullptr );
  aFill( objects, 1 + MAX_OBJECTS, nullptr );
  aFill( frags, 1 + MAX_FRAGS, nullptr );

  strLength  = 0;
  objLength  = 0;
  fragLength = 0;

  Log::printEnd( " OK" );
}

void Orbis::destroy()
{
  Log::println( "Destroying Orbis ... OK" );
}

Orbis orbis;

}
