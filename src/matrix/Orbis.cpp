/*
 *  Orbis.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Orbis.hpp"

#include "matrix/Lua.hpp"

namespace oz
{

  Orbis orbis;

  const float Cell::SIZE     = float( Cell::SIZEI );
  const float Cell::INV_SIZE = 1.0f / float( Cell::SIZEI );
  const float Cell::RADIUS   = Cell::SIZE * Math::SQRT2 / 2.0f;

  const float Orbis::DIM     = Cell::SIZE * Orbis::MAX / 2.0f;

  Orbis::Orbis() : bsps( 32 ), structs( 128 ), objects( 1024 ), parts( 1024 )
  {}

  void Orbis::init()
  {
    log.print( "Initialising Orbis ..." );

    freeing = 0;
    waiting = 1;

    mins = Vec3( -Orbis::DIM, -Orbis::DIM, -Orbis::DIM );
    maxs = Vec3(  Orbis::DIM,  Orbis::DIM,  Orbis::DIM );

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
        int iBsp = translator.bspIndex( bspName );

        if( bsps[iBsp] == null ) {
          bsps[iBsp] = new BSP( translator.bsps[iBsp].name );
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
        ostream->writeString( translator.bsps[str->iBsp].name );
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
