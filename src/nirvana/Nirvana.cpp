/*
 *  Nirvana.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
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
    // remove minds of removed bots
    for( auto i = minds.iter(); i.isValid(); ) {
      Mind* mind = i;
      ++i;

      if( orbis.objects[mind->bot] == null ) {
        minds.remove( mind );
        delete mind;
      }
    }
    // add minds for new bots
    foreach( i, synapse.addedObjects.citer() ) {
      const Object* obj = orbis.objects[*i];

      if( obj != null && ( obj->flags & Object::BOT_BIT ) ) {
        const BotClass* clazz = static_cast<const BotClass*>( obj->clazz );

        if( !clazz->mindFunction.isEmpty() ) {
          minds.add( new Mind( obj->index ) );
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
      minds.add( new Mind( istream ) );
    }

    log.printEnd( " OK" );
  }

  void Nirvana::write( OutputStream* ostream ) const
  {
    log.print( "Writing Nirvana ..." );

    ostream->writeInt( minds.length() );

    foreach( mind, minds.citer() ) {
      mind->write( ostream );
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

    minds.free();
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
