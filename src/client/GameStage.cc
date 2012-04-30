/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 */

/**
 * @file client/GameStage.cc
 */

#include "stable.hh"

#include "client/GameStage.hh"

#include "matrix/Synapse.hh"
#include "matrix/Matrix.hh"

#include "nirvana/Nirvana.hh"

#include "modules/Modules.hh"

#include "client/Loader.hh"
#include "client/Render.hh"
#include "client/Sound.hh"
#include "client/Network.hh"
#include "client/Camera.hh"
#include "client/Lua.hh"
#include "client/Profile.hh"
#include "client/QuestList.hh"
#include "client/MenuStage.hh"

#include "client/ui/LoadingArea.hh"

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
  try {
    gameStage.auxRun();
  }
  catch( const std::exception& e ) {
    oz::log.verboseMode = false;
    oz::log.printException( &e );

    oz::System::bell();
    oz::System::abort();
  }
  return 0;
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

  log.println( "Reading Client {" );
  log.indent();

  questList.read( &istream );
  camera.read( &istream );
  modules.read( &istream );

  lua.read( &istream );

  log.unindent();
  log.println( "}" );

  file.unmap();

  return true;
}

void GameStage::write( const char* path ) const
{
  BufferStream ostream;

  matrix.write( &ostream );
  nirvana.write( &ostream );

  log.print( "Writing Client ..." );

  questList.write( &ostream );
  camera.write( &ostream );
  modules.write( &ostream );

  lua.write( &ostream );

  log.printEnd( " OK" );

  log.print( "Saving state to %s ...", path );

  if( !File( path ).write( ostream.begin(), ostream.length() ) ) {
    log.printEnd( " Failed" );
  }
  else {
    log.printEnd( " OK" );
  }
}

void GameStage::reload()
{
  log.print( "[" );
  log.printTime();
  log.printEnd( "] Reloading GameStage {" );
  log.indent();

  ui::mouse.doShow = false;
  ui::ui.loadingScreen->status.setText( "%s", OZ_GETTEXT( "Loading ..." ) );
  ui::ui.showLoadingScreen( true );
  ui::ui.loadingScreen->raise();

  loader.unload();

  render.draw( Render::DRAW_UI_BIT );
  render.draw( Render::DRAW_UI_BIT );
  render.swap();

  camera.reset();

  modules.unload();
  context.unload();
  render.unload();
  questList.unload();

  lua.free();

  nirvana.unload();
  matrix.unload();

  matrix.load();
  nirvana.load();

  lua.init();
  modules.registerLua();

  questList.load();
  render.load();
  context.load();
  modules.load();

  if( stateFile.isEmpty() ) {
    log.println( "Initialising new world" );

    lua.create( mission );
  }
  else {
    if( !read( stateFile ) ) {
      throw Exception( "Reading saved state '%s' failed", stateFile.cstr() );
    }
  }

  nirvana.sync();
  synapse.update();

  camera.prepare();
  camera.update();

  render.draw( Render::DRAW_ORBIS_BIT | Render::DRAW_UI_BIT );
  loader.syncUpdate();

  render.draw( Render::DRAW_ORBIS_BIT | Render::DRAW_UI_BIT );
  loader.syncUpdate();

  sound.play();
  render.swap();
  sound.sync();

  loader.load();

  ui::ui.prepare();
  ui::ui.showLoadingScreen( false );

  log.unindent();
  log.println( "}" );
}

void GameStage::auxRun()
{
  uint beginMicros;

  SDL_SemPost( mainSemaphore );
  SDL_SemPost( mainSemaphore );
  SDL_SemWait( auxSemaphore );

  while( isAuxAlive ) {
    /*
     * PHASE 2
     */

    beginMicros = Time::uclock();

    network.update();

    // update world
    matrix.update();

    matrixMicros += Time::uclock() - beginMicros;

    loader.hasTime = false;

    SDL_SemPost( mainSemaphore );
    SDL_SemWait( auxSemaphore );

    /*
     * PHASE 3
     */

    beginMicros = Time::uclock();

    // sync nirvana
    nirvana.sync();

    // now synapse lists are not needed any more
    synapse.update();

    // update minds
    nirvana.update();

    nirvanaMicros += Time::uclock() - beginMicros;

    // we can now manipulate world from the main thread after synapse lists have been cleared
    // and nirvana is not accessing matrix any more
    SDL_SemPost( mainSemaphore );

    /*
     * PHASE 1
     */

    SDL_SemPost( mainSemaphore );
    SDL_SemWait( auxSemaphore );
  }
}

bool GameStage::update()
{
  uint beginMicros;

  SDL_SemWait( mainSemaphore );

  /*
   * PHASE 1
   */

  beginMicros = Time::uclock();

  if( ui::keyboard.keys[SDLK_F9] && !ui::keyboard.oldKeys[SDLK_F9] ) {
    write( QUICKSAVE_FILE );
  }
  if( ui::keyboard.keys[SDLK_F10] && !ui::keyboard.oldKeys[SDLK_F10] ) {
    if( ui::keyboard.keys[SDLK_LCTRL] || ui::keyboard.keys[SDLK_RCTRL] ) {
      stateFile = AUTOSAVE_FILE;
    }
    else {
      stateFile = QUICKSAVE_FILE;
    }
    reload();
  }
  if( ui::keyboard.keys[SDLK_ESCAPE] ) {
    Stage::nextStage = &menuStage;
  }

  camera.prepare();

  ui::ui.update();
  modules.update();
  lua.update();

  uiMicros += Time::uclock() - beginMicros;

  loader.hasTime = true;

  SDL_SemPost( auxSemaphore );
  SDL_SemWait( mainSemaphore );

  /*
   * PHASE 2
   */

  beginMicros = Time::uclock();

  context.updateLoad();

  loader.update();

  loaderMicros += Time::uclock() - beginMicros;

  SDL_SemPost( auxSemaphore );
  SDL_SemWait( mainSemaphore );

  /*
   * PHASE 3
   */

  camera.update();

  return true;
}

void GameStage::present( bool full )
{
  uint beginMicros = Time::uclock();
  uint currentMicros;

  sound.play();

  currentMicros = Time::uclock();
  soundMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  if( full ) {
    render.draw( Render::DRAW_ORBIS_BIT | Render::DRAW_UI_BIT );
    render.swap();

    currentMicros = Time::uclock();
    renderMicros += currentMicros - beginMicros;
    beginMicros = currentMicros;
  }

  sound.sync();

  currentMicros = Time::uclock();
  soundMicros += currentMicros - beginMicros;
}

void GameStage::wait( uint micros )
{
  sleepMicros += micros;

  Time::usleep( micros );
}

void GameStage::load()
{
  log.print( "[" );
  log.printTime();
  log.printEnd( "] Loading GameStage {" );
  log.indent();

  loadingMicros = Time::uclock();

  ui::mouse.doShow = false;
  ui::ui.loadingScreen->status.setText( "%s", OZ_GETTEXT( "Loading ..." ) );
  ui::ui.loadingScreen->show( true );

  render.draw( Render::DRAW_UI_BIT );
  render.draw( Render::DRAW_UI_BIT );
  render.swap();

  timer.reset();

  sleepMicros   = 0;
  uiMicros      = 0;
  loaderMicros  = 0;
  soundMicros   = 0;
  renderMicros  = 0;
  matrixMicros  = 0;
  nirvanaMicros = 0;

  log.print( "Starting auxilary thread ..." );

  isAuxAlive    = true;
  mainSemaphore = SDL_CreateSemaphore( 0 );
  auxSemaphore  = SDL_CreateSemaphore( 0 );
  auxThread     = SDL_CreateThread( auxMain, null );

  log.printEnd( " OK" );

  network.connect();

  matrix.load();
  nirvana.load();

  lua.init();
  modules.registerLua();

  questList.load();
  render.load();
  context.load();

  camera.reset();
  camera.setState( Camera::STRATEGIC );

  modules.load();

  if( stateFile.isEmpty() ) {
    log.println( "Initialising new world" );

    log.println( "Loading Client {" );
    log.indent();

    lua.create( mission );

    log.unindent();
    log.println( "}" );
  }
  else if( !read( stateFile ) ) {
    throw Exception( "Reading saved state '%s' failed", stateFile.cstr() );
  }

  nirvana.sync();
  synapse.update();

  ui::mouse.buttons = 0;
  ui::mouse.currButtons = 0;

  camera.prepare();
  camera.update();

  ui::ui.showLoadingScreen( true );

  render.draw( Render::DRAW_ORBIS_BIT | Render::DRAW_UI_BIT );
  loader.syncUpdate();

  render.draw( Render::DRAW_ORBIS_BIT | Render::DRAW_UI_BIT );
  loader.syncUpdate();

  sound.play();
  render.swap();
  sound.sync();

  loader.load();

  ui::ui.prepare();
  ui::ui.showLoadingScreen( false );

  loadingMicros = Time::uclock() - loadingMicros;

  isLoaded = true;

  log.unindent();
  log.println( "}" );
}

void GameStage::unload()
{
  log.print( "[" );
  log.printTime();
  log.printEnd( "] Unloading GameStage {" );
  log.indent();

  ui::mouse.doShow = false;
  ui::ui.loadingScreen->status.setText( "%s", OZ_GETTEXT( "Shutting down ..." ) );
  ui::ui.showLoadingScreen( true );

  loader.unload();

  render.draw( Render::DRAW_UI_BIT );
  render.draw( Render::DRAW_UI_BIT );
  render.swap();

  float sleepTime             = float( sleepMicros )                    * 1.0e-6f;
  float uiTime                = float( uiMicros )                       * 1.0e-6f;
  float loaderTime            = float( loaderMicros )                   * 1.0e-6f;
  float presentTime           = float( soundMicros + renderMicros )     * 1.0e-6f;
  float soundTime             = float( soundMicros )                    * 1.0e-6f;
  float renderTime            = float( renderMicros )                   * 1.0e-6f;
  float renderPrepareTime     = float( render.prepareMicros )           * 1.0e-6f;
  float renderSetupTime       = float( render.setupMicros )             * 1.0e-6f;
  float renderCaelumTime      = float( render.caelumMicros )            * 1.0e-6f;
  float renderTerraTime       = float( render.terraMicros )             * 1.0e-6f;
  float renderStructsTime     = float( render.structsMicros )           * 1.0e-6f;
  float renderObjectsTime     = float( render.objectsMicros )           * 1.0e-6f;
  float renderFragsTime       = float( render.fragsMicros )             * 1.0e-6f;
  float renderMiscTime        = float( render.miscMicros )              * 1.0e-6f;
  float renderPostprocessTime = float( render.postprocessMicros )       * 1.0e-6f;
  float renderUITime          = float( render.uiMicros )                * 1.0e-6f;
  float renderSwapTime        = float( render.swapMicros )              * 1.0e-6f;
  float matrixTime            = float( matrixMicros )                   * 1.0e-6f;
  float nirvanaTime           = float( nirvanaMicros )                  * 1.0e-6f;
  float loadingTime           = float( loadingMicros )                  * 1.0e-6f;
  float runTime               = float( timer.runMicros )                * 1.0e-6f;
  float gameTime              = float( timer.micros )                   * 1.0e-6f;
  float droppedTime           = float( timer.runMicros - timer.micros ) * 1.0e-6f;
  int   nFrameDrops           = int( timer.ticks - timer.nFrames );
  float frameDropRate         = float( timer.ticks - timer.nFrames ) / float( timer.ticks );

  if( isLoaded ) {
    write( AUTOSAVE_FILE );
  }

  modules.unload();

  camera.reset();

  context.unload();
  render.unload();
  questList.unload();

  lua.free();

  nirvana.unload();
  matrix.unload();

  network.disconnect();

  log.print( "Stopping auxilary thread ..." );

  isAuxAlive = false;

  SDL_SemPost( auxSemaphore );
  SDL_WaitThread( auxThread, null );

  SDL_DestroySemaphore( auxSemaphore );
  SDL_DestroySemaphore( mainSemaphore );

  mainSemaphore = null;
  auxSemaphore  = null;
  auxThread     = null;

  log.printEnd( " OK" );

  ui::ui.showLoadingScreen( false );

  log.println( "Time statistics {" );
  log.indent();
  log.println( "loading time          %8.2f s",         loadingTime                         );
  log.println( "run time              %8.2f s",         runTime                             );
  log.println( "game time             %8.2f s",         gameTime                            );
  log.println( "dropped time          %8.2f s",         droppedTime                         );
  log.println( "optimal tick/frame rate %6.2f Hz",      1.0f / Timer::TICK_TIME             );
  log.println( "tick rate in run time   %6.2f Hz",      float( timer.ticks ) / runTime      );
  log.println( "frame rate in run time  %6.2f Hz",      float( timer.nFrames ) / runTime    );
  log.println( "frame drop rate         %6.2f %%",      frameDropRate * 100.0f              );
  log.println( "frame drops             %6d",           nFrameDrops                         );
  log.println( "Run time usage {" );
  log.indent();
  log.println( "%6.2f %%  [M:0] sleep",            sleepTime             / runTime * 100.0f );
  log.println( "%6.2f %%  [M:1] input & ui",       uiTime                / runTime * 100.0f );
  log.println( "%6.2f %%  [M:2] loader",           loaderTime            / runTime * 100.0f );
  log.println( "%6.2f %%  [M:3] present",          presentTime           / runTime * 100.0f );
  log.println( "%6.2f %%  [M:3] + sound",          soundTime             / runTime * 100.0f );
  log.println( "%6.2f %%  [M:3] + render",         renderTime            / runTime * 100.0f );
  log.println( "%6.2f %%  [M:3]   + prepare",      renderPrepareTime     / runTime * 100.0f );
  log.println( "%6.2f %%  [M:3]   + shader setup", renderSetupTime       / runTime * 100.0f );
  log.println( "%6.2f %%  [M:3]   + caelum",       renderCaelumTime      / runTime * 100.0f );
  log.println( "%6.2f %%  [M:3]   + terra",        renderTerraTime       / runTime * 100.0f );
  log.println( "%6.2f %%  [M:3]   + structs",      renderStructsTime     / runTime * 100.0f );
  log.println( "%6.2f %%  [M:3]   + objects",      renderObjectsTime     / runTime * 100.0f );
  log.println( "%6.2f %%  [M:3]   + frags",        renderFragsTime       / runTime * 100.0f );
  log.println( "%6.2f %%  [M:3]   + misc",         renderMiscTime        / runTime * 100.0f );
  log.println( "%6.2f %%  [M:3]   + postprocess",  renderPostprocessTime / runTime * 100.0f );
  log.println( "%6.2f %%  [M:3]   + ui",           renderUITime          / runTime * 100.0f );
  log.println( "%6.2f %%  [M:3]   + swap",         renderSwapTime        / runTime * 100.0f );
  log.println( "%6.2f %%  [A:2] matrix",           matrixTime            / runTime * 100.0f );
  log.println( "%6.2f %%  [A:3] nirvana",          nirvanaTime           / runTime * 100.0f );
  log.unindent();
  log.println( "}" );
  log.unindent();
  log.println( "}" );

  isLoaded = false;

  log.unindent();
  log.println( "}" );
}

void GameStage::init()
{
  isLoaded = false;

  log.println( "Initialising GameStage {" );
  log.indent();

  AUTOSAVE_FILE = String::str( "%s/saves/autosave.ozState", config.get( "dir.config", "" ) );
  QUICKSAVE_FILE = String::str( "%s/saves/quicksave.ozState", config.get( "dir.config", "" ) );

  matrix.init();
  nirvana.init();
  loader.init();
  profile.init();
  modules.init();

  log.unindent();
  log.println( "}" );
}

void GameStage::free()
{
  log.println( "Freeing GameStage {" );
  log.indent();

  modules.free();
  profile.free();
  loader.free();
  nirvana.free();
  matrix.free();

  stateFile      = "";
  mission        = "";
  AUTOSAVE_FILE  = "";
  QUICKSAVE_FILE = "";

  log.unindent();
  log.println( "}" );
}

}
}
