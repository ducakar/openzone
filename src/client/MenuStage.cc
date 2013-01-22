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
 * @file client/MenuStage.cc
 */

#include <stable.hh>
#include <client/MenuStage.hh>

#include <client/Camera.hh>
#include <client/Render.hh>
#include <client/Sound.hh>
#include <client/GameStage.hh>
#include <client/Input.hh>
#include <client/NaClPlatform.hh>

namespace oz
{
namespace client
{

bool MenuStage::update()
{
  camera.prepare();
  camera.update();

  return !doExit;
}

void MenuStage::present( bool isFull )
{
  sound.play();

  if( isFull ) {
    render.draw( Render::DRAW_UI_BIT );
    render.swap();
  }

  sound.sync();
}

void MenuStage::wait( uint micros )
{
  Time::usleep( micros );
}

void MenuStage::load()
{
  File autosaveFile( File::NATIVE, GameStage::AUTOSAVE_FILE );
  File quicksaveFile( File::NATIVE, GameStage::QUICKSAVE_FILE );

  if( autosaveFile.type() == File::REGULAR ) {
    showAutosaved = true;
  }
  if( quicksaveFile.type() == File::REGULAR ) {
    showQuicksaved = true;
  }

  OZ_MAIN_CALL( this, {
    input.buttons     = 0;
    input.currButtons = 0;

    _this->mainMenu = new ui::MainMenu();
    ui::ui.root->add( _this->mainMenu, 0, 0 );

    ui::ui.showLoadingScreen( false );
    ui::mouse.doShow = true;
  } )

  render.draw( Render::DRAW_UI_BIT );
  render.draw( Render::DRAW_UI_BIT );
  render.swap();
}

void MenuStage::unload()
{
  OZ_MAIN_CALL( this, {
    ui::mouse.doShow = false;
    ui::ui.root->remove( _this->mainMenu );
  } )
}

void MenuStage::init()
{
  doExit = false;
}

void MenuStage::destroy()
{}

MenuStage menuStage;

}
}
