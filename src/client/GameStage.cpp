/*
 *  GameStage.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
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
#include "client/Lua.hpp"

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

      // update minds
      nirvana.update();

      timer.nirvanaMillis += SDL_GetTicks() - beginTime;

      // we can now manipulate world from the main thread after synapse lists have been cleared
      // and nirvana is not accessing matrix any more
      SDL_SemPost( mainSemaphore );

      /*
       * PHASE 3
       */

      SDL_SemPost( mainSemaphore );
      SDL_SemWait( auxSemaphore );
    }
  }

  bool GameStage::update()
  {
    uint beginTime;
    uint currentTime;

    SDL_SemWait( mainSemaphore );

    /*
     * PHASE 3
     */

    beginTime = SDL_GetTicks();

    if( ui::keyboard.keys[SDLK_o] ) {
      if( ui::keyboard.keys[SDLK_LSHIFT] || ui::keyboard.keys[SDLK_RSHIFT] ) {
        orbis.caelum.time -= orbis.caelum.period * 0.002f;
      }
      else {
        orbis.caelum.time += orbis.caelum.period * 0.002f;
      }
    }

    if( ui::keyboard.keys[SDLK_F5] && !ui::keyboard.oldKeys[SDLK_F5] ) {
      write( config.get( "dir.rc", "" ) + String( "/quicksave.ozState" ) );
    }
    if( ui::keyboard.keys[SDLK_F7] && !ui::keyboard.oldKeys[SDLK_F7] ) {
      ui::ui.loadingScreen->status.setText( gettext( "Loading ..." ) );
      ui::ui.showLoadingScreen( true );
      ui::ui.root->focus( ui::ui.loadingScreen );

      render.draw( Render::DRAW_UI_BIT );
      render.sync();

      clear();
      read( config.get( "dir.rc", "" ) + String( "/quicksave.ozState" ) );

      camera.update();
      camera.prepare();

      render.draw( Render::DRAW_ORBIS_BIT | Render::DRAW_UI_BIT );
      loader.update();
      render.sync();

      sound.play();
      sound.update();

      ui::ui.showLoadingScreen( false );
    }
    if( ui::keyboard.keys[SDLK_F8] && !ui::keyboard.oldKeys[SDLK_F8] ) {
      ui::ui.loadingScreen->status.setText( gettext( "Loading ..." ) );
      ui::ui.showLoadingScreen( true );
      ui::ui.root->focus( ui::ui.loadingScreen );

      render.draw( Render::DRAW_UI_BIT );
      render.sync();

      clear();
      camera.clear();

      if( ui::keyboard.keys[SDLK_LCTRL] || ui::keyboard.keys[SDLK_RCTRL] ) {
        read( null );
      }
      else {
        read( config.get( "dir.rc", "" ) + String( "/autosave.ozState" ) );
      }

      camera.update();
      camera.prepare();

      render.draw( Render::DRAW_ORBIS_BIT | Render::DRAW_UI_BIT );
      loader.update();
      render.sync();

      sound.play();
      sound.update();

      ui::ui.showLoadingScreen( false );
    }

    camera.update();
    ui::ui.update();

    timer.uiMillis += SDL_GetTicks() - beginTime;

    SDL_SemPost( auxSemaphore );
    SDL_SemWait( mainSemaphore );

    /*
     * PHASE 1
     */

    beginTime = SDL_GetTicks();

#ifndef NDEBUG
    context.updateLoad();
#endif

    // clean up unused models, audios and audio sources
    loader.cleanup();
    // load scheduled resources
    loader.update();

    timer.loaderMillis += SDL_GetTicks() - beginTime;

    SDL_SemPost( auxSemaphore );
    SDL_SemWait( mainSemaphore );

    /*
     * PHASE 2
     */

    beginTime = SDL_GetTicks();

    camera.prepare();

    currentTime = SDL_GetTicks();
    timer.uiMillis += currentTime - beginTime;

    beginTime = currentTime;

    // play sounds, but don't do any cleanups
    sound.play();

    timer.soundMillis += SDL_GetTicks() - beginTime;

    return !ui::keyboard.keys[SDLK_ESCAPE];
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
    ui::ui.loadingScreen->status.setText( gettext( "Shutting down ..." ) );
    ui::ui.loadingScreen->show( true );
    ui::ui.root->focus( ui::ui.loadingScreen );

    render.draw( Render::DRAW_UI_BIT );
    render.sync();
  }

  bool GameStage::read( const char* file )
  {
    if( file == null ) {
      log.println( "Initialising new world" );

      matrix.read( null );

      lua.staticCall( onCreate );

      if( orbis.terra.id == -1 || orbis.caelum.id == -1 ) {
        throw Exception( "Terrain and Caelum must both be loaded via the client.onCreate" );
      }
    }
    else {
      log.print( "Loading state from '%s' ...", file );

      Buffer buffer;
      if( !buffer.read( file ) ) {
        log.printEnd( " Failed" );
        return false;
      }

      log.printEnd( " OK" );

      InputStream is = buffer.inputStream();

      matrix.read( &is );
      nirvana.read( &is );
      camera.read( &is );
    }

    return true;
  }

  void GameStage::write( const char* file ) const
  {
    Buffer buffer( 4 * 1024 * 1024 );
    OutputStream os = buffer.outputStream();

    matrix.write( &os );
    nirvana.write( &os );
    camera.write( &os );

    log.print( "Saving state to %s ...", file );
    buffer.write( file, os.length() );
    log.printEnd( " OK" );
  }

  void GameStage::clear()
  {
    context.unload();

    nirvana.unload();
    matrix.unload();

    matrix.load();
    nirvana.load();

    context.load();
  }

  void GameStage::load()
  {
    log.println( "Loading GameStage {" );
    log.indent();

    matrix.load();
    nirvana.load();

    if( !config.getSet( "gameStage.autoload", true ) ||
        !read( config.get( "dir.rc", "" ) + String( "/autosave.ozState" ) ) )
    {
      read( null );
    }

    network.connect();

    log.print( "Starting auxilary thread ..." );

    isAlive = true;

    mainSemaphore = SDL_CreateSemaphore( 0 );
    auxSemaphore  = SDL_CreateSemaphore( 0 );
    auxThread     = SDL_CreateThread( auxMain, null );

    SDL_SemWait( mainSemaphore );

    log.printEnd( " OK" );

    context.load();

    render.draw( Render::DRAW_UI_BIT );
    render.sync();
    render.load();
    render.draw( Render::DRAW_UI_BIT );
    render.sync();

    isLoaded = true;

    log.unindent();
    log.println( "}" );
  }

  void GameStage::unload()
  {
    log.println( "Unloading GameStage {" );
    log.indent();

    render.unload();
    context.unload();

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

    network.disconnect();

    if( isLoaded && config.getSet( "gameStage.autosave", true ) ) {
      write( config.get( "dir.rc", "" ) + String( "/autosave.ozState" ) );
    }

    nirvana.unload();
    matrix.unload();

    isLoaded = false;

    log.unindent();
    log.println( "}" );
  }

  void GameStage::init()
  {
    isLoaded = false;

    log.println( "Initialising GameStage {" );
    log.indent();

    ui::ui.loadingScreen->status.setText( gettext( "Loading ..." ) );
    ui::ui.loadingScreen->show( true );

    render.draw( Render::DRAW_UI_BIT );
    render.sync();

    onCreate = config.getSet( "client.onCreate", "client_onCreate" );

    if( onCreate.isEmpty() ) {
      throw Exception( "missing client.onCreate setting" );
    }

    matrix.init();
    nirvana.init();
    loader.init();
    lua.init();

    log.unindent();
    log.println( "}" );
  }

  void GameStage::free()
  {
    log.println( "Freeing GameStage {" );
    log.indent();

    lua.free();
    loader.free();
    nirvana.free();
    matrix.free();

    log.unindent();
    log.println( "}" );
  }

}
}
