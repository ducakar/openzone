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

namespace oz
{

  Orbis orbis;

  const float Cell::SIZE     = float( Cell::SIZEI );
  const float Cell::INV_SIZE = 1.0f / float( Cell::SIZEI );
  const float Cell::RADIUS   = Cell::SIZE * Math::SQRT2 / 2.0f;

  const float Orbis::DIM     = Cell::SIZE * Orbis::MAX / 2.0f;

  void Orbis::requestBSP( int bsp ) {
    if( bsps[bsp] == null ) {
      bsps[bsp] = new BSP( translator.bsps[bsp].name );
    }
  }

  bool Orbis::position( Structure* str )
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
        assert( !cells[x][y].structs.contains( short( str->index ) ) );

        cells[x][y].structs.add( short( str->index ) );
      }
    }
    return true;
  }

  void Orbis::unposition( Structure* str )
  {
    Span span = getInters( *str, EPSILON );

    for( int x = span.minX; x <= span.maxX; ++x ) {
      for( int y = span.minY; y <= span.maxY; ++y ) {
        assert( cells[x][y].structs.contains( short( str->index ) ) );

        cells[x][y].structs.excludeUO( short( str->index ) );
      }
    }
  }

  void Orbis::position( Object* obj )
  {
    assert( obj->cell == null );

    Cell* cell = getCell( obj->p );

    obj->cell = cell;
    obj->next[0] = cell->firstObject;
    obj->prev[0] = null;

    if( cell->firstObject == null ) {
      cell->firstObject = obj;
    }
    else {
      cell->firstObject->prev[0] = obj;
      cell->firstObject = obj;
    }
  }

  void Orbis::unposition( Object* obj )
  {
    assert( obj->cell != null );

    Cell* cell = obj->cell;

    obj->cell = null;

    if( obj->prev[0] == null ) {
      cell->firstObject = obj->next[0];
    }
    else {
      obj->prev[0]->next[0] = obj->next[0];
    }
    if( obj->next[0] != null ) {
      obj->next[0]->prev[0] = obj->prev[0];
    }
  }

  void Orbis::reposition( Object* obj )
  {
    assert( obj->cell != null );

    Cell* oldCell = obj->cell;
    Cell* newCell = getCell( obj->p );

    if( newCell != oldCell ) {
      if( obj->prev[0] == null ) {
        oldCell->firstObject = obj->next[0];
      }
      else {
        obj->prev[0]->next[0] = obj->next[0];
      }
      if( obj->next[0] != null ) {
        obj->next[0]->prev[0] = obj->prev[0];
      }

      obj->cell = newCell;
      obj->next[0] = newCell->firstObject;
      obj->prev[0] = null;

      if( newCell->firstObject == null ) {
        newCell->firstObject = obj;
      }
      else {
        newCell->firstObject->prev[0] = obj;
        newCell->firstObject = obj;
      }
    }
  }

  void Orbis::position( Particle* part )
  {
    assert( part->cell == null );

    Cell* cell = getCell( part->p );

    part->cell = cell;
    part->next[0] = cell->firstPart;
    part->prev[0] = null;

    if( cell->firstPart == null ) {
      cell->firstPart = part;
    }
    else {
      cell->firstPart->prev[0] = part;
      cell->firstPart = part;
    }
  }

  void Orbis::unposition( Particle* part )
  {
    assert( part->cell != null );

    Cell* cell = part->cell;

    part->cell = null;

    if( part->prev[0] == null ) {
      cell->firstPart = part->next[0];
    }
    else {
      part->prev[0]->next[0] = part->next[0];
    }
    if( part->next[0] != null ) {
      part->next[0]->prev[0] = part->prev[0];
    }
  }

  void Orbis::reposition( Particle* part )
  {
    assert( part->cell != null );

    Cell* oldCell = part->cell;
    Cell* newCell = getCell( part->p );

    if( newCell != oldCell ) {
      if( part->prev[0] == null ) {
        oldCell->firstPart = part->next[0];
      }
      else {
        part->prev[0]->next[0] = part->next[0];
      }
      if( part->next[0] != null ) {
        part->next[0]->prev[0] = part->prev[0];
      }

      part->cell = newCell;
      part->next[0] = newCell->firstPart;
      part->prev[0] = null;

      if( newCell->firstPart == null ) {
        newCell->firstPart = part;
      }
      else {
        newCell->firstPart->prev[0] = part;
        newCell->firstPart = part;
      }
    }
  }

  int Orbis::addStruct( const char* name, const Point3& p, Structure::Rotation rot )
  {
    int index;

    if( strAvailableIndices.isEmpty() ) {
      index = structs.length();
      structs.pushLast( translator.createStruct( index, name, p, rot ) );
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
      objects.pushLast( null );
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
      parts.pushLast( new Particle( index, p, velocity, colour, restitution, mass, lifeTime ) );
    }
    else {
      index = partAvailableIndices.popLast();
      parts[index] = new Particle( index, p, velocity, colour, restitution, mass, lifeTime );
    }
    return index;
  }

  void Orbis::remove( Structure* str )
  {
    assert( str->index >= 0 );

    strFreedIndices[freeing].pushLast( str->index );
    structs[str->index] = null;
    str->index = -1;
  }

  void Orbis::remove( Object* obj )
  {
    assert( obj->index >= 0 );
    assert( obj->cell == null );

    if( obj->flags & Object::LUA_BIT ) {
      lua.unregisterObject( obj->index );
    }
    objFreedIndices[freeing].pushLast( obj->index );
    objects[obj->index] = null;
    obj->index = -1;
  }

  void Orbis::remove( Particle* part )
  {
    assert( part->index >= 0 );
    assert( part->cell == null );

    partFreedIndices[freeing].pushLast( part->index );
    parts[part->index] = null;
    part->index = -1;
  }

  Orbis::Orbis() : bsps( 32 ), structs( 128 ), objects( 1024 ), parts( 1024 )
  {}

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
  {
    log.print( "Freeing Orbis ..." );
    log.printEnd( " OK" );
  }

  void Orbis::load()
  {
    log.print( "Loading Orbis ..." );

    for( int i = 0; i < translator.bsps.length(); ++i ) {
      bsps.add( null );
    }

    log.printEnd( " OK" );
  }

  void Orbis::unload()
  {
    log.println( "Unloading Orbis {" );
    log.indent();

    for( int i = 0; i < Orbis::MAX; ++i ) {
      for( int j = 0; j < Orbis::MAX; ++j ) {
        cells[i][j].structs.clear();
        cells[i][j].firstObject = null;
        cells[i][j].firstPart = null;
      }
    }

    parts.free();
    parts.trim();

    objects.free();
    objects.trim();

    structs.free();
    structs.trim();

    bsps.free();

    log.unindent();
    log.println( "}" );
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

    sky.update();
  }

  bool Orbis::read( InputStream* istream )
  {
    assert( structs.length() == 0 && objects.length() == 0 && parts.length() == 0 );

    log.println( "Reading Orbis from stream {" );
    log.indent();

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

    sky.read( istream );

    int nStructures = istream->readInt();
    int nObjects    = istream->readInt();
    int nParticles  = istream->readInt();

    String     bspName;
    Structure* str;
    Object*    obj;
    String     typeName;
    Particle*  part;

    for( int i = 0; i < nStructures; ++i ) {
      bspName = istream->readString();

      if( bspName.isEmpty() ) {
        structs.add( null );
      }
      else {
        int bsp = translator.bspIndex( bspName );

        if( bsps[bsp] == null ) {
          bsps[bsp] = new BSP( translator.bsps[bsp].name );
        }

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

    log.unindent();
    log.println( "}" );
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

    sky.write( ostream );

    ostream->writeInt( structs.length() );
    ostream->writeInt( objects.length() );
    ostream->writeInt( parts.length() );

    String    typeName;
    Structure* str;
    Object*    obj;
    Particle*  part;

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

}
