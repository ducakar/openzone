/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#include <nirvana/Nirvana.hh>

#include <matrix/Synapse.hh>
#include <matrix/Matrix.hh>
#include <matrix/Bot.hh>
#include <nirvana/LuaNirvana.hh>
#include <nirvana/Memo.hh>
#include <nirvana/TechTree.hh>
#include <nirvana/QuestList.hh>

#define OZ_REGISTER_DEVICE( name ) \
  deviceClasses.add( #name, &name::create )

namespace oz
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
    const Object* obj = orbis.obj( *i );

    if( obj != nullptr && ( obj->flags & Object::BOT_BIT ) ) {
      minds.add( obj->index, new Mind( obj->index ) );
    }
  }
}

void Nirvana::update()
{
  int count = 0;
  foreach( i, minds.citer() ) {
    Mind* mind = i->value;

    const Bot* bot = static_cast<const Bot*>( orbis.obj( mind->bot ) );
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

  luaNirvana.read( istream );

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

  techTree.read( istream );
  questList.read( istream );

  Log::printEnd( " OK" );
}

void Nirvana::write( OutputStream* ostream ) const
{
  Log::print( "Writing Nirvana ..." );

  luaNirvana.write( ostream );

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

  techTree.write( ostream );
  questList.write( ostream );

  Log::printEnd( " OK" );
}

void Nirvana::load()
{
  Log::print( "Loading Nirvana ..." );

  techTree.load();
  questList.load();

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

  techTree.unload();
  questList.unload();

  Log::printEnd( " OK" );
}

void Nirvana::init()
{
  Log::println( "Initialising Nirvana {" );
  Log::indent();

  OZ_REGISTER_DEVICE( Memo );

  luaNirvana.init();

  updateModulo = 0;

  Log::unindent();
  Log::println( "}" );
}

void Nirvana::destroy()
{
  Log::println( "Destroy Nirvana {" );
  Log::indent();

  luaNirvana.destroy();

  deviceClasses.clear();
  deviceClasses.deallocate();

  Log::unindent();
  Log::println( "}" );
}

Nirvana nirvana;

}
