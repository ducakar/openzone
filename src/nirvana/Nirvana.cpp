/*
 *  Nirvana.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "Nirvana.h"

#include "matrix/Matrix.h"
#include "matrix/BotClass.h"
#include "Lua.h"

#include "LuaMind.h"
#include "RandomMind.h"

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
    for( typeof( minds.iterator() ) i = minds.iterator(); !i.isPassed(); ) {
      Mind *mind = i;
      ++i;

      if( world.objects[mind->botIndex] == null ) {
        minds.remove( mind );
        delete mind;
      }
    }
    // add minds for new bots
    foreach( i, synapse.addedObjects.iterator() ) {
      Object *obj = world.objects[*i];

      if( obj != null && ( obj->flags & Object::BOT_BIT ) ) {
        Bot *bot = static_cast<Bot*>( obj );

        if( ~bot->state & Bot::PLAYER_BIT ) {
          const BotClass *clazz = static_cast<const BotClass*>( bot->type );

          if( mindClasses.contains( clazz->mindType ) ) {
            minds << mindClasses.cachedValue().create( bot->index );
          }
          else {
            throw Exception( "Invalid mind type" );
          }
        }
      }
    }
  }

  void Nirvana::update()
  {
    int count = 0;
    foreach( mind, minds.iterator() ) {
      if( ( mind->flags & Mind::FORCE_UPDATE_BIT ) || count % UPDATE_INTERVAL == updateModulo ) {
        mind->update();
      }
      count++;
    }
    updateModulo = ( updateModulo + 1 ) % UPDATE_INTERVAL;
  }

  int Nirvana::run( void* )
  {
    try{
      while( nirvana.isAlive ) {
        uint timeBegin = SDL_GetTicks();

        // FIXME freezes on Windows, works fine on Wine?
#ifndef OZ_MINGW32
        nirvana.sync();

        // update minds
        nirvana.update();
#endif

        // notify matrix it can proceed changing the world
        SDL_SemPost( matrix.semaphore );

        timer.nirvanaMillis += SDL_GetTicks() - timeBegin;

        // wait until world has changed
        SDL_SemWait( nirvana.semaphore );
      }
    }
    catch( const Exception &e ) {
      log.resetIndent();
      log.println();
      log.println( "EXCEPTION: %s:%d: %s", e.file, e.line, e.message );

      if( log.isFile() ) {
        fprintf( stderr, "EXCEPTION: %s:%d: %s\n", e.file, e.line, e.message );
      }
      abort();
    }
    return 0;
  }

  void Nirvana::init()
  {
    log.println( "Initializing Nirvana {" );
    log.indent();

    lua.init();

    OZ_REGISTER_MINDCLASS();
    OZ_REGISTER_MINDCLASS( Lua );
    OZ_REGISTER_MINDCLASS( Random );

    semaphore = SDL_CreateSemaphore( 0 );

    log.unindent();
    log.println( "}" );
  }

  void Nirvana::free()
  {
    log.println( "Freeing Nirvana {" );
    log.indent();

    SDL_DestroySemaphore( semaphore );

    minds.free();
    mindClasses.clear();
    lua.free();

    log.unindent();
    log.println( "}" );
  }

  void Nirvana::load( InputStream *istream )
  {
    log.print( "Loading Nirvana ..." );

    if( istream != null ) {
      read( istream );
    }

    log.printEnd( " OK" );
  }

  void Nirvana::unload( OutputStream *ostream )
  {
    log.print( "Unloading Nirvana ..." );

    if( ostream != null ) {
      write( ostream );
    }
    minds.free();

    log.printEnd( " OK" );
  }

  void Nirvana::start()
  {
    log.print( "Starting Nirvana thread ..." );

    updateModulo = 0;
    isAlive = true;
    thread = SDL_CreateThread( run, null );

    log.printEnd( " OK" );
  }

  void Nirvana::stop()
  {
    log.print( "Stopping Nirvana thread ..." );

    isAlive = false;

    SDL_SemPost( semaphore );
    SDL_WaitThread( thread, null );

    thread = null;

    log.printEnd( " OK" );
  }

  void Nirvana::read( InputStream *istream )
  {
    String typeName;
    int nMinds = istream->readInt();

    for( int i = 0; i < nMinds; i++ ) {
      istream->readString( typeName );
      minds << mindClasses[typeName].read( istream );
    }
  }

  void Nirvana::write( OutputStream *ostream ) const
  {
    ostream->writeInt( minds.length() );

    foreach( mind, minds.iterator() ) {
      ostream->writeString( mind->type() );
      mind->write( ostream );
    }
  }

}
}
