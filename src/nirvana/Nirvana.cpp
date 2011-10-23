/*
 *  Nirvana.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "nirvana/Nirvana.hpp"

#include "matrix/Synapse.hpp"
#include "matrix/Matrix.hpp"
#include "matrix/BotClass.hpp"

#include "nirvana/Lua.hpp"
#include "nirvana/Mind.hpp"
#include "nirvana/Memo.hpp"

#define OZ_REGISTER_DEVICE( name ) \
  deviceClasses.add( #name, &name::create )

namespace oz
{
namespace nirvana
{

  Nirvana nirvana;

  void Nirvana::sync()
  {
    // remove devices and minds of removed objects
    foreach( i, synapse.removedObjects.citer() ) {
      const Device* const* device = devices.find( *i );
      const Mind* const* mind = minds.find( *i );

      if( device != null ) {
        delete *device;
        devices.exclude( *i );
      }
      if( mind != null ) {
        delete *mind;
        minds.exclude( *i );
      }
    }
    // add minds for new bots
    foreach( i, synapse.addedObjects.citer() ) {
      const Object* obj = orbis.objects[*i];

      if( obj != null && ( obj->flags & Object::BOT_BIT ) ) {
        minds.add( obj->index, new Mind( obj->index ) );
      }
    }
  }

  void Nirvana::update()
  {
    int count = 0;
    foreach( i, minds.iter() ) {
      Mind* mind = i.value();

      const Bot* bot = static_cast<const Bot*>( orbis.objects[mind->bot] );
      hard_assert( bot != null && ( bot->flags & Object::BOT_BIT ) );

      if( !( bot->state & Bot::PLAYER_BIT ) &&
          ( ( mind->flags & Mind::FORCE_UPDATE_BIT ) || count % UPDATE_INTERVAL == updateModulo ) )
      {
        mind->update();
      }
      ++count;
    }
    updateModulo = ( updateModulo + 1 ) % UPDATE_INTERVAL;
  }

  void Nirvana::read( InputStream* istream )
  {
    log.print( "Reading Nirvana ..." );

    lua.read( istream );

    String typeName;

    int nDevices = istream->readInt();
    int nMinds   = istream->readInt();

    for( int i = 0; i < nDevices; ++i ) {
      int index   = istream->readInt();
      String type = istream->readString();

      const Device::CreateFunc* func = deviceClasses.find( type );

      if( func == null ) {
        throw Exception( "Invalid device type '" + type + "'" );
      }

      devices.add( index, ( *func )( index, istream ) );
    }
    for( int i = 0; i < nMinds; ++i ) {
      int index = istream->readInt();

      minds.add( index, new Mind( index, istream ) );
    }

    log.printEnd( " OK" );
  }

  void Nirvana::write( OutputStream* ostream ) const
  {
    log.print( "Writing Nirvana ..." );

    lua.write( ostream );

    ostream->writeInt( devices.length() );
    ostream->writeInt( minds.length() );

    foreach( device, devices.citer() ) {
      ostream->writeInt( device.key() );
      ostream->writeString( device.value()->type() );

      device.value()->write( ostream );
    }
    foreach( mind, minds.citer() ) {
      ostream->writeInt( mind.value()->bot );
      mind.value()->write( ostream );
    }

    log.printEnd( " OK" );
  }

  void Nirvana::load()
  {
    log.print( "Loading Nirvana ..." );
    log.printEnd( " OK" );
  }

  void Nirvana::unload()
  {
    log.print( "Unloading Nirvana ..." );

    devices.free();
    devices.dealloc();

    minds.free();
    minds.dealloc();

    Memo::pool.free();
    Mind::pool.free();

    log.printEnd( " OK" );
  }

  void Nirvana::init()
  {
    log.println( "Initialising Nirvana {" );
    log.indent();

    OZ_REGISTER_DEVICE( Memo );

    lua.init();

    updateModulo = 0;

    log.unindent();
    log.println( "}" );
  }

  void Nirvana::free()
  {
    log.println( "Freeing Nirvana {" );
    log.indent();

    lua.free();

    deviceClasses.clear();
    deviceClasses.dealloc();

    log.unindent();
    log.println( "}" );
  }

}
}
