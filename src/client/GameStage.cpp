/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file client/GameStage.cpp
 */

#include "stable.hpp"

#include "client/GameStage.hpp"

#include "matrix/Bot.hpp"
#include "matrix/Synapse.hpp"
#include "matrix/Matrix.hpp"

#include "nirvana/Nirvana.hpp"

#include "client/Loader.hpp"
#include "client/Render.hpp"
#include "client/Sound.hpp"
#include "client/Network.hpp"
#include "client/Camera.hpp"
#include "client/Lua.hpp"
#include "client/MenuStage.hpp"
#include "client/Module.hpp"

namespace oz
{
namespace client
{

using oz::matrix::matrix;
using oz::nirvana::nirvana;

GameStage gameStage;

String GameStage::AUTOSAVE_FILE;
String GameStage::QUICKSAVE_FILE;

int GameStage::auxMain( void* )
{
  System::catchSignals();
#ifndef NDEBUG
  System::enableHalt( true );
#endif

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

void GameStage::reload()
{
  ui::ui.loadingScreen->status.setText( gettext( "Loading ..." ) );
  ui::ui.showLoadingScreen( true );
  ui::ui.root->focus( ui::ui.loadingScreen );

  render.draw( Render::DRAW_UI_BIT );
  render.sync();

  for( int i = modules.length() - 1; i >= 0; --i ) {
    modules[i]->unload();
  }

  context.unload();

  lua.free();

  nirvana.unload();
  matrix.unload();

  matrix.load();
  nirvana.load();

  lua.init();
  for( int i = 0; i < modules.length(); ++i ) {
    modules[i]->registerLua();
  }

  context.load();

  for( int i = modules.length() - 1; i >= 0; --i ) {
    modules[i]->load();
  }

  if( stateFile.isEmpty() ) {
    log.println( "Initialising new world" );

    lua.create( "lua/mission/" + missionFile + ".lua" );

    if( orbis.terra.id == -1 || orbis.caelum.id == -1 ) {
      throw Exception( "Terrain and Caelum must both be loaded via the client.onCreate" );
    }
  }
  else {
    if( !read( stateFile ) ) {
      throw Exception( "reading saved state '" + stateFile + "' failed" );
    }
  }

  nirvana.sync();
  synapse.update();

  camera.update();
  camera.prepare();

  render.draw( Render::DRAW_ORBIS_BIT | Render::DRAW_UI_BIT );
  loader.update();
  render.sync();

  sound.play();
  sound.update();

  ui::ui.showLoadingScreen( false );
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

  if( ui::keyboard.keys[SDLK_TAB] && !ui::keyboard.oldKeys[SDLK_TAB] ) {
    ui::mouse.doShow = !ui::mouse.doShow;
  }

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
    stateFile = QUICKSAVE_FILE;
    reload();
  }
  if( ui::keyboard.keys[SDLK_F8] && !ui::keyboard.oldKeys[SDLK_F8] ) {
    stateFile = AUTOSAVE_FILE;
    reload();
  }
  if( ui::keyboard.keys[SDLK_ESCAPE] ) {
    Stage::nextStage = &menuStage;
  }

  camera.update();
  ui::ui.update();

  for( int i = 0; i < modules.length(); ++i ) {
    modules[i]->update();
  }

  lua.update();

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

  // clean up unused imagines, sones and sources
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

  return true;
}

void GameStage::present()
{
  uint beginTime = SDL_GetTicks();

  render.draw( Render::DRAW_ORBIS_BIT | Render::DRAW_UI_BIT );
  sound.update();
  render.sync();

  timer.renderMillis += SDL_GetTicks() - beginTime;
}

bool GameStage::read( const char* path )
{
  log.print( "Loading state from '%s' ...", path );

  File file( path );
  if( !file.map() ) {
    log.printEnd( " Failed" );
    return false;
  }

  log.printEnd( " OK" );

  InputStream istream = file.inputStream();

  matrix.read( &istream );
  nirvana.read( &istream );
  camera.read( &istream );
  lua.read( &istream );

  for( int i = 0; i < modules.length(); ++i ) {
    modules[i]->read( &istream );
  }

  file.unmap();

  return true;
}

void GameStage::write( const char* path ) const
{
  BufferStream ostream;

  matrix.write( &ostream );
  nirvana.write( &ostream );
  camera.write( &ostream );
  lua.write( &ostream );

  for( int i = 0; i < modules.length(); ++i ) {
    modules[i]->write( &ostream );
  }

  log.print( "Saving state to %s ...", path );

  if( File( path ).write( &ostream ) ) {
    log.printEnd( " Failed" );
  }
  else {
    log.printEnd( " OK" );
  }
}

void GameStage::load()
{
  log.println( "Loading GameStage {" );
  log.indent();

  ui::ui.loadingScreen->status.setText( gettext( "Loading ..." ) );
  ui::ui.loadingScreen->show( true );

  render.draw( Render::DRAW_UI_BIT );
  render.sync();

  matrix.load();
  nirvana.load();

  network.connect();

  log.print( "Starting auxilary thread ..." );

  isAlive = true;

  mainSemaphore = SDL_CreateSemaphore( 0 );
  auxSemaphore  = SDL_CreateSemaphore( 0 );
  auxThread     = SDL_CreateThread( auxMain, null );

  log.printEnd( " OK" );

  lua.init();
  for( int i = 0; i < modules.length(); ++i ) {
    modules[i]->registerLua();
  }

  context.load();
  render.load();

  camera.reset();

  for( int i = modules.length() - 1; i >= 0; --i ) {
    modules[i]->load();
  }

  if( stateFile.isEmpty() ) {
    log.println( "Initialising new world" );

    lua.create( "lua/mission/" + missionFile + ".lua" );

    if( orbis.terra.id == -1 || orbis.caelum.id == -1 ) {
      throw Exception( "Terrain and Caelum must both be loaded via the client.onCreate" );
    }
  }
  else if( !read( stateFile ) ) {
    throw Exception( "reading saved state '" + stateFile + "' failed" );
  }

  nirvana.sync();
  synapse.update();

  ui::mouse.buttons = 0;
  ui::mouse.currButtons = 0;

  camera.update();
  camera.prepare();

  ui::ui.showLoadingScreen( true );

  render.draw( Render::DRAW_ORBIS_BIT | Render::DRAW_UI_BIT );
  loader.update();
  render.sync();

  sound.play();
  sound.update();

  ui::ui.showLoadingScreen( false );

  isLoaded = true;

  log.unindent();
  log.println( "}" );
}

void GameStage::unload()
{
  log.println( "Unloading GameStage {" );
  log.indent();

  ui::ui.loadingScreen->status.setText( gettext( "Shutting down ..." ) );
  ui::ui.showLoadingScreen( true );

  render.draw( Render::DRAW_UI_BIT );
  render.sync();

  if( isLoaded ) {
    write( AUTOSAVE_FILE );
  }

  for( int i = modules.length() - 1; i >= 0; --i ) {
    modules[i]->unload();
  }

  render.unload();
  context.unload();

  lua.free();

  log.print( "Stopping auxilary thread ..." );

  isAlive = false;

  SDL_SemPost( auxSemaphore );
  SDL_WaitThread( auxThread, null );

  SDL_DestroySemaphore( mainSemaphore );
  SDL_DestroySemaphore( auxSemaphore );

  mainSemaphore = null;
  auxSemaphore  = null;
  auxThread     = null;

  log.printEnd( " OK" );

  network.disconnect();

  nirvana.unload();
  matrix.unload();

  ui::ui.showLoadingScreen( false );

  isLoaded = false;

  log.unindent();
  log.println( "}" );
}

void GameStage::init()
{
  isLoaded = false;

  log.println( "Initialising GameStage {" );
  log.indent();

  AUTOSAVE_FILE = config.get( "dir.rc", "" ) + String( "/autosave.ozState" );
  QUICKSAVE_FILE = config.get( "dir.rc", "" ) + String( "/quicksave.ozState" );

  Module::listModules( &modules );

  matrix.init();
  nirvana.init();
  loader.init();

  for( int i = modules.length() - 1; i >= 0; --i ) {
    modules[i]->init();
  }

  log.unindent();
  log.println( "}" );
}

void GameStage::free()
{
  log.println( "Freeing GameStage {" );
  log.indent();

  for( int i = modules.length() - 1; i >= 0; --i ) {
    modules[i]->free();
  }
  modules.clear();
  modules.dealloc();

  loader.free();
  nirvana.free();
  matrix.free();

  stateFile.dealloc();
  missionFile.dealloc();

  AUTOSAVE_FILE.dealloc();
  QUICKSAVE_FILE.dealloc();

  log.unindent();
  log.println( "}" );
}

}
}
