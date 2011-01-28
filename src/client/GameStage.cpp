/*
 *  GameStage.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/GameStage.hpp"

#include "matrix/Bot.hpp"
#include "matrix/Matrix.hpp"
#include "nirvana/Nirvana.hpp"
#include "client/Network.hpp"
#include "client/Render.hpp"
#include "client/Sound.hpp"

#include "client/Camera.hpp"

namespace oz
{
namespace client
{

  GameStage gameStage;

  bool GameStage::update()
  {
    // wait until nirvana thread has stopped
    SDL_SemWait( matrix.semaphore );
    assert( SDL_SemValue( matrix.semaphore ) == 0 );

    // we can finally delete removed object after render and sound are sync'd (as model/audio dtors
    // have pointers to objects) and nirvana has read vector of removed objects and sync'd
    synapse.update();

    // clean events and remove destroyed objects
    matrix.cleanObjects();

    if( ui::keyboard.keys[SDLK_o] && !ui::keyboard.oldKeys[SDLK_o] ) {
      orbis.sky.time += orbis.sky.period * 0.25f;
    }

    camera.update();
    ui::ui.update();

    bool doQuit = ui::keyboard.keys[SDLK_ESCAPE] != 0;

    synapse.clearTickets();

    network.update();

    // update world
    matrix.update();
    // don't add any objects until next Game::update call or there will be index collisions in
    // nirvana

    // delete models and audio objects of removed objects
    oz::client::render.sync();
    sound.sync();

    if( !doQuit ) {
      // resume nirvana
      SDL_SemPost( nirvana::nirvana.semaphore );
    }

    camera.prepare();

    // play sounds, but don't do any cleanups
    sound.play();

    return !doQuit;
  }

  void GameStage::render()
  {
    // render graphics
    oz::client::render.update();
    // stop playing stopped continuous sounds, do cleanups
    sound.update();
  }

  void GameStage::load()
  {
    log.println( "Loading GameStage {" );
    log.indent();

    network.connect();

    Buffer buffer;
    String stateFile = config.get( "dir.rc", "" ) + String( "/default.ozState" );

    log.print( "Loading world stream from %s ...", stateFile.cstr() );
    if( buffer.read( stateFile ) ) {
      log.printEnd( " OK" );

      InputStream istream = buffer.inputStream();

      matrix.load( &istream );
      nirvana::nirvana.load( &istream );
    }
    else {
      log.printEnd( " Failed, starting a new world" );

      matrix.load( null );
      nirvana::nirvana.load( null );
    }

    nirvana::nirvana.start();

    camera.warp( Point3( 62, -29, 40 ) );

    log.unindent();
    log.println( "}" );
  }

  void GameStage::unload()
  {
    log.println( "Unloading GameStage {" );
    log.indent();

    nirvana::nirvana.stop();

    if( camera.bot != -1 ) {
      const_cast<Bot*>( camera.botObj )->state &= ~Bot::PLAYER_BIT;
    }

    Buffer buffer( 1024 * 1024 * 10 );
    OutputStream ostream = buffer.outputStream();
    String stateFile = config.get( "dir.rc", "" ) + String( "/default.ozState" );

    // to delete removed objects, world.unload only deletes those that haven't been removed yet
    synapse.update();

    matrix.unload( &ostream );
    nirvana::nirvana.unload( &ostream );

    log.print( "Writing world stream to %s ...", stateFile.cstr() );
    buffer.write( stateFile );
    log.printEnd( " OK" );

    network.disconnect();

    log.unindent();
    log.println( "}" );
  }

  void GameStage::init()
  {
    log.println( "Initialising GameStage {" );
    log.indent();

    matrix.init();
    nirvana::nirvana.init();

    log.unindent();
    log.println( "}" );
  }

  void GameStage::free()
  {
    log.println( "Freeing GameStage {" );
    log.indent();

    nirvana::nirvana.free();
    matrix.free();

    log.unindent();
    log.println( "}" );
  }

}
}
