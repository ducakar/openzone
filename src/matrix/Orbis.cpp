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
 * @file matrix/Orbis.cpp
 */

#include "stable.hpp"

#include "matrix/Orbis.hpp"

#include "matrix/Library.hpp"
#include "matrix/Lua.hpp"
#include "matrix/Dynamic.hpp"
#include "matrix/Bot.hpp"
#include "matrix/Weapon.hpp"
#include "matrix/Vehicle.hpp"

namespace oz
{
namespace matrix
{

Orbis orbis;

const float Cell::SIZE     = float( SIZEI );
const float Cell::INV_SIZE = 1.0f / float( SIZEI );
const float Orbis::DIM     = Cell::SIZE * MAX / 2.0f;

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

int Orbis::addStruct( const BSP* bsp, const Point3& p, Heading heading )
{
  int index;

  const_cast<BSP*>( bsp )->request();

  if( strAvailableIndices.isEmpty() ) {
    index = structs.length();
    structs.add( new Struct( bsp, index, p, heading ) );
  }
  else {
    index = strAvailableIndices.popLast();
    structs[index] = new Struct( bsp, index, p, heading );
  }
  return index;
}

int Orbis::addObject( const ObjectClass* clazz, const Point3& p, Heading heading )
{
  int index;

  if( objAvailableIndices.isEmpty() ) {
    index = objects.length();
    objects.add( clazz->create( index, p, heading ) );
  }
  else {
    index = objAvailableIndices.popLast();
    objects[index] = clazz->create( index, p, heading );
  }

  if( objects[index]->flags & Object::LUA_BIT ) {
    lua.registerObject( index );
  }
  return index;
}

int Orbis::addFrag( const Point3& p, const Vec3& velocity, const Vec3& colour,
                    float restitution, float mass, float lifeTime )
{
  int index;

  if( fragAvailableIndices.isEmpty() ) {
    index = frags.length();
    frags.add( new Frag( index, p, velocity, colour, restitution, mass, lifeTime ) );
  }
  else {
    index = fragAvailableIndices.popLast();
    frags[index] = new Frag( index, p, velocity, colour, restitution, mass, lifeTime );
  }
  return index;
}

void Orbis::remove( Struct* str )
{
  hard_assert( str->index >= 0 );

  BSP* bsp = const_cast<BSP*>( str->bsp );

  strFreedIndices[freeing].add( str->index );
  structs[str->index] = null;

  delete str;
  bsp->release();
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

  log.println( "Reading Orbis {" );
  log.indent();

  lua.read( istream );

  terra.read( istream );
  caelum.read( istream );

  int nStructs = istream->readInt();
  int nObjects = istream->readInt();
  int nFrags   = istream->readInt();

  String  bspName;
  Struct* str;
  Object* obj;
  String  typeName;
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
    typeName = istream->readString();

    if( typeName.isEmpty() ) {
      objects.add( null );
    }
    else {
      obj = library.objClass( typeName )->create( istream );
      objects.add( obj );

      // no need to register objects since Lua state is being deserialised

      bool isCut = istream->readBool();
      if( !isCut ) {
        position( obj );
      }
    }
  }
  for( int i = 0; i < nFrags; ++i ) {
    bool exists = istream->readBool();

    if( !exists ) {
      frags.add( null );
    }
    else {
      frag = new Frag();
      frag->readFull( istream );
      frag->index = i;
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

  log.unindent();
  log.println( "}" );
}

void Orbis::write( BufferStream* ostream ) const
{
  log.print( "Writing Orbis ..." );

  lua.write( ostream );

  terra.write( ostream );
  caelum.write( ostream );

  ostream->writeInt( structs.length() );
  ostream->writeInt( objects.length() );
  ostream->writeInt( frags.length() );

  String  typeName;
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
      ostream->writeBool( false );
    }
    else {
      ostream->writeBool( true );
      frag->writeFull( ostream );
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

  log.printEnd( " OK" );
}

void Orbis::load()
{
  log.println( "Loading Orbis {" );
  log.indent();

  structs.alloc( 128 );
  objects.alloc( 4096 );
  frags.alloc( 2048 );

  strFreedIndices[0].alloc( 4 );
  strFreedIndices[1].alloc( 4 );
  objFreedIndices[0].alloc( 64 );
  objFreedIndices[1].alloc( 64 );
  fragFreedIndices[0].alloc( 128 );
  fragFreedIndices[1].alloc( 128 );

  strAvailableIndices.alloc( 16 );
  objAvailableIndices.alloc( 256 );
  fragAvailableIndices.alloc( 512 );

  log.unindent();
  log.println( "}" );
}

void Orbis::unload()
{
  log.println( "Unloading Orbis {" );
  log.indent();

  for( int i = 0; i < objects.length(); ++i ) {
    if( objects[i] != null && ( objects[i]->flags & Object::LUA_BIT ) ) {
      lua.unregisterObject( i );
    }
  }

  for( int i = 0; i < Orbis::MAX; ++i ) {
    for( int j = 0; j < Orbis::MAX; ++j ) {
      cells[i][j].structs.clear();
      cells[i][j].objects.clear();
      cells[i][j].frags.clear();
    }
  }

  structs.free();
  structs.dealloc();
  objects.free();
  objects.dealloc();
  frags.free();
  frags.dealloc();

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

  Frag::pool.free();

  log.unindent();
  log.println( "}" );
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
