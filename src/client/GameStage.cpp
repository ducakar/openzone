/*
 *  GameStage.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
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
    camera.update();

    // wait until nirvana thread has stopped
    SDL_SemWait( matrix.semaphore );
    assert( SDL_SemValue( matrix.semaphore ) == 0 );

    // we can finally delete removed object after render and sound are sync'd (as model/audio dtors
    // have pointers to objects) and nirvana has read vector of removed objects and sync'd
    synapse.update();

    // clean events and remove destroyed objects
    matrix.cleanObjects();

    // interface
    ui::ui.update();

    if( ui::keyboard.keys[SDLK_i] && !ui::keyboard.oldKeys[SDLK_i] ) {
      if( camera.bot == -1 ) {
        camera.bot = synapse.addObject( "DroidCommander", camera.p );

        Bot* me = static_cast<Bot*>( world.objects[camera.bot] );
        me->h = camera.h;
        me->v = camera.v;
        me->state |= Bot::PLAYER_BIT;

        camera.setBot( me );
        camera.setState( Camera::INTERNAL );
      }
      else {
        Bot* me = static_cast<Bot*>( world.objects[camera.bot] );
        me->kill();
      }
    }
    if( ui::keyboard.keys[SDLK_o] && !ui::keyboard.oldKeys[SDLK_o] ) {
      world.sky.time += world.sky.period * 0.25f;
    }
    bool doQuit = ui::keyboard.keys[SDLK_ESCAPE];

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

    nirvana::nirvana.start();

    camera.warp( Vec3( 52, -49, 40 ) );

    log.unindent();
    log.println( "}" );
  }

  void GameStage::unload()
  {
    log.println( "Unloading GameStage {" );
    log.indent();

    nirvana::nirvana.stop();

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

    log.unindent();
    log.println( "}" );
  }

  void GameStage::init()
  {
    log.println( "Initializing GameStage {" );
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
