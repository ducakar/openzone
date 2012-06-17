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
#include "client/NaClMainCall.hh"

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

void GameStage::auxMain( void* )
{
  try {
    gameStage.auxRun();
  }
  catch( const std::exception& e ) {
    System::error( e );
  }
}

bool GameStage::read( const char* path )
{
  Log::print( "Loading state from '%s' ...", path );

  File file( path );
  if( !file.map() ) {
    Log::printEnd( " Failed" );
    return false;
  }

  Log::printEnd( " OK" );

  InputStream istream = file.inputStream();

  matrix.read( &istream );
  nirvana.read( &istream );

  Log::println( "Reading Client {" );
  Log::indent();

  questList.read( &istream );
  camera.read( &istream );
  modules.read( &istream );

  lua.read( &istream );

  Log::unindent();
  Log::println( "}" );

  file.unmap();

  return true;
}

void GameStage::write( const char* path ) const
{
  BufferStream ostream;

  matrix.write( &ostream );
  nirvana.write( &ostream );

  Log::print( "Writing Client ..." );

  questList.write( &ostream );
  camera.write( &ostream );
  modules.write( &ostream );

  lua.write( &ostream );

  Log::printEnd( " OK" );

  Log::print( "Saving state to %s ...", path );

  if( !File( path ).write( ostream.begin(), ostream.length() ) ) {
    Log::printEnd( " Failed" );
  }
  else {
    Log::printEnd( " OK" );
  }
}

void GameStage::reload()
{
  Log::print( "[" );
  Log::printTime( Time::local() );
  Log::printEnd( "] Reloading GameStage {" );
  Log::indent();

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
    Log::println( "Initialising new world" );

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

  Log::unindent();
  Log::println( "}" );
}

void GameStage::auxRun()
{
  uint beginMicros;

  auxSemaphore.wait();

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

    mainSemaphore.post();
    auxSemaphore.wait();

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
    mainSemaphore.post();
    auxSemaphore.wait();

    /*
     * PHASE 1
     */

  }
}

bool GameStage::update()
{
  uint beginMicros;

  mainSemaphore.wait();

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

  auxSemaphore.post();

  /*
   * PHASE 2
   */

  beginMicros = Time::uclock();

  context.updateLoad();

  loader.update();

  loaderMicros += Time::uclock() - beginMicros;

  auxSemaphore.post();
  mainSemaphore.wait();

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
  Log::print( "[" );
  Log::printTime( Time::local() );
  Log::printEnd( "] Loading GameStage {" );
  Log::indent();

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

  Log::print( "Starting auxilary thread ..." );

  isAuxAlive = true;
  mainSemaphore.init( 1 );
  auxSemaphore.init( 0 );
  auxThread.start( auxMain );

  Log::printEnd( " OK" );

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
    Log::println( "Initialising new world" );

    Log::println( "Loading Client {" );
    Log::indent();

    lua.create( mission );

    Log::unindent();
    Log::println( "}" );
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

  Log::unindent();
  Log::println( "}" );
}

void GameStage::unload()
{
  Log::print( "[" );
  Log::printTime( Time::local() );
  Log::printEnd( "] Unloading GameStage {" );
  Log::indent();

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

  Log::print( "Stopping auxilary thread ..." );

  isAuxAlive = false;

  auxSemaphore.post();
  auxThread.join();

  auxSemaphore.destroy();
  mainSemaphore.destroy();

  Log::printEnd( " OK" );

  ui::ui.showLoadingScreen( false );

  Log::println( "Time statistics {" );
  Log::indent();
  Log::println( "loading time          %8.2f s",    loadingTime                              );
  Log::println( "run time              %8.2f s",    runTime                                  );
  Log::println( "game time             %8.2f s",    gameTime                                 );
  Log::println( "dropped time          %8.2f s",    droppedTime                              );
  Log::println( "optimal tick/frame rate %6.2f Hz", 1.0f / Timer::TICK_TIME                  );
  Log::println( "tick rate in run time   %6.2f Hz", float( timer.ticks ) / runTime           );
  Log::println( "frame rate in run time  %6.2f Hz", float( timer.nFrames ) / runTime         );
  Log::println( "frame drop rate         %6.2f %%", frameDropRate * 100.0f                   );
  Log::println( "frame drops             %6d",      nFrameDrops                              );
  Log::println( "Run time usage {" );
  Log::indent();
  Log::println( "%6.2f %%  [M:0] sleep",            sleepTime             / runTime * 100.0f );
  Log::println( "%6.2f %%  [M:1] input & ui",       uiTime                / runTime * 100.0f );
  Log::println( "%6.2f %%  [M:2] loader",           loaderTime            / runTime * 100.0f );
  Log::println( "%6.2f %%  [M:3] present",          presentTime           / runTime * 100.0f );
  Log::println( "%6.2f %%  [M:3] + sound",          soundTime             / runTime * 100.0f );
  Log::println( "%6.2f %%  [M:3] + render",         renderTime            / runTime * 100.0f );
  Log::println( "%6.2f %%  [M:3]   + prepare",      renderPrepareTime     / runTime * 100.0f );
  Log::println( "%6.2f %%  [M:3]   + shader setup", renderSetupTime       / runTime * 100.0f );
  Log::println( "%6.2f %%  [M:3]   + caelum",       renderCaelumTime      / runTime * 100.0f );
  Log::println( "%6.2f %%  [M:3]   + terra",        renderTerraTime       / runTime * 100.0f );
  Log::println( "%6.2f %%  [M:3]   + structs",      renderStructsTime     / runTime * 100.0f );
  Log::println( "%6.2f %%  [M:3]   + objects",      renderObjectsTime     / runTime * 100.0f );
  Log::println( "%6.2f %%  [M:3]   + frags",        renderFragsTime       / runTime * 100.0f );
  Log::println( "%6.2f %%  [M:3]   + misc",         renderMiscTime        / runTime * 100.0f );
  Log::println( "%6.2f %%  [M:3]   + postprocess",  renderPostprocessTime / runTime * 100.0f );
  Log::println( "%6.2f %%  [M:3]   + ui",           renderUITime          / runTime * 100.0f );
  Log::println( "%6.2f %%  [M:3]   + swap",         renderSwapTime        / runTime * 100.0f );
  Log::println( "%6.2f %%  [A:2] matrix",           matrixTime            / runTime * 100.0f );
  Log::println( "%6.2f %%  [A:3] nirvana",          nirvanaTime           / runTime * 100.0f );
  Log::unindent();
  Log::println( "}" );
  Log::unindent();
  Log::println( "}" );

  isLoaded = false;

  Log::unindent();
  Log::println( "}" );
}

void GameStage::init()
{
  isLoaded = false;

  Log::println( "Initialising GameStage {" );
  Log::indent();

  AUTOSAVE_FILE = String::str( "%s/saves/autosave.ozState", config.get( "dir.config", "" ) );
  QUICKSAVE_FILE = String::str( "%s/saves/quicksave.ozState", config.get( "dir.config", "" ) );

  matrix.init();
  nirvana.init();
  loader.init();
  profile.init();
  modules.init();

  Log::unindent();
  Log::println( "}" );
}

void GameStage::free()
{
  Log::println( "Freeing GameStage {" );
  Log::indent();

  modules.free();
  profile.free();
  loader.free();
  nirvana.free();
  matrix.free();

  stateFile      = "";
  mission        = "";
  AUTOSAVE_FILE  = "";
  QUICKSAVE_FILE = "";

  Log::unindent();
  Log::println( "}" );
}

}
}
