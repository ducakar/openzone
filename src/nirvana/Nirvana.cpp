/*
 *  Nirvana.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Nirvana.h"

namespace oz
{

  Nirvana nirvana;

  void Nirvana::sync()
  {
    // remove minds of removed bots
    for( auto i( minds.iterator() ); !i.isPassed(); ) {
      Mind *mind = i;
      ++i;

      if( world.objects[mind->botIndex] == null ) {
        minds.remove( mind );
        delete mind;
      }
    }
    // add minds for new bots
    foreach( obj, synapse.putObjects.iterator() ) {
      if( ( *obj )->flags & Object::BOT_BIT ) {
        Bot *bot = (Bot*) *obj;

        if( !( bot->state & Bot::PLAYER_BIT ) ) {
          minds << new RandomMind( ( *obj )->index );
        }
      }
    }
  }

  void Nirvana::update()
  {
    foreach( mind, minds.iterator() ) {
      mind->update();
    }
  }

  int Nirvana::run( void* )
  {
    while( nirvana.isAlive ) {
      nirvana.sync();

      // update minds
      nirvana.update();

      // notify matrix it can proceed changing the world
      SDL_SemPost( matrix.semaphore );
      // wait until world has changed
      SDL_SemWait( nirvana.semaphore );
    }
    return 0;
  }

  void Nirvana::start()
  {
    log.print( "Starting Nirvana thread ..." );

    isAlive = true;

    semaphore = SDL_CreateSemaphore( 0 );
    thread = SDL_CreateThread( run, null );

    log.printEnd( " OK" );
  }

  void Nirvana::stop()
  {
    log.print( "Stopping Nirvana thread ..." );

    isAlive = false;

    SDL_SemPost( semaphore );
    SDL_WaitThread( thread, null );
    SDL_DestroySemaphore( semaphore );

    thread = null;

    log.printEnd( " OK" );
  }

  void Nirvana::load()
  {
    log.println( "Loading Nirvana {" );
    log.indent();

    log.unindent();
    log.println( "}" );
  }

  void Nirvana::free()
  {
    log.print( "Shutting down Nirvana ..." );

    minds.free();

    log.printEnd( " OK" );
  }

}
