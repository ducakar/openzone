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

#include "nirvana/Mind.hpp"
#include "nirvana/Lua.hpp"

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
      const Mind* mind = minds.find( *i );

      if( device != null ) {
        delete *device;
        devices.exclude( *i );
      }
      if( mind != null ) {
        minds.exclude( *i );
      }
    }
    // add minds for new bots
    foreach( i, synapse.addedObjects.citer() ) {
      const Object* obj = orbis.objects[*i];

      if( obj == null ) {
        continue;
      }

      if( obj->flags & Object::DEVICE_BIT ) {
      }
      else if( obj->flags & Object::BOT_BIT ) {
        const BotClass* clazz = static_cast<const BotClass*>( obj->clazz );

        if( !clazz->mindFunction.isEmpty() ) {
          minds.add( obj->index, Mind( obj->index ) );
        }
      }
    }
  }

  void Nirvana::update()
  {
    int count = 0;
    foreach( mind, minds.iter() ) {
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

    String typeName;
    int nMinds = istream->readInt();

    for( int i = 0; i < nMinds; ++i ) {
      int index = istream->readInt();

      minds.add( index, Mind( index, istream ) );
    }

    log.printEnd( " OK" );
  }

  void Nirvana::write( OutputStream* ostream ) const
  {
    log.print( "Writing Nirvana ..." );

    ostream->writeInt( minds.length() );

    foreach( mind, minds.citer() ) {
      mind.value().write( ostream );
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

    devices.clear();
    minds.clear();
    Mind::pool.free();

    log.printEnd( " OK" );
  }

  void Nirvana::init()
  {
    log.println( "Initialising Nirvana {" );
    log.indent();

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

    log.unindent();
    log.println( "}" );
  }

}
}
