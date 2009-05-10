/*
 *  Game.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Game.h"

#include "nirvana/Nirvana.h"
#include "Network.h"

namespace oz
{
namespace server
{

  Game game;

  bool Game::init()
  {
    network.connect();

    logFile.println( "Loading Matrix {" );
    logFile.indent();
    matrix.load();
    logFile.unindent();
    logFile.println( "}" );

    logFile.println( "Loading Nirvana {" );
    logFile.indent();
    nirvana.load();
    logFile.unindent();
    logFile.println( "}" );

    return true;
  }

  void Game::start()
  {
    logFile.print( "Starting Nirvana thread ..." );
    nirvana.start();
    logFile.printEnd( " OK" );
  }

  void Game::update( int time )
  {
    nirvana.requestSuspend = true;
    SDL_SemWait( matrix.semaphore );

    timer.update( time );
    matrix.update();
    network.update();

    SDL_SemPost( nirvana.semaphore );
  }

  void Game::stop()
  {
    logFile.print( "Stopping Nirvana thread ..." );
    nirvana.stop();
    logFile.printEnd( " OK" );
  }

  void Game::free()
  {
    logFile.print( "Shutting down Nirvana ..." );
    logFile.indent();

    nirvana.free();

    logFile.unindent();
    logFile.printEnd( " OK" );

    logFile.print( "Shutting down Matrix ..." );
    logFile.indent();

    matrix.free();

    logFile.unindent();
    logFile.printEnd( " OK" );
  }

}
}
