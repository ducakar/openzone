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

#include "Lua.h"

namespace oz
{

  World world;

  const float Cell::SIZE = static_cast<float>( Cell::SIZEI );
  const float Cell::RADIUS = Cell::SIZE * Math::SQRT2 / 2.0f;

  const float World::DIM = Cell::SIZE * World::MAX / 2.0f;

  void World::init()
  {
    log.print( "Initializing World ..." );

    freeing = 0;
    waiting = 1;

    mins = Vec3( -World::DIM, -World::DIM, -World::DIM );
    maxs = Vec3(  World::DIM,  World::DIM,  World::DIM );

    for( int i = 0; i < translator.bsps.length(); i++ ) {
      bsps << null;
    }

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
    log.print( "Loading World ..." );
    log.printEnd( " OK" );
  }

  void World::unload()
  {
    log.println( "Unloading World {" );
    log.indent();

    for( int i = 0; i < World::MAX; i++ ) {
      for( int j = 0; j < World::MAX; j++ ) {
        cells[i][j].structures.clear();
        cells[i][j].objects.clear();
        cells[i][j].particles.clear();
      }
    }

    foreach( part, particles.iterator() ) {
      if( *part != null ) {
        delete *part;
      }
    }
    particles.clear();
    particles.trim( 0 );

    foreach( obj, objects.iterator() ) {
      if( *obj != null ) {
        delete *obj;
      }
    }
    objects.clear();
    objects.trim( 0 );

    foreach( str, structures.iterator() ) {
      if( *str != null ) {
        delete *str;
      }
    }
    structures.clear();
    structures.trim( 0 );

    PoolAlloc<Object::Event, 0>::pool.free();
    PoolAlloc<Particle, 0>::pool.free();

    foreach( bsp, bsps.iterator() ) {
      if( *bsp != null ) {
        delete *bsp;
        *bsp = null;
      }
    }

    log.unindent();
    log.println( "}" );
  }

  void World::update()
  {
    strAvailableIndices.addAll( strFreedIndices[waiting] );
    strFreedIndices[waiting].clear();

    objAvailableIndices.addAll( objFreedIndices[waiting] );
    objFreedIndices[waiting].clear();

    partAvailableIndices.addAll( partFreedIndices[waiting] );
    partFreedIndices[waiting].clear();

    freeing = !freeing;
    waiting = !waiting;

    sky.update();
  }

  bool World::read( InputStream *istream )
  {
    assert( structures.length() == 0 && objects.length() == 0 && particles.length() == 0 );

    log.println( "Reading World from stream {" );
    log.indent();

    try {
      int n;

      n = istream->readInt();
      for( int i = 0; i < n; i++ ) {
        strFreedIndices[freeing] << istream->readInt();
      }
      n = istream->readInt();
      for( int i = 0; i < n; i++ ) {
        objFreedIndices[freeing] << istream->readInt();
      }
      n = istream->readInt();
      for( int i = 0; i < n; i++ ) {
        partFreedIndices[freeing] << istream->readInt();
      }

      n = istream->readInt();
      for( int i = 0; i < n; i++ ) {
        strFreedIndices[waiting] << istream->readInt();
      }
      n = istream->readInt();
      for( int i = 0; i < n; i++ ) {
        objFreedIndices[waiting] << istream->readInt();
      }
      n = istream->readInt();
      for( int i = 0; i < n; i++ ) {
        partFreedIndices[waiting] << istream->readInt();
      }

      n = istream->readInt();
      for( int i = 0; i < n; i++ ) {
        strAvailableIndices << istream->readInt();
      }
      n = istream->readInt();
      for( int i = 0; i < n; i++ ) {
        objAvailableIndices << istream->readInt();
      }
      n = istream->readInt();
      for( int i = 0; i < n; i++ ) {
        partAvailableIndices << istream->readInt();
      }

      sky.read( istream );

      int nStructures = istream->readInt();
      int nObjects    = istream->readInt();
      int nParticles  = istream->readInt();

      String    bspName;
      Structure *str;
      Object    *obj;
      String    typeName;
      Particle  *part;

      for( int i = 0; i < nStructures; i++ ) {
        istream->readString( bspName );

        if( bspName.isEmpty() ) {
          structures << null;
        }
        else {
          str = translator.createStruct( i, bspName, istream );
          structures << str;

          if( bsps[str->bsp] == null ) {
            bsps[str->bsp] = new BSP();
            if( !bsps[str->bsp]->load( translator.bsps[str->bsp].name ) ) {
              throw Exception( "Matrix BSP loading failed" );
            }
          }

          position( str );
        }
      }
      for( int i = 0; i < nObjects; i++ ) {
        istream->readString( typeName );

        if( typeName.isEmpty() ) {
          objects << null;
        }
        else {
          obj = translator.createObject( i, typeName, istream );
          objects << obj;

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
      log.unindent();
      log.println( "} %s", e.message );
      return false;
    }

    log.unindent();
    log.println( "}" );
    return true;
  }

  bool World::write( OutputStream *ostream )
  {
    log.print( "Writing World to stream ..." );
    try {
      ostream->writeInt( strFreedIndices[freeing].length() );
      foreach( i, strFreedIndices[freeing].iterator() ) {
        ostream->writeInt( *i );
      }
      ostream->writeInt( objFreedIndices[freeing].length() );
      foreach( i, objFreedIndices[freeing].iterator() ) {
        ostream->writeInt( *i );
      }
      ostream->writeInt( partFreedIndices[freeing].length() );
      foreach( i, partFreedIndices[freeing].iterator() ) {
        ostream->writeInt( *i );
      }

      ostream->writeInt( strFreedIndices[waiting].length() );
      foreach( i, strFreedIndices[waiting].iterator() ) {
        ostream->writeInt( *i );
      }
      ostream->writeInt( objFreedIndices[waiting].length() );
      foreach( i, objFreedIndices[waiting].iterator() ) {
        ostream->writeInt( *i );
      }
      ostream->writeInt( partFreedIndices[waiting].length() );
      foreach( i, partFreedIndices[waiting].iterator() ) {
        ostream->writeInt( *i );
      }

      ostream->writeInt( strAvailableIndices.length() );
      foreach( i, strAvailableIndices.iterator() ) {
        ostream->writeInt( *i );
      }
      ostream->writeInt( objAvailableIndices.length() );
      foreach( i, objAvailableIndices.iterator() ) {
        ostream->writeInt( *i );
      }
      ostream->writeInt( partAvailableIndices.length() );
      foreach( i, partAvailableIndices.iterator() ) {
        ostream->writeInt( *i );
      }

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
