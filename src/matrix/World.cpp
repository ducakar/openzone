/*
 *  World.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "stable.hpp"

#include "matrix/World.hpp"

#include "matrix/Lua.hpp"

namespace oz
{

  World world;

  const float Cell::SIZE     = float( Cell::SIZEI );
  const float Cell::INV_SIZE = 1.0f / float( Cell::SIZEI );
  const float Cell::RADIUS   = Cell::SIZE * Math::SQRT2 / 2.0f;

  const float World::DIM     = Cell::SIZE * World::MAX / 2.0f;

  World::World() : bsps( 32 ), structs( 128 ), objects( 1024 ), parts( 1024 )
  {}

  void World::init()
  {
    log.print( "Initializing World ..." );

    freeing = 0;
    waiting = 1;

    mins = Vec3( -World::DIM, -World::DIM, -World::DIM );
    maxs = Vec3(  World::DIM,  World::DIM,  World::DIM );

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

    for( int i = 0; i < translator.bsps.length(); ++i ) {
      bsps << null;
    }

    log.printEnd( " OK" );
  }

  void World::unload()
  {
    log.println( "Unloading World {" );
    log.indent();

    for( int i = 0; i < World::MAX; ++i ) {
      for( int j = 0; j < World::MAX; ++j ) {
        cells[i][j].structs.clear();
        cells[i][j].objects.clear();
        cells[i][j].parts.clear();
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

  void World::update()
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

  bool World::read( InputStream* istream )
  {
    assert( structs.length() == 0 && objects.length() == 0 && parts.length() == 0 );

    log.println( "Reading World from stream {" );
    log.indent();

    int n;

    n = istream->readInt();
    for( int i = 0; i < n; ++i ) {
      strFreedIndices[freeing] << istream->readInt();
    }
    n = istream->readInt();
    for( int i = 0; i < n; ++i ) {
      objFreedIndices[freeing] << istream->readInt();
    }
    n = istream->readInt();
    for( int i = 0; i < n; ++i ) {
      partFreedIndices[freeing] << istream->readInt();
    }

    n = istream->readInt();
    for( int i = 0; i < n; ++i ) {
      strFreedIndices[waiting] << istream->readInt();
    }
    n = istream->readInt();
    for( int i = 0; i < n; ++i ) {
      objFreedIndices[waiting] << istream->readInt();
    }
    n = istream->readInt();
    for( int i = 0; i < n; ++i ) {
      partFreedIndices[waiting] << istream->readInt();
    }

    n = istream->readInt();
    for( int i = 0; i < n; ++i ) {
      strAvailableIndices << istream->readInt();
    }
    n = istream->readInt();
    for( int i = 0; i < n; ++i ) {
      objAvailableIndices << istream->readInt();
    }
    n = istream->readInt();
    for( int i = 0; i < n; ++i ) {
      partAvailableIndices << istream->readInt();
    }

    sky.read( istream );

    int nStructures = istream->readInt();
    int nObjects    = istream->readInt();
    int nParticles  = istream->readInt();

    String    bspName;
    Structure* str;
    Object*    obj;
    String    typeName;
    Particle*  part;

    for( int i = 0; i < nStructures; ++i ) {
      istream->readString( bspName );

      if( bspName.isEmpty() ) {
        structs << null;
      }
      else {
        str = translator.createStruct( i, bspName, istream );
        structs << str;

        if( bsps[str->bsp] == null ) {
          bsps[str->bsp] = new BSP();
          if( !bsps[str->bsp]->load( translator.bsps[str->bsp].name ) ) {
            throw Exception( "Matrix BSP loading failed" );
          }
        }

        if( !position( str ) ) {
          throw Exception( "Matrix structure reading failed, too many structures per cell." );
        }
      }
    }
    for( int i = 0; i < nObjects; ++i ) {
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
    for( int i = 0; i < nParticles; ++i ) {
      bool exists = istream->readBool();

      if( !exists ) {
        parts << null;
      }
      else {
        part = new Particle();
        part->readFull( istream );
        part->index = i;
        parts << part;
        position( part );
      }
    }

    log.unindent();
    log.println( "}" );
    return true;
  }

  bool World::write( OutputStream* ostream )
  {
    log.print( "Writing World to stream ..." );

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
        ostream->writeString( obj->type->name );
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
