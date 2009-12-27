/*
 *  GameStage.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "GameStage.h"

#include "matrix/Bot.h"
#include "matrix/Matrix.h"
#include "nirvana/Nirvana.h"
#include "Network.h"
#include "Render.h"
#include "Sound.h"

#include "Camera.h"

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

    if( ui::keyboard.keys[SDLK_TAB] && !ui::keyboard.oldKeys[SDLK_TAB] ) {
      if( ui::mouse.doShow ) {
        ui::mouse.hide();
      }
      else {
        ui::mouse.show();
      }
    }

    camera.update();

    if( ui::keyboard.keys[SDLK_i] && !ui::keyboard.oldKeys[SDLK_i] ) {
      if( camera.bot == -1 ) {
        camera.bot = synapse.addObject( "DroidCommander", camera.p );

        Bot *me = static_cast<Bot*>( world.objects[camera.bot] );
        me->h = camera.h;
        me->v = camera.v;
        me->state |= Bot::PLAYER_BIT;

        camera.setBot( me );
        camera.setState( Camera::INTERNAL );
      }
      else {
        Bot *me = static_cast<Bot*>( world.objects[camera.bot] );
        me->kill();
      }
    }
    if( ui::keyboard.keys[SDLK_o] && !ui::keyboard.oldKeys[SDLK_o] ) {
      world.sky.time += world.sky.period * 0.25f;
    }

    if( ui::mouse.doShow ) {
      // interface
      ui::ui.update();
    }

    synapse.clearTickets();

    network.update();

    // update world
    matrix.update();
    // don't add any objects until next Game::update call or there will be index collisions in
    // nirvana

    // delete models and audio objects of removed objects
    oz::client::render.sync();
    sound.sync();

    // resume nirvana
    SDL_SemPost( nirvana::nirvana.semaphore );

    camera.prepare();

    // play sounds, but don't do any cleanups
    sound.play();

    return !ui::keyboard.keys[SDLK_ESCAPE];
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
    log.println( "Initializing Game {" );
    log.indent();

    matrix.init();
    nirvana::nirvana.init();

    network.connect();

    Buffer buffer( 1024 * 1024 * 10 );
    String stateFile = config.get( "dir.rc", "" ) + String( "/default.ozState" );

    log.print( "Loading world stream from %s ...", stateFile.cstr() );
    if( buffer.load( stateFile ) ) {
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

    log.unindent();
    log.println( "}" );
  }

  void GameStage::unload()
  {
    log.println( "Shutting down Game {" );
    log.indent();

    // remove myself
    if( camera.bot != -1 ) {
      synapse.remove( world.objects[camera.bot] );
    }
    synapse.update();

    Buffer buffer( 1024 * 1024 * 10 );
    OutputStream ostream = buffer.outputStream();
    String stateFile = config.get( "dir.rc", "" ) + String( "/default.ozState" );

    matrix.unload( &ostream );
    nirvana::nirvana.unload( &ostream );

    log.print( "Writing world stream to %s ...", stateFile.cstr() );
    buffer.write( stateFile );
    log.printEnd( " OK" );

    network.disconnect();

    nirvana::nirvana.free();
    matrix.free();

    log.unindent();
    log.println( "}" );
  }

  void GameStage::begin()
  {
    nirvana::nirvana.start();

    camera.warp( Vec3( 52, -49, 40 ) );
  }

  void GameStage::end()
  {
    nirvana::nirvana.stop();
  }

}
}
