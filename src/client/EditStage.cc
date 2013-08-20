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
 * @file client/EditStage.cc
 */

#include <client/EditStage.hh>

#include <matrix/Synapse.hh>
#include <matrix/Matrix.hh>
#include <nirvana/Nirvana.hh>
#include <client/Context.hh>
#include <client/Loader.hh>
#include <client/Render.hh>
#include <client/Sound.hh>
#include <client/Camera.hh>
#include <client/Profile.hh>
#include <client/MenuStage.hh>
#include <client/Input.hh>
#include <client/ui/LoadingArea.hh>
#include <client/ui/StrategicArea.hh>
#include <client/ui/UI.hh>

namespace oz
{
namespace client
{

void EditStage::read()
{
  Log::print( "Loading layout from '%s' ...", layoutFile.path().cstr() );

  JSON json;
  if( !json.load( layoutFile ) ) {
    OZ_ERROR( "Reading saved layout '%s' failed", layoutFile.path().cstr() );
  }
  layoutFile.unmap();

  Log::printEnd( " OK" );

  matrix.read( json );

  json.clear( true );
}

void EditStage::write() const
{
  JSON json = matrix.write();

  Log::print( "Saving layout to %s ...", layoutFile.path().cstr() );

  if( !json.save( layoutFile ) ) {
    Log::printEnd( " Failed" );
  }
  else {
    Log::printEnd( " OK" );
  }
}

void EditStage::auxMain( void* )
{
  editStage.auxRun();
}

void EditStage::auxRun()
{
  auxSemaphore.wait();

  while( isAuxAlive ) {
    /*
     * PHASE 2
     *
     * World is being updated, other threads should not access world structures here.
     */

    // update world
    matrix.update();

    mainSemaphore.post();
    auxSemaphore.wait();

    /*
     * PHASE 3
     *
     * Process AI, main thread renders world and plays sound.
     */

    // now synapse lists are not needed any more
    synapse.update();

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

bool EditStage::update()
{
  mainSemaphore.wait();

  /*
   * PHASE 1
   *
   * UI and modules update, world may be updated from the main thread during this phase.
   */

  if( input.keys[Input::KEY_SAVE_LAYOUT] && !input.oldKeys[Input::KEY_SAVE_LAYOUT] ) {
    layoutFile = File( config["dir.config"].asString() + "/layouts/default.json" );
    write();
    layoutFile = "";
  }
  if( input.keys[Input::KEY_LOAD_LAYOUT] && !input.oldKeys[Input::KEY_LOAD_LAYOUT] ) {
    layoutFile.stat();

    if( layoutFile.type() == File::REGULAR ) {
      Stage::nextStage = this;
    }
  }

  if( input.keys[Input::KEY_DELETE] && !input.oldKeys[Input::KEY_DELETE] ) {
    Struct* str = orbis.str( ui::ui.strategicArea->taggedStr );

    if( str != nullptr ) {
      synapse.remove( str );
    }

    foreach( i, ui::ui.strategicArea->taggedObjs.citer() ) {
      Object* obj = orbis.obj( *i );

      if( obj != nullptr ) {
        synapse.remove( obj );
      }
    }
  }

  camera.prepare();

  auxSemaphore.post();

  /*
   * PHASE 2
   *
   * World is being updated in the auxiliary thread, any access of world structures might crash the
   * game.
   */

  context.updateLoad();
  loader.update();

  auxSemaphore.post();
  mainSemaphore.wait();

  /*
   * PHASE 3
   *
   * AI is processed in auxiliary thread here, world is rendered later in this phase in present().
   */

  camera.update();

  return !input.keys[Input::KEY_QUIT];
}

void EditStage::present( bool isFull )
{
  sound.play();

  if( isFull ) {
    render.draw( Render::DRAW_ORBIS_BIT | Render::DRAW_UI_BIT );
    render.swap();
  }

  render.update();
  sound.sync();
}

void EditStage::wait( uint micros )
{
  Time::usleep( micros );
}

void EditStage::load()
{
  Log::println( "[%s] Loading EditStage {", Time::local().toString().cstr() );
  Log::indent();

  ui::mouse.doShow = false;
  ui::ui.loadingScreen->status.setText( "%s", OZ_GETTEXT( "Loading ..." ) );
  ui::ui.loadingScreen->show( true );

  render.draw( Render::DRAW_UI_BIT );
  render.draw( Render::DRAW_UI_BIT );
  render.swap();

  timer.reset();

  matrix.load();
  nirvana.load();

  render.load();
  context.load();

  camera.reset();
  camera.setState( Camera::STRATEGIC );
  camera.strategic.hasBuildFrame = true;

  editFrame = new ui::EditFrame();
  ui::ui.root->add( editFrame, ui::Area::CENTRE, 8 );

  if( layoutFile.type() == File::REGULAR ) {
    read();
  }

  synapse.update();

  input.buttons = 0;
  input.currButtons = 0;

  camera.prepare();
  camera.update();

  ui::ui.showLoadingScreen( true );

  render.draw( Render::DRAW_ORBIS_BIT | Render::DRAW_UI_BIT );
  loader.syncUpdate();
  sound.play();
  render.swap();
  sound.sync();

  loader.load();

  isAuxAlive = true;
  mainSemaphore.init( 1 );
  auxSemaphore.init( 0 );
  auxThread.start( "aux", Thread::JOINABLE, auxMain );

  ui::ui.showLoadingScreen( false );

  Log::unindent();
  Log::println( "}" );
}

void EditStage::unload()
{
  Log::println( "[%s] Unloading EditStage {", Time::local().toString().cstr() );
  Log::indent();

  ui::mouse.doShow = false;
  ui::ui.loadingScreen->status.setText( "%s", OZ_GETTEXT( "Shutting down ..." ) );
  ui::ui.showLoadingScreen( true );

  loader.unload();

  render.draw( Render::DRAW_UI_BIT );
  render.draw( Render::DRAW_UI_BIT );
  render.swap();

  isAuxAlive = false;

  auxSemaphore.post();
  auxThread.join();

  if( layoutFile.type() == File::REGULAR ) {
    write();
  }

  ui::ui.root->remove( editFrame );
  editFrame = nullptr;

  camera.reset();

  context.unload();
  render.unload();

  nirvana.unload();
  matrix.unload();

  auxSemaphore.destroy();
  mainSemaphore.destroy();

  ui::ui.showLoadingScreen( false );

  Log::unindent();
  Log::println( "}" );
}

void EditStage::init()
{
  Log::println( "Initialising EditStage {" );
  Log::indent();

  matrix.init();
  nirvana.init();
  loader.init();
  profile.init();

  Log::unindent();
  Log::println( "}" );
}

void EditStage::destroy()
{
  Log::println( "Destroying EditStage {" );
  Log::indent();

  profile.destroy();
  loader.destroy();
  nirvana.destroy();
  matrix.destroy();

  layoutFile = "";

  Log::unindent();
  Log::println( "}" );
}

EditStage editStage;

}
}