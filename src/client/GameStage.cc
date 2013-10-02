/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#include <client/GameStage.hh>

#include <matrix/Synapse.hh>
#include <matrix/Matrix.hh>
#include <nirvana/Nirvana.hh>
#include <modules/Modules.hh>
#include <client/Context.hh>
#include <client/Loader.hh>
#include <client/Render.hh>
#include <client/Sound.hh>
#include <client/Network.hh>
#include <client/Camera.hh>
#include <client/LuaClient.hh>
#include <client/Profile.hh>
#include <client/MenuStage.hh>
#include <client/Input.hh>
#include <client/ui/QuestFrame.hh>
#include <client/ui/LoadingArea.hh>
#include <client/ui/UI.hh>

namespace oz
{
namespace client
{

using modules::modules;

void GameStage::read()
{
  Log::print( "Loading state from '%s' ...", stateFile.path().cstr() );

  Buffer buffer = stateFile.read();
  if( buffer.isEmpty() ) {
    OZ_ERROR( "Reading saved state '%s' failed", stateFile.path().cstr() );
  }

  buffer = buffer.inflate();
  if( buffer.isEmpty() ) {
    OZ_ERROR( "Decompressing saved state '%s' failed", stateFile.path().cstr() );
  }

  Log::printEnd( " OK" );

  InputStream istream = buffer.inputStream( Endian::LITTLE );

  matrix.read( &istream );
  nirvana.read( &istream );

  Log::println( "Reading Client {" );
  Log::indent();

  camera.read( &istream );
  modules.read( &istream );

  luaClient.read( &istream );

  Log::unindent();
  Log::println( "}" );
}

void GameStage::write() const
{
  OutputStream ostream( 0, Endian::LITTLE );

  matrix.write( &ostream );
  nirvana.write( &ostream );

  camera.write( &ostream );
  modules.write( &ostream );

  luaClient.write( &ostream );

  Log::print( "Saving state to %s ...", stateFile.path().cstr() );

  Buffer buffer( ostream.begin(), ostream.tell() );
  buffer = buffer.deflate( 1 );

  if( !stateFile.write( buffer ) ) {
    Log::printEnd( " Failed" );
    System::bell();
  }
  else {
    Log::printEnd( " OK" );
  }
}

void GameStage::auxMain( void* )
{
  gameStage.auxRun();
}

void GameStage::auxRun()
{
  uint beginMicros;

  auxSemaphore.wait();

  while( isAuxAlive ) {
    /*
     * PHASE 2
     *
     * World is being updated, other threads should not access world structures here.
     */

    beginMicros = Time::uclock();

    network.update();

    // update world
    matrix.update();

    matrixMicros += Time::uclock() - beginMicros;

    mainSemaphore.post();
    auxSemaphore.wait();

    /*
     * PHASE 3
     *
     * Process AI, main thread renders world and plays sound.
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
     *
     * Nothing, main thread may manipulate world here.
     */
  }
}

bool GameStage::update()
{
  uint beginMicros;

  mainSemaphore.wait();

  /*
   * PHASE 1
   *
   * UI and modules update, world may be updated from the main thread during this phase.
   */

  beginMicros = Time::uclock();

  if( input.keys[Input::KEY_QUIT] ) {
    Stage::nextStage = &menuStage;
  }

  if( input.keys[Input::KEY_QUICKSAVE] && !input.oldKeys[Input::KEY_QUICKSAVE] ) {
    stateFile = quicksaveFile;
    write();
    stateFile = "";
  }
  if( input.keys[Input::KEY_QUICKLOAD] && !input.oldKeys[Input::KEY_QUICKLOAD] ) {
    quicksaveFile.stat();

    if( quicksaveFile.type() == File::REGULAR ) {
      stateFile = quicksaveFile;
      Stage::nextStage = this;
    }
  }
  if( input.keys[Input::KEY_AUTOLOAD] && !input.oldKeys[Input::KEY_AUTOLOAD] ) {
    autosaveFile.stat();

    if( autosaveFile.type() == File::REGULAR ) {
      stateFile = autosaveFile;
      Stage::nextStage = this;
    }
  }

  camera.prepare();

  modules.update();
  luaClient.update();

  uiMicros += Time::uclock() - beginMicros;

  auxSemaphore.post();

  /*
   * PHASE 2
   *
   * World is being updated in the auxiliary thread, any access of world structures might crash the
   * game.
   */

  beginMicros = Time::uclock();

  context.updateLoad();
  loader.update();

  loaderMicros += Time::uclock() - beginMicros;

  auxSemaphore.post();
  mainSemaphore.wait();

  /*
   * PHASE 3
   *
   * AI is processed in auxiliary thread here, world is rendered later in this phase in present().
   */

  camera.update();

  return true;
}

void GameStage::present( bool isFull )
{
  uint beginMicros = Time::uclock();
  uint currentMicros;

  sound.play();

  currentMicros = Time::uclock();
  soundMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

  render.update( isFull ? Render::DRAW_ORBIS_BIT | Render::DRAW_UI_BIT : 0 );

  currentMicros = Time::uclock();
  renderMicros += currentMicros - beginMicros;
  beginMicros = currentMicros;

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
  Log::println( "[%s] Loading GameStage {", Time::local().toString().cstr() );
  Log::indent();

  loadingMicros = Time::uclock();

  ui::mouse.doShow = false;
  ui::ui.loadingScreen->status.setText( "%s", OZ_GETTEXT( "Loading ..." ) );
  ui::ui.loadingScreen->show( true );

  render.update( Render::DRAW_UI_BIT );

  timer.reset();

  sleepMicros   = 0;
  uiMicros      = 0;
  loaderMicros  = 0;
  soundMicros   = 0;
  renderMicros  = 0;
  matrixMicros  = 0;
  nirvanaMicros = 0;

  network.connect();

  matrix.load();
  nirvana.load();

  luaClient.init();
  modules.registerLua();

  render.load();
  context.load();

  camera.reset();
  camera.setState( Camera::STRATEGIC );

  modules.load();

  if( stateFile.type() == File::REGULAR ) {
    read();
  }
  else {
    Log::println( "Initialising new world" );

    Log::println( "Loading Client {" );
    Log::indent();

    luaClient.create( mission );

    Log::unindent();
    Log::println( "}" );
  }

  stateFile = "";

  nirvana.sync();
  synapse.update();

  input.buttons = 0;
  input.currButtons = 0;

  camera.prepare();
  camera.update();

  ui::ui.questFrame->enable( true );

  startTicks = timer.ticks;

  ui::ui.showLoadingScreen( true );

  render.update( Render::DRAW_ORBIS_BIT | Render::DRAW_UI_BIT );
  loader.syncUpdate();
  sound.play();
  sound.sync();

  loader.load();

  isAuxAlive = true;
  mainSemaphore.init( 1 );
  auxSemaphore.init( 0 );
  auxThread.start( "aux", Thread::JOINABLE, auxMain );

  ui::ui.showLoadingScreen( false );

  loadingMicros = Time::uclock() - loadingMicros;

  Log::unindent();
  Log::println( "}" );
}

void GameStage::unload()
{
  Log::println( "[%s] Unloading GameStage {", Time::local().toString().cstr() );
  Log::indent();

  ui::mouse.doShow = false;
  ui::ui.loadingScreen->status.setText( "%s", OZ_GETTEXT( "Shutting down ..." ) );
  ui::ui.showLoadingScreen( true );

  loader.unload();

  render.update( Render::DRAW_UI_BIT );

  isAuxAlive = false;

  auxSemaphore.post();
  auxThread.join();

  float ticks                 = float( timer.ticks - startTicks );
  float sleepTime             = float( sleepMicros )                    * 1.0e-6f;
  float uiTime                = float( uiMicros )                       * 1.0e-6f;
  float loaderTime            = float( loaderMicros )                   * 1.0e-6f;
  float presentTime           = float( soundMicros + renderMicros )     * 1.0e-6f;
  float soundTime             = float( soundMicros )                    * 1.0e-6f;
  float renderTime            = float( renderMicros )                   * 1.0e-6f;
  float renderPrepareTime     = float( render.prepareMicros )           * 1.0e-6f;
  float renderCaelumTime      = float( render.caelumMicros )            * 1.0e-6f;
  float renderTerraTime       = float( render.terraMicros )             * 1.0e-6f;
  float renderMeshesTime      = float( render.meshesMicros )            * 1.0e-6f;
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

  if( stateFile.isEmpty() ) {
    stateFile = autosaveFile;
    write();
    stateFile = "";
  }

  modules.unload();
  profile.save();

  ui::ui.questFrame->enable( false );

  camera.reset();

  context.unload();
  render.unload();

  luaClient.destroy();

  nirvana.unload();
  matrix.unload();

  network.disconnect();

  auxSemaphore.destroy();
  mainSemaphore.destroy();

  ui::ui.showLoadingScreen( false );

  Log::println( "Time statistics {" );
  Log::indent();
  Log::println( "loading time          %8.2f s",    loadingTime                              );
  Log::println( "run time              %8.2f s",    runTime                                  );
  Log::println( "game time             %8.2f s",    gameTime                                 );
  Log::println( "dropped time          %8.2f s",    droppedTime                              );
  Log::println( "optimal tick/frame rate %6.2f Hz", 1.0f / Timer::TICK_TIME                  );
  Log::println( "tick rate in run time   %6.2f Hz", ticks / runTime                          );
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
  Log::println( "%6.2f %%  [M:3]   + caelum",       renderCaelumTime      / runTime * 100.0f );
  Log::println( "%6.2f %%  [M:3]   + terra",        renderTerraTime       / runTime * 100.0f );
  Log::println( "%6.2f %%  [M:3]   + meshes",       renderMeshesTime      / runTime * 100.0f );
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

  Log::unindent();
  Log::println( "}" );
}

void GameStage::init()
{
  Log::println( "Initialising GameStage {" );
  Log::indent();

  autosaveFile  = "@state/autosave.ozState";
  quicksaveFile = "@state/quicksave.ozState";

  matrix.init();
  nirvana.init();
  loader.init();
  profile.init();
  modules.init();

  Log::unindent();
  Log::println( "}" );
}

void GameStage::destroy()
{
  Log::println( "Destroying GameStage {" );
  Log::indent();

  modules.destroy();
  profile.destroy();
  loader.destroy();
  nirvana.destroy();
  matrix.destroy();

  stateFile     = "";
  mission       = "";
  autosaveFile  = "";
  quicksaveFile = "";

  Log::unindent();
  Log::println( "}" );
}

GameStage gameStage;

}
}
