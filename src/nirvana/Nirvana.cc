/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2011 Davorin Učakar
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
 * @file nirvana/Nirvana.cc
 */

#include "stable.hh"

#include "nirvana/Nirvana.hh"

#include "matrix/Synapse.hh"
#include "matrix/Matrix.hh"
#include "matrix/Bot.hh"

#include "nirvana/Lua.hh"
#include "nirvana/Mind.hh"
#include "nirvana/Memo.hh"

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
      throw Exception( "Invalid device type '%s'", type.cstr() );
    }

    devices.add( index, ( *func )( index, istream ) );
  }
  for( int i = 0; i < nMinds; ++i ) {
    int index = istream->readInt();

    minds.add( index, new Mind( index, istream ) );
  }

  log.printEnd( " OK" );
}

void Nirvana::write( BufferStream* ostream ) const
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
