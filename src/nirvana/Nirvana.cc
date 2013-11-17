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
#include <matrix/Bot.hh>
#include <nirvana/LuaNirvana.hh>
#include <nirvana/Memo.hh>
#include <nirvana/QuestList.hh>
#include <nirvana/TechGraph.hh>

#define OZ_REGISTER_DEVICE( name ) \
  deviceClasses.add( #name, &name::create )

namespace oz
{

void Nirvana::sync()
{
  // remove devices and minds of removed objects
  foreach( i, synapse.removedObjects.citer() ) {
    const Device* const* device = devices.find( *i );
    const Mind* mind = minds.find( *i );

    if( device != nullptr ) {
      delete *device;
      devices.exclude( *i );
    }
    if( mind != nullptr ) {
      minds.exclude( *i );
    }
  }
  // add minds for new bots
  foreach( i, synapse.addedObjects.citer() ) {
    const Object* obj = orbis.obj( *i );

    if( obj != nullptr && ( obj->flags & Object::BOT_BIT ) ) {
      minds.add( obj->index, Mind( obj->index ) );
    }
  }
}

void Nirvana::update()
{
  int count = 0;
  foreach( i, minds.iter() ) {
    Mind& mind = i->value;

    const Bot* bot = static_cast<const Bot*>( orbis.obj( mind.bot ) );
    hard_assert( bot != nullptr && ( bot->flags & Object::BOT_BIT ) );

    if( !( bot->state & Bot::PLAYER_BIT ) &&
        ( ( mind.flags & Mind::FORCE_UPDATE_BIT ) || count % UPDATE_INTERVAL == updateModulo ) )
    {
      mind.update();
    }
    ++count;
  }
  updateModulo = ( updateModulo + 1 ) % UPDATE_INTERVAL;

  techGraph.update();
}

void Nirvana::read( InputStream* is )
{
  Log::print( "Reading Nirvana ..." );

  luaNirvana.read( is );

  String typeName;

  int nDevices = is->readInt();
  int nMinds   = is->readInt();

  for( int i = 0; i < nDevices; ++i ) {
    int index   = is->readInt();
    String type = is->readString();

    Device::CreateFunc* const* func = deviceClasses.find( type );

    if( func == nullptr ) {
      OZ_ERROR( "Invalid device type '%s'", type.cstr() );
    }

    devices.add( index, ( *func )( index, is ) );
  }
  for( int i = 0; i < nMinds; ++i ) {
    int index = is->readInt();

    minds.add( index, Mind( index, is ) );
  }

  questList.read( is );
  techGraph.read( is );

  Log::printEnd( " OK" );
}

void Nirvana::write( OutputStream* os ) const
{
  luaNirvana.write( os );

  os->writeInt( devices.length() );
  os->writeInt( minds.length() );

  foreach( device, devices.citer() ) {
    os->writeInt( device->key );
    os->writeString( device->value->type() );

    device->value->write( os );
  }
  foreach( mind, minds.citer() ) {
    os->writeInt( mind->value.bot );
    mind->value.write( os );
  }

  questList.write( os );
  techGraph.write( os );
}

void Nirvana::load()
{
  Log::print( "Loading Nirvana ..." );

  questList.load();
  techGraph.load();

  Log::printEnd( " OK" );
}

void Nirvana::unload()
{
  Log::print( "Unloading Nirvana ..." );

  devices.free();
  devices.deallocate();

  minds.clear();
  minds.deallocate();

  Memo::pool.free();

  questList.unload();
  techGraph.unload();

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
