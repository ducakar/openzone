/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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

#include <stable.hh>
#include <nirvana/Nirvana.hh>

#include <matrix/Synapse.hh>
#include <matrix/Matrix.hh>
#include <matrix/Bot.hh>
#include <nirvana/Lua.hh>
#include <nirvana/Memo.hh>

#define OZ_REGISTER_DEVICE( name ) \
  deviceClasses.add( #name, &name::create )

namespace oz
{
namespace nirvana
{

void Nirvana::sync()
{
  // remove devices and minds of removed objects
  foreach( i, synapse.removedObjects.citer() ) {
    const Device* const* device = devices.find( *i );
    const Mind* const* mind = minds.find( *i );

    if( device != nullptr ) {
      delete *device;
      devices.exclude( *i );
    }
    if( mind != nullptr ) {
      delete *mind;
      minds.exclude( *i );
    }
  }
  // add minds for new bots
  foreach( i, synapse.addedObjects.citer() ) {
    const Object* obj = orbis.objects[*i];

    if( obj != nullptr && ( obj->flags & Object::BOT_BIT ) ) {
      minds.add( obj->index, new Mind( obj->index ) );
    }
  }
}

void Nirvana::update()
{
  int count = 0;
  foreach( i, minds.iter() ) {
    Mind* mind = i->value;

    const Bot* bot = static_cast<const Bot*>( orbis.objects[mind->bot] );
    hard_assert( bot != nullptr && ( bot->flags & Object::BOT_BIT ) );

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
  Log::print( "Reading Nirvana ..." );

  lua.read( istream );

  String typeName;

  int nDevices = istream->readInt();
  int nMinds   = istream->readInt();

  for( int i = 0; i < nDevices; ++i ) {
    int index   = istream->readInt();
    String type = istream->readString();

    Device::CreateFunc* const* func = deviceClasses.find( type );

    if( func == nullptr ) {
      OZ_ERROR( "Invalid device type '%s'", type.cstr() );
    }

    devices.add( index, ( *func )( index, istream ) );
  }
  for( int i = 0; i < nMinds; ++i ) {
    int index = istream->readInt();

    minds.add( index, new Mind( index, istream ) );
  }

  Log::printEnd( " OK" );
}

void Nirvana::write( BufferStream* ostream ) const
{
  Log::print( "Writing Nirvana ..." );

  lua.write( ostream );

  ostream->writeInt( devices.length() );
  ostream->writeInt( minds.length() );

  foreach( device, devices.citer() ) {
    ostream->writeInt( device->key );
    ostream->writeString( device->value->type() );

    device->value->write( ostream );
  }
  foreach( mind, minds.citer() ) {
    ostream->writeInt( mind->value->bot );
    mind->value->write( ostream );
  }

  Log::printEnd( " OK" );
}

void Nirvana::load()
{
  Log::print( "Loading Nirvana ..." );
  Log::printEnd( " OK" );
}

void Nirvana::unload()
{
  Log::print( "Unloading Nirvana ..." );

  devices.free();
  devices.deallocate();

  minds.free();
  minds.deallocate();

  Memo::pool.free();
  Mind::pool.free();

  Log::printEnd( " OK" );
}

void Nirvana::init()
{
  Log::println( "Initialising Nirvana {" );
  Log::indent();

  OZ_REGISTER_DEVICE( Memo );

  lua.init();

  updateModulo = 0;

  Log::unindent();
  Log::println( "}" );
}

void Nirvana::free()
{
  Log::println( "Freeing Nirvana {" );
  Log::indent();

  lua.free();

  deviceClasses.clear();
  deviceClasses.deallocate();

  Log::unindent();
  Log::println( "}" );
}

Nirvana nirvana;

}
}
