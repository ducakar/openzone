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

static int popMinimal( List<int>* list )
{
  hard_assert( !list->isEmpty() );

  int minimal  = 0;
  int minValue = list->first();

  for( int i = 1; i < list->length(); ++i ) {
    if( ( *list )[i] < minValue ) {
      minimal  = i;
      minValue = ( *list )[i];
    }
  }

  list->eraseUnordered( minimal );
  return minValue;
}

bool Orbis::position( Struct* str )
{
  Span span = getInters( *str, EPSILON );

  for( int x = span.minX; x <= span.maxX; ++x ) {
    for( int y = span.minY; y <= span.maxY; ++y ) {
      if( cells[x][y].structs.length() == cells[x][y].structs.capacity() ) {
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

  if( strAvailableIndices.isEmpty() ) {
    int index = structs.length();

    if( index == MAX_STRUCTS ) {
      return nullptr;
    }

    str = new Struct( bsp, index - 1, p, heading );
    structs.add( str );
  }
  else {
    int index = popMinimal( &strAvailableIndices );

    str = new Struct( bsp, index - 1, p, heading );
    structs[index] = str;
  }

  return str;
}

Object* Orbis::add( const ObjectClass* clazz, const Point& p, Heading heading )
{
  Object* obj;

  if( objAvailableIndices.isEmpty() ) {
    int index = objects.length();

    if( index == MAX_OBJECTS ) {
      return nullptr;
    }

    obj = clazz->create( index - 1, p, heading );
    objects.add( obj );
  }
  else {
    int index = popMinimal( &objAvailableIndices );

    obj = clazz->create( index - 1, p, heading );
    objects[index] = obj;
  }

  if( obj->flags & Object::LUA_BIT ) {
    luaMatrix.registerObject( obj->index );
  }

  return obj;
}

Frag* Orbis::add( const FragPool* pool, const Point& p, const Vec3& velocity )
{
  Frag* frag;

  if( fragAvailableIndices.isEmpty() ) {
    int index = frags.length();

    if( index == MAX_FRAGS ) {
      return nullptr;
    }

    frag = new Frag( pool, index - 1, p, velocity );
    frags.add( frag );
  }
  else {
    int index = popMinimal( &fragAvailableIndices );

    frag = new Frag( pool, index - 1, p, velocity );
    frags[index] = frag;
  }

  return frag;
}

void Orbis::remove( Struct* str )
{
  hard_assert( str->index >= 0 );

  strFreedIndices[freeing].add( 1 + str->index );
  structs[1 + str->index] = nullptr;

  const_cast<BSP*>( str->bsp )->release();

  delete str;
}

void Orbis::remove( Object* obj )
{
  hard_assert( obj->index >= 0 );
  hard_assert( obj->cell == nullptr );

  if( obj->flags & Object::LUA_BIT ) {
    luaMatrix.unregisterObject( obj->index );
  }
  objFreedIndices[freeing].add( 1 + obj->index );
  objects[1 + obj->index] = nullptr;

  delete obj;
}

void Orbis::remove( Frag* frag )
{
  hard_assert( frag->index >= 0 );
  hard_assert( frag->cell == nullptr );

  fragFreedIndices[freeing].add( 1 + frag->index );
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
  hard_assert( structs.length() == 1 && objects.length() == 1 && frags.length() == 1 );

  luaMatrix.read( istream );

  caelum.read( istream );
  terra.read( istream );

  int nStructs = istream->readInt();
  int nObjects = istream->readInt();
  int nFrags   = istream->readInt();

  for( int i = 0; i < nStructs; ++i ) {
    Struct* str = nullptr;

    const char* bspName = istream->readString();

    if( !String::isEmpty( bspName ) ) {
      const BSP* bsp = liber.bsp( bspName );
      const_cast<BSP*>( bsp )->request();

      str = new Struct( bsp, istream );
      position( str );
    }
    structs.add( str );
  }
  for( int i = 0; i < nObjects; ++i ) {
    Object* obj = nullptr;

    const char* className = istream->readString();

    if( !String::isEmpty( className ) ) {
      const ObjectClass* clazz = liber.objClass( className );

      obj = clazz->create( istream );

      bool isCut = istream->readBool();
      if( !isCut ) {
        position( obj );
      }

      // No need to register objects since Lua state is being deserialised.
    }
    objects.add( obj );
  }
  for( int i = 0; i < nFrags; ++i ) {
    Frag* frag = nullptr;

    const char* poolName = istream->readString();

    if( !String::isEmpty( poolName ) ) {
      const FragPool* pool = liber.fragPool( poolName );

      frag = new Frag( pool, istream );
      position( frag );
    }
    frags.add( frag );
  }

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

void Orbis::write( OutputStream* ostream ) const
{
  luaMatrix.write( ostream );

  caelum.write( ostream );
  terra.write( ostream );

  ostream->writeInt( structs.length() - 1 );
  ostream->writeInt( objects.length() - 1 );
  ostream->writeInt( frags.length() - 1 );

  Struct* str;
  Object* obj;
  Frag*   frag;

  for( int i = 1; i < structs.length(); ++i ) {
    str = structs[i];

    if( str == nullptr ) {
      ostream->writeString( "" );
    }
    else {
      ostream->writeString( str->bsp->name );
      str->write( ostream );
    }
  }
  for( int i = 1; i < objects.length(); ++i ) {
    obj = objects[i];

    if( obj == nullptr ) {
      ostream->writeString( "" );
    }
    else {
      ostream->writeString( obj->clazz->name );
      obj->write( ostream );
      ostream->writeBool( obj->cell == nullptr );
    }
  }
  for( int i = 1; i < frags.length(); ++i ) {
    frag = frags[i];

    if( frag == nullptr ) {
      ostream->writeString( "" );
    }
    else {
      ostream->writeString( frag->pool->name );
      frag->write( ostream );
    }
  }

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

void Orbis::read( const JSON& json )
{
  hard_assert( structs.length() == 1 && objects.length() == 1 && frags.length() == 1 );

  caelum.read( json["caelum"] );
  terra.read( json["terra"] );

  String bspName;
  String className;
  String poolName;

  const JSON& structsJSON = json["structs"];

  foreach( i, structsJSON.arrayCIter() ) {
    Struct* str = nullptr;

    if( !i->isEmpty() ) {
      bspName = ( *i )["bsp"].asString();

      const BSP* bsp = liber.bsp( bspName );
      const_cast<BSP*>( bsp )->request();

      str = new Struct( bsp, *i );
      position( str );
    }
    structs.add( str );
  }

  const JSON& objectsJSON = json["objects"];

  foreach( i, objectsJSON.arrayCIter() ) {
    Object* obj = nullptr;

    if( !i->isEmpty() ) {
      className = ( *i )["class"].asString();

      const ObjectClass* clazz = liber.objClass( className );

      obj = clazz->create( *i );

      bool isCut = ( *i )["isCut"].asBool();
      if( !isCut ) {
        position( obj );
      }

      if( obj->flags & Object::LUA_BIT ) {
        luaMatrix.registerObject( obj->index );
      }
    }
    objects.add( obj );
  }

  const JSON& fragsJSON = json["frags"];

  foreach( i, fragsJSON.arrayCIter() ) {
    Frag* frag = nullptr;

    if( !i->isEmpty() ) {
      poolName = ( *i )["pool"].asString();

      const FragPool* pool = liber.fragPool( poolName );

      frag = new Frag( pool, *i );
      position( frag );
    }
    frags.add( frag );
  }
}

JSON Orbis::write() const
{
  JSON json( JSON::OBJECT );

  json.add( "caelum", caelum.write() );
  json.add( "terra", terra.write() );

  int nStructs = structs.length() - 1;
  int nObjects = objects.length() - 1;
  int nFrags   = frags.length() - 1;

  while( structs[nStructs] == nullptr && nStructs > 0 ) {
    --nStructs;
  }
  while( objects[nObjects] == nullptr && nObjects > 0 ) {
    --nObjects;
  }
  while( frags[nFrags] == nullptr && nFrags > 0 ) {
    --nFrags;
  }

  Struct* str;
  Object* obj;
  Frag*   frag;

  JSON& structsJSON = json.add( "structs", JSON::ARRAY );

  for( int i = 1; i <= nStructs; ++i ) {
    str = structs[i];

    if( str == nullptr ) {
      structsJSON.add( JSON::OBJECT );
    }
    else {
      structsJSON.add( str->write() );
    }
  }

  JSON& objectsJSON = json.add( "objects", JSON::ARRAY );

  for( int i = 1; i <= nObjects; ++i ) {
    obj = objects[i];

    if( obj == nullptr ) {
      objectsJSON.add( JSON::OBJECT );
    }
    else {
      JSON& objectJSON = objectsJSON.add( obj->write() );
      objectJSON.add( "isCut", obj->cell == nullptr );
    }
  }

  JSON& fragsJSON = json.add( "frags", JSON::ARRAY );

  for( int i = 1; i <= nFrags; ++i ) {
    frag = frags[i];

    if( frag == nullptr ) {
      fragsJSON.add( JSON::OBJECT );
    }
    else {
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
  for( int i = 0; i < objects.length(); ++i ) {
    if( objects[i] != nullptr && ( objects[i]->flags & Object::LUA_BIT ) ) {
      luaMatrix.unregisterObject( i - 1 );
    }
  }

  for( int i = 0; i < Orbis::CELLS; ++i ) {
    for( int j = 0; j < Orbis::CELLS; ++j ) {
      cells[i][j].structs.clear();
      cells[i][j].objects.clear();
      cells[i][j].frags.clear();
    }
  }

  frags.free();
  objects.free();
  structs.free();

  structs.add( nullptr );
  objects.add( nullptr );
  frags.add( nullptr );

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
  Struct::overlappingObjs.clear();
  Struct::overlappingObjs.deallocate();

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

  structs.add( nullptr );
  objects.add( nullptr );
  frags.add( nullptr );

  Log::printEnd( " OK" );
}

void Orbis::destroy()
{
  Log::println( "Destroying Orbis ... OK" );
}

Orbis orbis;

}
