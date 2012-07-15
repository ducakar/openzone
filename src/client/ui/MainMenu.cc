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

#include "stable.hh"

#include "client/ui/MainMenu.hh"

#include "BuildInfo.hh"

#include "client/Camera.hh"
#include "client/Shape.hh"
#include "client/GameStage.hh"
#include "client/MenuStage.hh"
#include "client/OpenGL.hh"

#include "client/ui/SettingsFrame.hh"
#include "client/ui/UI.hh"
#include "MissionMenu.hh"

namespace oz
{
namespace client
{
namespace ui
{

static void continueAutosaved( Button* )
{
  Stage::nextStage = &gameStage;
  gameStage.stateFile = GameStage::AUTOSAVE_FILE;
  gameStage.mission = "";
}

static void continueQuicksaved( Button* )
{
  Stage::nextStage = &gameStage;
  gameStage.stateFile = GameStage::QUICKSAVE_FILE;
  gameStage.mission = "";
}

static void openMissions( Button* sender )
{
  MainMenu* mainMenu = static_cast<MainMenu*>( sender->parent );

  mainMenu->add( new MissionMenu(), 0, 0 );
}

static void settings( Button* sender )
{
  MainMenu* mainMenu = static_cast<MainMenu*>( sender->parent );

  mainMenu->add( new SettingsFrame(), Area::CENTRE, Area::CENTRE );
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
  copyright.set( "OpenZone Engine © 2012 Davorin Učakar. %s",
                 OZ_GETTEXT( "Licensed under GNU GPL 3.0. Data files and libraries distributed with"
                             " OpenZone are work of various authors and use separate licences."
                             " See doc/README.html and/or respective README.txt and COPYING.txt"
                             " files in game data archives for details." ) );

  foreach( child, children.iter() ) {
    child->reposition();
  }
}

bool MainMenu::onMouseEvent()
{
  return passMouseEvents();
}

void MainMenu::onDraw()
{
  shape.colour( 0.05f, 0.05f, 0.05f, 1.0f );
  shape.fill( 0, 0, camera.width, camera.height );

  shape.colour( 0.0f, 0.0f, 0.0f, 1.0f );
  shape.fill( camera.width - 240, 0, 240, camera.height );

  copyright.draw( this, false );

  drawChildren();
}

MainMenu::MainMenu() :
  Area( camera.width, camera.height ),
  copyright( 20, 10, 360, 2, Font::SMALL, Area::ALIGN_NONE )
{
  onReposition();

  Button* continueButton  = new Button( OZ_GETTEXT( "Continue" ),  continueAutosaved,  200, 30 );
  Button* quickLoadButton = new Button( OZ_GETTEXT( "Quickload" ), continueQuicksaved, 200, 30 );
  Button* missionsButton  = new Button( OZ_GETTEXT( "Missions" ),  openMissions,       200, 30 );
  Button* settingsButton  = new Button( OZ_GETTEXT( "Settings" ),  settings,           200, 30 );
  Button* quitButton      = new Button( OZ_GETTEXT( "Exit" ),      quit,               200, 30 );

  add( continueButton,  -20, 270 );
  add( quickLoadButton, -20, 230 );
  add( missionsButton,  -20, 170 );
  add( settingsButton,  -20, 110 );
  add( quitButton,      -20,  70 );
}

}
}
}
