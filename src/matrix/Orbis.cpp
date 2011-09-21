/*
 *  Orbis.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Orbis.hpp"

#include "matrix/Translator.hpp"
#include "matrix/Lua.hpp"
#include "matrix/Dynamic.hpp"
#include "matrix/Bot.hpp"
#include "matrix/Weapon.hpp"
#include "matrix/Vehicle.hpp"

namespace oz
{

  Orbis orbis;

  const float Cell::SIZE     = float( SIZEI );
  const float Cell::INV_SIZE = 1.0f / float( SIZEI );
  const float Cell::RADIUS   = SIZE * Math::sqrt( 2 ) / 2.0f;

#ifndef OZ_TOOLS

  const float Orbis::DIM     = Cell::SIZE * MAX / 2.0f;

  bool Orbis::position( Struct* str )
  {
    str->setRotation( *bsps[str->bsp], str->rot );

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

  void Orbis::position( Particle* part )
  {
    hard_assert( part->cell == null );

    Cell* cell = getCell( part->p );

    part->cell = cell;
    part->prev[0] = null;

    if( !cell->particles.isEmpty() ) {
      cell->particles.first()->prev[0] = part;
    }

    cell->particles.add( part );
  }

  void Orbis::unposition( Particle* part )
  {
    hard_assert( part->cell != null );

    Cell* cell = part->cell;

    part->cell = null;

    if( part->next[0] != null ) {
      part->next[0]->prev[0] = part->prev[0];
    }

    cell->particles.remove( part, part->prev[0] );
  }

  void Orbis::reposition( Particle* part )
  {
    hard_assert( part->cell != null );

    Cell* oldCell = part->cell;
    Cell* newCell = getCell( part->p );

    if( newCell != oldCell ) {
      if( part->next[0] != null ) {
        part->next[0]->prev[0] = part->prev[0];
      }

      oldCell->particles.remove( part, part->prev[0] );

      part->cell = newCell;
      part->prev[0] = null;

      if( !newCell->particles.isEmpty() ) {
        newCell->particles.first()->prev[0] = part;
      }

      newCell->particles.add( part );
    }
  }

  int Orbis::addStruct( const char* name, const Point3& p, Struct::Rotation rot )
  {
    int index;

    if( strAvailableIndices.isEmpty() ) {
      index = structs.length();
      structs.add( translator.createStruct( index, name, p, rot ) );
    }
    else {
      index = strAvailableIndices.popLast();
      structs[index] = translator.createStruct( index, name, p, rot );
    }
    return index;
  }

  // has to be reentrant, can be called again from translator.createObject
  int Orbis::addObject( const char* name, const Point3& p )
  {
    int index;

    if( objAvailableIndices.isEmpty() ) {
      index = objects.length();
      // reserve slot so reentrant calls cannot occupy it again
      objects.add( null );
    }
    else {
      index = objAvailableIndices.popLast();
    }
    // objects vector may relocate during createObject call, we must use this workaround
    Object* obj = translator.createObject( index, name, p );
    objects[index] = obj;

    if( objects[index]->flags & Object::LUA_BIT ) {
      lua.registerObject( index );
    }
    return index;
  }

  int Orbis::addPart( const Point3& p, const Vec3& velocity, const Vec3& colour,
                      float restitution, float mass, float lifeTime )
  {
    int index;

    if( partAvailableIndices.isEmpty() ) {
      index = parts.length();
      parts.add( new Particle( index, p, velocity, colour, restitution, mass, lifeTime ) );
    }
    else {
      index = partAvailableIndices.popLast();
      parts[index] = new Particle( index, p, velocity, colour, restitution, mass, lifeTime );
    }
    return index;
  }

  void Orbis::remove( Struct* str )
  {
    hard_assert( str->index >= 0 );

    strFreedIndices[freeing].add( str->index );
    structs[str->index] = null;
    str->index = -1;
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
    obj->index = -1;
  }

  void Orbis::remove( Particle* part )
  {
    hard_assert( part->index >= 0 );
    hard_assert( part->cell == null );

    partFreedIndices[freeing].add( part->index );
    parts[part->index] = null;
    part->index = -1;
  }

  void Orbis::update()
  {
    strAvailableIndices.addAll( strFreedIndices[waiting], strFreedIndices[waiting].length() );
    strFreedIndices[waiting].clear();

    objAvailableIndices.addAll( objFreedIndices[waiting], objFreedIndices[waiting].length() );
    objFreedIndices[waiting].clear();

    partAvailableIndices.addAll( partFreedIndices[waiting], objFreedIndices[waiting].length() );
    partFreedIndices[waiting].clear();

    freeing = !freeing;
    waiting = !waiting;

    caelum.update();
  }

  void Orbis::load()
  {
    log.println( "Loading Orbis {" );
    log.indent();

    bsps.alloc( 32 );
    structs.alloc( 128 );
    objects.alloc( 4096 );
    parts.alloc( 2048 );

    strFreedIndices[0].alloc( 4 );
    strFreedIndices[1].alloc( 4 );
    objFreedIndices[0].alloc( 64 );
    objFreedIndices[1].alloc( 64 );
    partFreedIndices[0].alloc( 128 );
    partFreedIndices[1].alloc( 128 );

    strAvailableIndices.alloc( 16 );
    objAvailableIndices.alloc( 256 );
    partAvailableIndices.alloc( 512 );

    for( int i = 0; i < translator.bsps.length(); ++i ) {
      bsps.add( new BSP( i ) );
    }

    log.unindent();
    log.println( "}" );
  }

  void Orbis::unload()
  {
    log.println( "Unloading Orbis {" );
    log.indent();

    for( int i = 0; i < Orbis::MAX; ++i ) {
      for( int j = 0; j < Orbis::MAX; ++j ) {
        cells[i][j].structs.clear();
        cells[i][j].objects.clear();
        cells[i][j].particles.clear();
      }
    }

    Struct::overlappingObjs.clear();
    Struct::overlappingObjs.dealloc();

    bsps.free();
    bsps.dealloc();
    structs.free();
    structs.dealloc();
    objects.free();
    objects.dealloc();
    parts.free();
    parts.dealloc();

    strFreedIndices[0].clear();
    strFreedIndices[0].dealloc();
    strFreedIndices[1].clear();
    strFreedIndices[1].dealloc();
    objFreedIndices[0].clear();
    objFreedIndices[0].dealloc();
    objFreedIndices[1].clear();
    objFreedIndices[1].dealloc();
    partFreedIndices[0].clear();
    partFreedIndices[0].dealloc();
    partFreedIndices[1].clear();
    partFreedIndices[1].dealloc();

    strAvailableIndices.clear();
    strAvailableIndices.dealloc();
    objAvailableIndices.clear();
    objAvailableIndices.dealloc();
    partAvailableIndices.clear();
    partAvailableIndices.dealloc();

    Struct::pool.free();

    Object::Event::pool.free();
    Object::pool.free();
    Dynamic::pool.free();
    Weapon::pool.free();
    Bot::pool.free();
    Vehicle::pool.free();

    Particle::pool.free();

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

  bool Orbis::read( InputStream* istream )
  {
    hard_assert( structs.length() == 0 && objects.length() == 0 && parts.length() == 0 );

    log.print( "Reading Orbis from stream ..." );

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
      partFreedIndices[freeing].add( istream->readInt() );
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
      partFreedIndices[waiting].add( istream->readInt() );
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
      partAvailableIndices.add( istream->readInt() );
    }

    caelum.read( istream );

    int nStructs   = istream->readInt();
    int nObjects   = istream->readInt();
    int nParticles = istream->readInt();

    String    bspName;
    Struct*   str;
    Object*   obj;
    String    typeName;
    Particle* part;

    for( int i = 0; i < nStructs; ++i ) {
      bspName = istream->readString();

      if( bspName.isEmpty() ) {
        structs.add( null );
      }
      else {
        str = translator.createStruct( i, bspName, istream );
        structs.add( str );

        if( !position( str ) ) {
          throw Exception( "Matrix structure reading failed, too many structures per cell." );
        }
      }
    }
    for( int i = 0; i < nObjects; ++i ) {
      typeName = istream->readString();

      if( typeName.isEmpty() ) {
        objects.add( null );
      }
      else {
        obj = translator.createObject( i, typeName, istream );
        objects.add( obj );

        if( obj->flags & Object::LUA_BIT ) {
          lua.registerObject( i );
        }
        if( obj->flags & Object::CUT_BIT ) {
          obj->flags &= ~Object::CUT_BIT;
        }
        else {
          position( obj );
        }
      }
    }
    for( int i = 0; i < nParticles; ++i ) {
      bool exists = istream->readBool();

      if( !exists ) {
        parts.add( null );
      }
      else {
        part = new Particle();
        part->readFull( istream );
        part->index = i;
        parts.add( part );
        position( part );
      }
    }

    log.printEnd( " OK" );
    return true;
  }

  bool Orbis::write( OutputStream* ostream )
  {
    log.print( "Writing Orbis to stream ..." );

    ostream->writeInt( strFreedIndices[freeing].length() );
    foreach( i, strFreedIndices[freeing].citer() ) {
      ostream->writeInt( *i );
    }
    ostream->writeInt( objFreedIndices[freeing].length() );
    foreach( i, objFreedIndices[freeing].citer() ) {
      ostream->writeInt( *i );
    }
    ostream->writeInt( partFreedIndices[freeing].length() );
    foreach( i, partFreedIndices[freeing].citer() ) {
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
    ostream->writeInt( partFreedIndices[waiting].length() );
    foreach( i, partFreedIndices[waiting].citer() ) {
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
    ostream->writeInt( partAvailableIndices.length() );
    foreach( i, partAvailableIndices.citer() ) {
      ostream->writeInt( *i );
    }

    caelum.write( ostream );

    ostream->writeInt( structs.length() );
    ostream->writeInt( objects.length() );
    ostream->writeInt( parts.length() );

    String    typeName;
    Struct*   str;
    Object*   obj;
    Particle* part;

    for( int i = 0; i < structs.length(); ++i ) {
      str = structs[i];

      if( str == null ) {
        ostream->writeString( "" );
      }
      else {
        ostream->writeString( translator.bsps[str->bsp].name );
        str->writeFull( ostream );
      }
    }
    for( int i = 0; i < objects.length(); ++i ) {
      obj = objects[i];

      if( obj == null ) {
        ostream->writeString( "" );
      }
      else {
        if( obj->cell == null ) {
          obj->flags |= obj->CUT_BIT;
        }
        ostream->writeString( obj->clazz->name );
        obj->writeFull( ostream );
      }
    }
    for( int i = 0; i < parts.length(); ++i ) {
      part = parts[i];

      if( part == null ) {
        ostream->writeBool( false );
      }
      else {
        ostream->writeBool( true );
        part->writeFull( ostream );
      }
    }

    log.printEnd( " OK" );
    return true;
  }

#endif // OZ_TOOLS

}
