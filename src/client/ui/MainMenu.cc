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
 * @file client/ui/MainMenu.cc
 */

#include <stable.hh>
#include <client/ui/MainMenu.hh>

#include <BuildInfo.hh>

#include <client/Camera.hh>
#include <client/Shape.hh>
#include <client/GameStage.hh>
#include <client/MenuStage.hh>
#include <client/OpenGL.hh>

#include <client/ui/Style.hh>
#include <client/ui/MissionMenu.hh>
#include <client/ui/SettingsFrame.hh>
#include <client/ui/CreditsMenu.hh>
#include <client/ui/UI.hh>

namespace oz
{
namespace client
{
namespace ui
{

static void loadAutosaved( Button* )
{
  Stage::nextStage = &gameStage;
  gameStage.stateFile = GameStage::AUTOSAVE_FILE;
  gameStage.mission = "";
}

static void loadQuicksaved( Button* )
{
  Stage::nextStage = &gameStage;
  gameStage.stateFile = GameStage::QUICKSAVE_FILE;
  gameStage.mission = "";
}

static void openMissions( Button* sender )
{
  OZ_MAIN_CALL( sender, {
    Button*   _sender  = static_cast<Button*>( _this );
    MainMenu* mainMenu = static_cast<MainMenu*>( _sender->parent );

    mainMenu->add( new MissionMenu(), 0, 0 );
  } )
}

static void openSettings( Button* sender )
{
  OZ_MAIN_CALL( sender, {
    Button*   _sender  = static_cast<Button*>( _this );
    MainMenu* mainMenu = static_cast<MainMenu*>( _sender->parent );

    mainMenu->add( new SettingsFrame(), Area::CENTRE, Area::CENTRE );
  } )
}

static void openCredits( Button* sender )
{
  OZ_MAIN_CALL( sender, {
    Button*   _sender  = static_cast<Button*>( _this );
    MainMenu* mainMenu = static_cast<MainMenu*>( _sender->parent );

    mainMenu->add( new CreditsMenu(), 0, 0 );
  } )
}

static void quit( Button* )
{
  menuStage.doExit = true;
}

void MainMenu::onReposition()
{
  width  = camera.width;
  height = camera.height;

  copyright.resize( width - 280 );
  copyright.set( "OpenZone © 2012 Davorin Učakar. %s",
                 OZ_GETTEXT( "Licensed under GNU GPL 3.0. Game data archives and libraries"
                             " distributed with OpenZone are work of various authors and use"
                             " separate licences. For more details see doc/README.html and files"
                             " named README.txt and COPYING.txt inside game data archives." ) );

  foreach( child, children.iter() ) {
    child->reposition();
  }
}

void MainMenu::onUpdate()
{
  File autosaveFile( GameStage::AUTOSAVE_FILE );
  File quicksaveFile( GameStage::QUICKSAVE_FILE );

  if( autosaveFile.stat() ) {
    OZ_MAIN_CALL( this, {
      Button* continueButton  = new Button( OZ_GETTEXT( "Continue" ), loadAutosaved,  200, 30 );
      _this->add( continueButton, -20, 320 );
    } )
  }

  if( quicksaveFile.stat() ) {
    OZ_MAIN_CALL( this, {
      Button* quickLoadButton = new Button( OZ_GETTEXT( "Quickload" ), loadQuicksaved, 200, 30 );
      _this->add( quickLoadButton, -20, 280 );
    } )
  }

  flags &= ~UPDATE_BIT;
}

bool MainMenu::onMouseEvent()
{
  return passMouseEvents();
}

void MainMenu::onDraw()
{
  shape.colour( style.colours.background );
  shape.fill( 0, 0, camera.width, camera.height );

  shape.colour( style.colours.menuStrip );
  shape.fill( camera.width - 240, 0, 240, camera.height );

  copyright.draw( this, true );
  title.draw( this, true );

  drawChildren();
}

MainMenu::MainMenu() :
  Area( camera.width, camera.height ),
  copyright( 20, 10, 360, 3, Font::SMALL, Area::ALIGN_NONE ),
  title( -120, -20, ALIGN_HCENTRE | ALIGN_VCENTRE, Font::LARGE, "OpenZone " OZ_VERSION )
{
  flags = UPDATE_BIT;

  Button* missionsButton  = new Button( OZ_GETTEXT( "Missions" ),  openMissions,       200, 30 );
  Button* settingsButton  = new Button( OZ_GETTEXT( "Settings" ),  openSettings,       200, 30 );
  Button* creditsButton   = new Button( OZ_GETTEXT( "Credits" ),   openCredits,        200, 30 );
  Button* quitButton      = new Button( OZ_GETTEXT( "Exit" ),      quit,               200, 30 );

  add( missionsButton,  -20, 220 );
  add( settingsButton,  -20, 160 );
  add( creditsButton,   -20, 120 );
  add( quitButton,      -20,  60 );
}

}
}
}
