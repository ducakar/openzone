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
#include "nirvana/Lua.hpp"

#include "nirvana/LuaMind.hpp"
#include "nirvana/RandomMind.hpp"

#define OZ_REGISTER_MINDCLASS( name ) \
  mindClasses.add( #name, MindCtor( &name##Mind::create, &name##Mind::read ) )

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
        const Bot* bot = static_cast<const Bot*>( obj );
        const BotClass* clazz = static_cast<const BotClass*>( bot->clazz );

        MindCtor* value = mindClasses.find( clazz->mindType );
        if( value != null ) {
          minds.add( value->create( bot->index ) );
        }
        else if( !clazz->mindType.isEmpty() ) {
          throw Exception( "Invalid mind type" );
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

  void Nirvana::load( InputStream* istream )
  {
    log.print( "Loading Nirvana ..." );

    if( istream != null ) {
      String typeName;
      int nMinds = istream->readInt();

      for( int i = 0; i < nMinds; ++i ) {
        typeName = istream->readString();
        if( !typeName.isEmpty() ) {
          minds.add( mindClasses.get( typeName ).read( istream ) );
        }
      }
    }

    log.printEnd( " OK" );
  }

  void Nirvana::unload( OutputStream* ostream )
  {
    log.print( "Unloading Nirvana ..." );

    if( ostream != null ) {
      ostream->writeInt( minds.length() );

      foreach( mind, minds.citer() ) {
        ostream->writeString( mind->type() );
        mind->write( ostream );
      }
    }
    minds.free();

    RandomMind::pool.free();
    LuaMind::pool.free();

    log.printEnd( " OK" );
  }

  void Nirvana::init()
  {
    log.println( "Initialising Nirvana {" );
    log.indent();

    lua.init();

    OZ_REGISTER_MINDCLASS( Lua );
    OZ_REGISTER_MINDCLASS( Random );

    updateModulo = 0;

    log.unindent();
    log.println( "}" );
  }

  void Nirvana::free()
  {
    log.println( "Freeing Nirvana {" );
    log.indent();

    mindClasses.clear();
    mindClasses.dealloc();
    lua.free();

    log.unindent();
    log.println( "}" );
  }

}
}
