/*
 *  Game.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "stable.hpp"

#include "Game.hpp"

#include "matrix/Matrix.hpp"
#include "nirvana/Nirvana.hpp"
#include "Network.hpp"

namespace oz
{
namespace server
{

  Game game;

  bool Game::init()
  {
    network.connect();

    log.println( "Loading Matrix {" );
    log.indent();
//    matrix.load();
    log.unindent();
    log.println( "}" );

    log.println( "Loading Nirvana {" );
    log.indent();
//    nirvana.load();
    log.unindent();
    log.println( "}" );

    return true;
  }

  void Game::start()
  {
    log.print( "Starting Nirvana thread ..." );
    nirvana::nirvana.start();
    log.printEnd( " OK" );
  }

  void Game::update()
  {
    SDL_SemWait( matrix.semaphore );

    timer.tick();
    matrix.update();
    network.update();

    SDL_SemPost( nirvana::nirvana.semaphore );
  }

  void Game::stop()
  {
    log.print( "Stopping Nirvana thread ..." );
    nirvana::nirvana.stop();
    log.printEnd( " OK" );
  }

  void Game::free()
  {
    log.print( "Shutting down Nirvana ..." );
    log.indent();

//    nirvana.free();

    log.unindent();
    log.printEnd( " OK" );

    log.print( "Shutting down Matrix ..." );
    log.indent();

//    matrix.free();

    log.unindent();
    log.printEnd( " OK" );
  }

}
}
