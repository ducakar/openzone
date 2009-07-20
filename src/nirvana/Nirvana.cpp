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

  void Nirvana::add( Mind *mind )
  {
    minds << mind;
  }

  void Nirvana::load()
  {
    logFile.println( "Loading Nirvana {" );
    logFile.indent();

//     add( new M_Walker( (Bot*) world.objects[1] ) );
//     add( new M_Walker( (Bot*) world.objects[2] ) );

    logFile.unindent();
    logFile.println( "}" );
  }

  void Nirvana::start()
  {
    logFile.print( "Starting Nirvana thread ..." );

    isAlive = true;
    requestSuspend = false;

    semaphore = SDL_CreateSemaphore( 1 );
    thread = SDL_CreateThread( runThread, null );

    logFile.printEnd( " OK" );
  }

  void Nirvana::stop()
  {
    logFile.print( "Stopping Nirvana thread ..." );

    isAlive = false;
    requestSuspend = true;

    SDL_SemPost( semaphore );
    SDL_WaitThread( thread, null );
    SDL_DestroySemaphore( semaphore );

    thread = null;

    logFile.printEnd( " OK" );
  }

  void Nirvana::free()
  {
    logFile.print( "Shutting down Nirvana ..." );

    minds.free();

    logFile.printEnd( " OK" );
  }

  void Nirvana::synchronize()
  {
    // add minds to new bots
    foreach( obj, world.objects.iterator() ) {
      if( *obj != null && ( *obj )->flags == Object::BOT_BIT ) {
        Bot *bot = (Bot*) *obj;

        if( bot->mind == null ) {
          bot->mind = new Mind();
          minds << bot->mind;
        }

        bot->mind->isUpdated = true;
      }
    }
    // remove unneccessary minds
    for( DList<Mind, 0>::Iterator i( minds ); !i.isPassed(); ) {
      Mind &mind = *i;
      ++i;

      if( !mind.isUpdated ) {
        minds.remove( &mind );
      }
    }
  }

  void Nirvana::think()
  {
    foreach( i, minds.iterator() ) {
      Mind &mind = *i;

      mind.update();
      mind.isUpdated = true;
    }
  }

  void Nirvana::run()
  {
    do {
      SDL_SemPost( matrix.semaphore );
      SDL_SemWait( semaphore );

      synchronize();

//       while( !requestSuspend ) {
        think();
//       }
//       requestSuspend = false;
    }
    while( isAlive );
  }

  int Nirvana::runThread( void *data )
  {
    // shut up, compiler!
    data = null;

    nirvana.run();

    return 0;
  }

}
