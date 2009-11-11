/*
 *  World.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "World.h"

namespace oz
{

  World world;

  const float Cell::SIZE = static_cast<float>( Cell::SIZEI );
  const float Cell::RADIUS = Cell::SIZE * Math::SQRT2 / 2.0f;

  const float World::DIM = Cell::SIZE * World::MAX / 2.0f;

  void World::position( Structure *str )
  {
    const Bounds &bsp = *bsps[str->bsp];

    switch( str->rot ) {
      case Structure::R0: {
        str->mins = bsp.mins + str->p;
        str->maxs = bsp.maxs + str->p;
        break;
      }
      case Structure::R90: {
        str->mins = Vec3( -bsp.maxs.y + str->p.x, bsp.mins.x + str->p.y, bsp.mins.z + str->p.z );
        str->maxs = Vec3( -bsp.mins.y + str->p.x, bsp.maxs.x + str->p.y, bsp.maxs.z + str->p.z );
        break;
      }
      case Structure::R180: {
        str->mins = str->p + Vec3( -bsp.maxs.x, -bsp.maxs.y, +bsp.mins.z );
        str->maxs = str->p + Vec3( -bsp.mins.x, -bsp.mins.y, +bsp.maxs.z );
        break;
      }
      case Structure::R270: {
        str->mins = Vec3( bsp.mins.y + str->p.x, -bsp.maxs.x + str->p.y, bsp.mins.z + str->p.z );
        str->maxs = Vec3( bsp.maxs.y + str->p.x, -bsp.mins.x + str->p.y, bsp.maxs.z + str->p.z );
        break;
      }
      default: {
        assert( false );
        break;
      }
    }

    getInters( *str, EPSILON );

    for( int x = minX; x <= maxX; x++ ) {
      for( int y = minY; y <= maxY; y++ ) {
        cells[x][y].structures << str->index;
      }
    }
  }

  void World::unposition( Structure *str )
  {
    getInters( *str, EPSILON );

    for( int x = minX; x <= maxX; x++ ) {
      for( int y = minY; y <= maxY; y++ ) {
        cells[x][y].structures.exclude( str->index );
      }
    }
  }

  void World::position( Object *obj )
  {
    obj->cell = world.getCell( obj->p );
    obj->cell->objects << obj;
  }

  void World::unposition( Object *obj )
  {
    obj->cell->objects.remove( obj );
    obj->cell = null;
  }

  void World::reposition( Object *obj )
  {
    Cell *oldCell = obj->cell;
    Cell *newCell = world.getCell( obj->p );

    if( newCell != oldCell ) {
      oldCell->objects.remove( obj );
      newCell->objects << obj;
      obj->cell = newCell;
    }
  }

  void World::position( Particle *part )
  {
    part->cell = world.getCell( part->p );
    part->cell->particles << part;
  }

  void World::unposition( Particle *part )
  {
    part->cell->particles.remove( part );
    part->cell = null;
  }

  void World::reposition( Particle *part )
  {
    Cell *oldCell = part->cell;
    Cell *newCell = world.getCell( part->p );

    if( newCell != oldCell ) {
      oldCell->particles.remove( part );
      newCell->particles << part;
      part->cell = newCell;
    }
  }

  void World::put( Structure *str )
  {
    if( strAvailableIndices.isEmpty() ) {
      str->index = structures.length();
      structures << str;
    }
    else {
      strAvailableIndices >> str->index;
      structures[str->index] = str;
    }
  }

  void World::put( Object *obj )
  {
    if( objAvailableIndices.isEmpty() ) {
      obj->index = objects.length();
      objects << obj;
    }
    else {
      objAvailableIndices >> obj->index;
      objects[obj->index] = obj;
    }
  }

  void World::put( Particle *part )
  {
    if( partAvailableIndices.isEmpty() ) {
      part->index = particles.length();
      particles << part;
    }
    else {
      partAvailableIndices >> part->index;
      particles[part->index] = part;
    }
  }

  void World::cut( Structure *str )
  {
    strPendingIndices << str->index;
    structures[str->index] = null;
    str->index = -1;
  }

  void World::cut( Object *obj )
  {
    objPendingIndices << obj->index;
    objects[obj->index] = null;
    obj->index = -1;
  }

  void World::cut( Particle *part )
  {
    partPendingIndices << part->index;
    particles[part->index] = null;
    part->index = -1;
  }

  World::World() :
      Bounds( Vec3( -World::DIM, -World::DIM, -World::DIM ),
              Vec3(  World::DIM,  World::DIM,  World::DIM ) )
  {}

  void World::init()
  {
    log.print( "Initializing World ..." );

    sky.set( -180.0f, 1440.0f, 0.0f );
    terra.init();

    log.printEnd( " OK" );
  }

  void World::free()
  {
    log.print( "Freeing World ..." );
    log.printEnd( " OK" );
  }

  void World::load()
  {
    log.println( "Loading World {" );
    log.indent();

    foreach( bsp, translator.bsps.iterator() ) {
      bsps << new BSP();
      if( !bsps.last()->load( bsp->name ) ) {
        throw Exception( "BSP loading failed" );
      }
    }

    log.unindent();
    log.println( "}" );
  }

  void World::unload()
  {
    log.print( "Unloading World ..." );

    for( int i = 0; i < World::MAX; i++ ) {
      for( int j = 0; j < World::MAX; j++ ) {
        cells[i][j].structures.clear();
        cells[i][j].objects.clear();
        cells[i][j].particles.clear();
      }
    }

    foreach( bsp, bsps.iterator() ) {
      ( *bsp )->free();
      delete *bsp;
    }
    bsps.clear();

    foreach( str, structures.iterator() ) {
      if( *str != null ) {
        delete *str;
      }
    }
    structures.clear();

    foreach( obj, objects.iterator() ) {
      if( *obj != null ) {
        delete *obj;
      }
    }
    objects.clear();

    foreach( part, particles.iterator() ) {
      if( *part != null ) {
        delete *part;
      }
    }
    particles.clear();

    PoolAlloc<Object::Event, 0>::pool.free();
    PoolAlloc<Particle, 0>::pool.free();

    log.printEnd( " OK" );
  }

  void World::update()
  {
    strAvailableIndices.addAll( strPendingIndices );
    strPendingIndices.clear();

    objAvailableIndices.addAll( objPendingIndices );
    objPendingIndices.clear();

    partAvailableIndices.addAll( partPendingIndices );
    partPendingIndices.clear();

    sky.update();
  }

  bool World::read( InputStream *istream )
  {
    assert( structures.length() == 0 && objects.length() == 0 && particles.length() == 0 );

    log.print( "Reading World from stream ..." );
    try {
      sky.read( istream );

      int nStructures = istream->readInt();
      int nObjects    = istream->readInt();
      int nParticles  = istream->readInt();

      String    bspFile;
      Structure *str;
      Object    *obj;
      String    typeName;
      Particle  *part;

      for( int i = 0; i < nStructures; i++ ) {
        istream->readString( bspFile );

        if( bspFile.length() == 0 ) {
          structures << null;
        }
        else {
          int bspIndex = translator.bspIndex( bspFile );
          str = new Structure();
          str->readFull( istream );
          str->index = i;
          str->bsp = bspIndex;
          structures << str;
          position( str );
        }
      }
      for( int i = 0; i < nObjects; i++ ) {
        istream->readString( typeName );

        if( typeName.length() == 0 ) {
          objects << null;
        }
        else {
          obj = translator.createObject( typeName, istream );
          obj->index = i;
          objects << obj;
          position( obj );
        }
      }
      for( int i = 0; i < nParticles; i++ ) {
        bool exists = istream->readBool();

        if( !exists ) {
          particles << null;
        }
        else {
          part = new Particle();
          part->readFull( istream );
          part->index = i;
          particles << part;
          position( part );
        }
      }
    }
    catch( const Exception &e ) {
      log.printEnd( " %s", e.message );
      return false;
    }
    log.printEnd( " OK" );
    return true;
  }

  bool World::write( OutputStream *ostream )
  {
    log.print( "Writing World to stream ..." );
    try {
      sky.write( ostream );

      ostream->writeInt( structures.length() );
      ostream->writeInt( objects.length() );
      ostream->writeInt( particles.length() );

      String    typeName;
      Structure *str;
      Object    *obj;
      Particle  *part;

      for( int i = 0; i < structures.length(); i++ ) {
        str = structures[i];

        if( str == null ) {
          ostream->writeString( "" );
        }
        else {
          ostream->writeString( translator.bsps[str->bsp].name );
          str->writeFull( ostream );
        }
      }
      for( int i = 0; i < objects.length(); i++ ) {
        obj = objects[i];

        if( obj == null ) {
          ostream->writeString( "" );
        }
        else {
          ostream->writeString( obj->type->name );
          obj->writeFull( ostream );
        }
      }
      for( int i = 0; i < particles.length(); i++ ) {
        part = particles[i];

        if( part == null ) {
          ostream->writeBool( false );
        }
        else {
          ostream->writeBool( true );
          part->writeFull( ostream );
        }
      }
    }
    catch( const Exception &e ) {
      log.printEnd( " %s", e.message );
      return false;
    }
    log.printEnd( " OK" );
    return true;
  }

}
