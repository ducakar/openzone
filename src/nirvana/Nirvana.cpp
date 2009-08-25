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

  void Nirvana::update()
  {
    foreach( mind, minds.iterator() ) {
      if( requestSuspend ) {
        break;
      }
      mind->update();
    }
  }

  void Nirvana::run()
  {
    do {
      SDL_SemPost( matrix.semaphore );
      SDL_SemWait( semaphore );

      update();
    }
    while( isAlive );
  }

  int Nirvana::runThread( void* )
  {
    nirvana.run();
    return 0;
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

  void Nirvana::start()
  {
    log.print( "Starting Nirvana thread ..." );

    isAlive = true;
    requestSuspend = false;

    semaphore = SDL_CreateSemaphore( 1 );
    thread = SDL_CreateThread( runThread, null );

    log.printEnd( " OK" );
  }

  void Nirvana::stop()
  {
    log.print( "Stopping Nirvana thread ..." );

    isAlive = false;
    requestSuspend = true;

    SDL_SemPost( semaphore );
    SDL_WaitThread( thread, null );
    SDL_DestroySemaphore( semaphore );

    thread = null;

    log.printEnd( " OK" );
  }

  void Nirvana::sync()
  {
    // remove minds of removed bots
    for( DList<Mind, 0>::Iterator i( minds ); !i.isPassed(); ) {
      Mind *mind = i;
      ++i;

      if( world.objects[mind->botIndex] == null ) {
        minds.remove( mind );
      }
    }
    foreach( mind, pendingMinds.iterator() ) {
      minds << *mind;
    }
    pendingMinds.clear();
  }

}
