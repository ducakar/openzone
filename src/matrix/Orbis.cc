/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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

#include "stable.hh"

#include "matrix/Orbis.hh"

#include "matrix/Library.hh"
#include "matrix/Lua.hh"
#include "matrix/Dynamic.hh"
#include "matrix/Bot.hh"
#include "matrix/Weapon.hh"
#include "matrix/Vehicle.hh"

namespace oz
{
namespace matrix
{

static_assert( Orbis::CELLS * Cell::SIZEI == Terra::QUADS * Terra::Quad::SIZEI,
               "Orbis and terrain size mismatch" );

Orbis orbis;

constexpr float Cell::SIZE;
constexpr float Cell::INV_SIZE;

constexpr float Orbis::DIM;

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

      cells[x][y].structs.excludeUO( short( str->index ) );
    }
  }
}

void Orbis::position( Object* obj )
{
  hard_assert( obj->cell == null );

  Cell* cell = getCell( obj->p );

  obj->cell = cell;
  obj->prev[0] = null;

  if( !cell->objects.isEmpty() ) {
    cell->objects.first()->prev[0] = obj;
  }

  cell->objects.add( obj );
}

void Orbis::unposition( Object* obj )
{
  hard_assert( obj->cell != null );

  Cell* cell = obj->cell;

  obj->cell = null;

  if( obj->next[0] != null ) {
    obj->next[0]->prev[0] = obj->prev[0];
  }

  cell->objects.remove( obj, obj->prev[0] );
}

void Orbis::reposition( Object* obj )
{
  hard_assert( obj->cell != null );

  Cell* oldCell = obj->cell;
  Cell* newCell = getCell( obj->p );

  if( newCell != oldCell ) {
    if( obj->next[0] != null ) {
      obj->next[0]->prev[0] = obj->prev[0];
    }

    oldCell->objects.remove( obj, obj->prev[0] );

    obj->cell = newCell;
    obj->prev[0] = null;

    if( !newCell->objects.isEmpty() ) {
      newCell->objects.first()->prev[0] = obj;
    }

    newCell->objects.add( obj );
  }
}

void Orbis::position( Frag* frag )
{
  hard_assert( frag->cell == null );

  Cell* cell = getCell( frag->p );

  frag->cell = cell;
  frag->prev[0] = null;

  if( !cell->frags.isEmpty() ) {
    cell->frags.first()->prev[0] = frag;
  }

  cell->frags.add( frag );
}

void Orbis::unposition( Frag* frag )
{
  hard_assert( frag->cell != null );

  Cell* cell = frag->cell;

  frag->cell = null;

  if( frag->next[0] != null ) {
    frag->next[0]->prev[0] = frag->prev[0];
  }

  cell->frags.remove( frag, frag->prev[0] );
}

void Orbis::reposition( Frag* frag )
{
  hard_assert( frag->cell != null );

  Cell* oldCell = frag->cell;
  Cell* newCell = getCell( frag->p );

  if( newCell != oldCell ) {
    if( frag->next[0] != null ) {
      frag->next[0]->prev[0] = frag->prev[0];
    }

    oldCell->frags.remove( frag, frag->prev[0] );

    frag->cell = newCell;
    frag->prev[0] = null;

    if( !newCell->frags.isEmpty() ) {
      newCell->frags.first()->prev[0] = frag;
    }

    newCell->frags.add( frag );
  }
}

Struct* Orbis::add( const BSP* bsp, const Point3& p, Heading heading )
{
  const_cast<BSP*>( bsp )->request();

  Struct* str;

  if( strAvailableIndices.isEmpty() ) {
    int index = structs.length();

    if( index == MAX_STRUCTS ) {
      return null;
    }

    str = new Struct( bsp, index, p, heading );
    structs.add( str );
  }
  else {
    int index = strAvailableIndices.popLast();

    str = new Struct( bsp, index, p, heading );
    structs[index] = str;
  }

  return str;
}

Object* Orbis::add( const ObjectClass* clazz, const Point3& p, Heading heading )
{
  Object* obj;

  if( objAvailableIndices.isEmpty() ) {
    int index = objects.length();

    if( index == MAX_OBJECTS ) {
      return null;
    }

    obj = clazz->create( index, p, heading );
    objects.add( obj );
  }
  else {
    int index = objAvailableIndices.popLast();

    obj = clazz->create( index, p, heading );
    objects[index] = obj;
  }

  if( obj->flags & Object::LUA_BIT ) {
    lua.registerObject( obj->index );
  }

  return obj;
}

Frag* Orbis::add( const FragPool* pool, const Point3& p, const Vec3& velocity )
{
  Frag* frag;

  if( fragAvailableIndices.isEmpty() ) {
    int index = frags.length();

    if( index == MAX_FRAGS ) {
      return null;
    }

    frag = new Frag( pool, index, p, velocity );
    frags.add( frag );
  }
  else {
    int index = fragAvailableIndices.popLast();

    frag = new Frag( pool, index, p, velocity );
    frags[index] = frag;
  }

  return frag;
}

void Orbis::remove( Struct* str )
{
  hard_assert( str->index >= 0 );

  strFreedIndices[freeing].add( str->index );
  structs[str->index] = null;

  delete str;

  const_cast<BSP*>( str->bsp )->release();
}

void Orbis::remove( Object* obj )
{
  hard_assert( obj->index >= 0 );
  hard_assert( obj->cell == null );

  if( obj->flags & Object::LUA_BIT ) {
    lua.unregisterObject( obj->index );
  }
  objFreedIndices[freeing].add( obj->index );
  objects[obj->index] = null;

  delete obj;
}

void Orbis::remove( Frag* frag )
{
  hard_assert( frag->index >= 0 );
  hard_assert( frag->cell == null );

  fragFreedIndices[freeing].add( frag->index );
  frags[frag->index] = null;

  delete frag;
}

void Orbis::update()
{
  strAvailableIndices.addAll( strFreedIndices[waiting], strFreedIndices[waiting].length() );
  strFreedIndices[waiting].clear();

  objAvailableIndices.addAll( objFreedIndices[waiting], objFreedIndices[waiting].length() );
  objFreedIndices[waiting].clear();

  fragAvailableIndices.addAll( fragFreedIndices[waiting], fragFreedIndices[waiting].length() );
  fragFreedIndices[waiting].clear();

  freeing = !freeing;
  waiting = !waiting;

  caelum.update();
}

void Orbis::read( InputStream* istream )
{
  hard_assert( structs.length() == 0 && objects.length() == 0 && frags.length() == 0 );

  lua.read( istream );

  terra.read( istream );
  caelum.read( istream );

  int nStructs = istream->readInt();
  int nObjects = istream->readInt();
  int nFrags   = istream->readInt();

  String  bspName;
  Struct* str;
  String  className;
  Object* obj;
  String  poolName;
  Frag*   frag;

  for( int i = 0; i < nStructs; ++i ) {
    bspName = istream->readString();

    if( bspName.isEmpty() ) {
      structs.add( null );
    }
    else {
      const BSP* bsp = library.bsp( bspName );

      const_cast<BSP*>( bsp )->request();

      str = new Struct( bsp, istream );
      structs.add( str );

      if( !position( str ) ) {
        throw Exception( "Orbis structure reading failed, too many structures per cell." );
      }
    }
  }
  for( int i = 0; i < nObjects; ++i ) {
    className = istream->readString();

    if( className.isEmpty() ) {
      objects.add( null );
    }
    else {
      const ObjectClass* clazz = library.objClass( className );

      obj = clazz->create( istream );
      objects.add( obj );

      // no need to register objects since Lua state is being deserialised

      bool isCut = istream->readBool();
      if( !isCut ) {
        position( obj );
      }
    }
  }
  for( int i = 0; i < nFrags; ++i ) {
    poolName = istream->readString();

    if( poolName.isEmpty() ) {
      frags.add( null );
    }
    else {
      const FragPool* pool = library.fragPool( poolName );

      frag = new Frag( pool, istream );
      frags.add( frag );
      position( frag );
    }
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

void Orbis::write( BufferStream* ostream ) const
{
  lua.write( ostream );

  terra.write( ostream );
  caelum.write( ostream );

  ostream->writeInt( structs.length() );
  ostream->writeInt( objects.length() );
  ostream->writeInt( frags.length() );

  Struct* str;
  Object* obj;
  Frag*   frag;

  for( int i = 0; i < structs.length(); ++i ) {
    str = structs[i];

    if( str == null ) {
      ostream->writeString( "" );
    }
    else {
      ostream->writeString( str->bsp->name );
      str->write( ostream );
    }
  }
  for( int i = 0; i < objects.length(); ++i ) {
    obj = objects[i];

    if( obj == null ) {
      ostream->writeString( "" );
    }
    else {
      ostream->writeString( obj->clazz->name );
      obj->write( ostream );
      ostream->writeBool( obj->cell == null );
    }
  }
  for( int i = 0; i < frags.length(); ++i ) {
    frag = frags[i];

    if( frag == null ) {
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

void Orbis::load()
{
  strFreedIndices[0].alloc( 4 );
  strFreedIndices[1].alloc( 4 );
  objFreedIndices[0].alloc( 64 );
  objFreedIndices[1].alloc( 64 );
  fragFreedIndices[0].alloc( 128 );
  fragFreedIndices[1].alloc( 128 );

  strAvailableIndices.alloc( 16 );
  objAvailableIndices.alloc( 256 );
  fragAvailableIndices.alloc( 512 );
}

void Orbis::unload()
{
  for( int i = 0; i < objects.length(); ++i ) {
    if( objects[i] != null && ( objects[i]->flags & Object::LUA_BIT ) ) {
      lua.unregisterObject( i );
    }
  }

  for( int i = 0; i < Orbis::CELLS; ++i ) {
    for( int j = 0; j < Orbis::CELLS; ++j ) {
      cells[i][j].structs.clear();
      cells[i][j].objects.clear();
      cells[i][j].frags.clear();
    }
  }

  structs.free();
  objects.free();
  frags.free();

  Struct::overlappingObjs.clear();
  Struct::overlappingObjs.dealloc();

  library.freeBSPs();

  strFreedIndices[0].clear();
  strFreedIndices[0].dealloc();
  strFreedIndices[1].clear();
  strFreedIndices[1].dealloc();
  objFreedIndices[0].clear();
  objFreedIndices[0].dealloc();
  objFreedIndices[1].clear();
  objFreedIndices[1].dealloc();
  fragFreedIndices[0].clear();
  fragFreedIndices[0].dealloc();
  fragFreedIndices[1].clear();
  fragFreedIndices[1].dealloc();

  strAvailableIndices.clear();
  strAvailableIndices.dealloc();
  objAvailableIndices.clear();
  objAvailableIndices.dealloc();
  fragAvailableIndices.clear();
  fragAvailableIndices.dealloc();

  Struct::pool.free();

  Object::Event::pool.free();
  Object::pool.free();
  Dynamic::pool.free();
  Weapon::pool.free();
  Bot::pool.free();
  Vehicle::pool.free();

  Frag::mpool.free();
}

void Orbis::init()
{
  log.print( "Initialising Orbis ..." );

  freeing = 0;
  waiting = 1;

  mins = Point3( -Orbis::DIM, -Orbis::DIM, -Orbis::DIM );
  maxs = Point3(  Orbis::DIM,  Orbis::DIM,  Orbis::DIM );

  terra.init();

  log.printEnd( " OK" );
}

void Orbis::free()
{}

}
}
