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
#include "matrix/Synapse.hpp"
#include "matrix/Matrix.hpp"

#include "nirvana/Nirvana.hpp"

#include "client/Network.hpp"
#include "client/Loader.hpp"
#include "client/Render.hpp"
#include "client/Sound.hpp"

#include "client/Camera.hpp"

namespace oz
{
namespace client
{

  using oz::nirvana::nirvana;

  GameStage gameStage;

  int GameStage::auxMain( void* )
  {
    System::catchSignals();

    try{
      gameStage.run();
    }
    catch( const Exception& e ) {
      log.resetIndent();
      log.println();
      log.printException( e );
      log.println();

      if( oz::log.isFile() ) {
        fprintf( stderr, "\nEXCEPTION: %s\n", e.what() );
        fprintf( stderr, "  in %s\n\n", e.function );
        fprintf( stderr, "  at %s:%d\n\n", e.file, e.line );
      }
      abort();
    }
    catch( const std::exception& e ) {
      log.resetIndent();
      log.println();
      log.println( "EXCEPTION: %s", e.what() );
      log.println();

      if( log.isFile() ) {
        fprintf( stderr, "\nEXCEPTION: %s\n\n", e.what() );
      }
      abort();
    }
    return 0;
  }

  void GameStage::run()
  {
    uint beginTime;

    nirvana.sync();

    SDL_SemPost( mainSemaphore );
    SDL_SemPost( mainSemaphore );
    SDL_SemWait( auxSemaphore );

    while( isAlive ) {
      /*
       * PHASE 1
       */
      beginTime = SDL_GetTicks();

      network.update();

      // update world
      matrix.update();

      timer.matrixMillis += SDL_GetTicks() - beginTime;

      SDL_SemPost( mainSemaphore );
      SDL_SemWait( auxSemaphore );

      /*
       * PHASE 2
       */
      beginTime = SDL_GetTicks();

      // sync nirvana
      nirvana.sync();

      // now synapse lists are not needed any more
      synapse.update();

      // we can now add/remove object from the main thread after synapse lists have been cleared
      SDL_SemPost( mainSemaphore );

      // update minds
      nirvana.update();

      timer.nirvanaMillis += SDL_GetTicks() - beginTime;

      SDL_SemPost( mainSemaphore );
      SDL_SemWait( auxSemaphore );
    }
  }

  bool GameStage::update()
  {
    uint beginTime;
    uint currentTime;

    SDL_SemWait( mainSemaphore );

    beginTime = SDL_GetTicks();

    if( ui::keyboard.keys[SDLK_o] && !ui::keyboard.oldKeys[SDLK_o] ) {
      orbis.sky.time += orbis.sky.period * 0.125f;
    }

    camera.update();
    ui::ui.update();

    bool doQuit = ui::keyboard.keys[SDLK_ESCAPE] != 0;

    timer.uiMillis += SDL_GetTicks() - beginTime;

    SDL_SemPost( auxSemaphore );
    SDL_SemWait( mainSemaphore );

    beginTime = SDL_GetTicks();

    context.updateLoad();
    // clean up unused models, audios and audio sources
    loader.cleanup();
    // load scheduled resources
    loader.update();

    if( ui::keyboard.keys[SDLK_F11] && !ui::keyboard.oldKeys[SDLK_F11] ) {
      loader.makeScreenshot();
    }

    timer.loaderMillis += SDL_GetTicks() - beginTime;

    SDL_SemPost( auxSemaphore );
    SDL_SemWait( mainSemaphore );

    beginTime = SDL_GetTicks();

    camera.prepare();

    currentTime = SDL_GetTicks();
    timer.uiMillis += currentTime - beginTime;

    beginTime = currentTime;

    // play sounds, but don't do any cleanups
    sound.play();

    timer.soundMillis += SDL_GetTicks() - beginTime;

    return !doQuit;
  }

  void GameStage::present()
  {
    uint beginTime = SDL_GetTicks();

    render.draw( Render::DRAW_ORBIS_BIT | Render::DRAW_UI_BIT );
    sound.update();
    render.sync();

    timer.renderMillis += SDL_GetTicks() - beginTime;
  }

  void GameStage::begin()
  {
    log.println( "Preloading GameStage {" );
    log.indent();

    SDL_Delay( uint( config.get( "gameStage.loadingTime", 0.0f ) * 1000.0f ) );

    render.draw( Render::DRAW_UI_BIT );
    render.sync();

    sound.loadMusic( "music/04_fanatic-unreleased-rage.ogg" );

    camera.update();
    camera.prepare();

    render.draw( Render::DRAW_ORBIS_BIT | Render::DRAW_UI_BIT );
    loader.update();
    render.sync();

    sound.play();
    sound.update();

    ui::ui.showLoadingScreen( false );

    log.unindent();
    log.println( "}" );
  }

  void GameStage::end()
  {
    sound.unloadMusic();
  }

  void GameStage::load()
  {
    log.println( "Loading GameStage {" );
    log.indent();

    network.connect();

    if( config.getSet( "gameStage.autoload", true ) ) {
      String stateFile = config.get( "dir.rc", "" ) + String( "/default.ozState" );

      log.print( "Loading world stream from '%s' ...", stateFile.cstr() );
      Buffer buffer( 4 * 1024 * 1024 );

      if( buffer.read( stateFile ) ) {
        log.printEnd( " OK" );

        InputStream is = buffer.inputStream();

        matrix.load( &is );
        nirvana.load( &is );
      }
      else {
        log.printEnd( " Failed, starting a new world" );

        matrix.load( null );
        nirvana.load( null );
      }
    }
    else {
      log.println( "Initialising a new world" );

      matrix.load( null );
      nirvana.load( null );
    }

    camera.warp( Point3( 141.0f, -12.0f, 84.75f ) );

    log.print( "Starting auxilary thread ..." );

    isAlive = true;

    mainSemaphore = SDL_CreateSemaphore( 0 );
    auxSemaphore  = SDL_CreateSemaphore( 0 );
    auxThread     = SDL_CreateThread( auxMain, null );

    log.printEnd( " OK" );

    log.unindent();
    log.println( "}" );
  }

  void GameStage::unload()
  {
    log.println( "Unloading GameStage {" );
    log.indent();

    log.print( "Stopping auxilary thread ..." );

    isAlive = false;

    SDL_SemPost( auxSemaphore );
    SDL_SemPost( auxSemaphore );
    SDL_WaitThread( auxThread, null );

    SDL_DestroySemaphore( mainSemaphore );
    SDL_DestroySemaphore( auxSemaphore );

    mainSemaphore = null;
    auxSemaphore  = null;
    auxThread     = null;

    log.printEnd( " OK" );

    if( camera.bot != -1 ) {
      const_cast<Bot*>( camera.botObj )->state &= ~Bot::PLAYER_BIT;
    }

    if( config.getSet( "gameStage.autosave", true ) ) {
      OutputStream os = buffer.outputStream();
      String stateFile = config.get( "dir.rc", "" ) + String( "/default.ozState" );

      matrix.unload( &os );
      nirvana.unload( &os );

      log.print( "Writing world stream to %s ...", stateFile.cstr() );
      buffer.write( stateFile, os.length() );
      log.printEnd( " OK" );
    }
    else {
      matrix.unload( null );
      nirvana.unload( null );
    }

    network.disconnect();

    log.unindent();
    log.println( "}" );
  }

  void GameStage::init()
  {
    log.println( "Initialising GameStage {" );
    log.indent();

    matrix.init();
    nirvana.init();
    loader.init();

    log.unindent();
    log.println( "}" );
  }

  void GameStage::free()
  {
    log.println( "Freeing GameStage {" );
    log.indent();

    loader.free();
    nirvana.free();
    matrix.free();

    log.unindent();
    log.println( "}" );
  }

}
}
