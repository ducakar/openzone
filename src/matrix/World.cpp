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

  void World::init()
  {
    log.print( "Initializing World ..." );

    mins = Vec3( -World::DIM, -World::DIM, -World::DIM );
    maxs = Vec3(  World::DIM,  World::DIM,  World::DIM );

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

          if( obj->flags & Object::CUT_BIT ) {
            obj->flags &= ~Object::CUT_BIT;
          }
          else {
            position( obj );
          }
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
          if( obj->cell == null ) {
            obj->flags |= obj->CUT_BIT;
          }
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
