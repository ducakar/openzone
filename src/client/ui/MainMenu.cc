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

static void loadTutorial( Button* )
{
  Stage::nextStage = &gameStage;
  gameStage.stateFile = "";
  gameStage.mission = "tutorial";
}

static void loadTest( Button* )
{
  Stage::nextStage = &gameStage;
  gameStage.stateFile = "";
  gameStage.mission = "test";
}

static void loadCvicek( Button* )
{
  Stage::nextStage = &gameStage;
  gameStage.stateFile = "";
  gameStage.mission = "cvicek";
}

static void settings( Button* sender )
{
  MainMenu* mainMenu = static_cast<MainMenu*>( sender->parent );

  mainMenu->show( false );
  mainMenu->settingsFrame->show( true );
}

static void exit( Button* )
{
  menuStage.doExit = true;
}

void MainMenu::onDraw()
{
  shape.colour( 0.1f, 0.1f, 0.1f, 1.0f );
  shape.fill( 0, 0, camera.width, camera.height );

  Frame::onDraw();

  copyright.draw( this );
}

MainMenu::MainMenu( bool showAutosaved, bool showQuicksaved ) :
  Frame( 400, 450, OZ_APPLICATION_TITLE " " OZ_APPLICATION_VERSION " " OZ_SYSTEM_NAME ),
  copyright( 10, 10, 380, 9, Font::SANS )
{
  x = ( camera.width  - width ) / 2;
  y = ( camera.height - height ) / 2;

  copyright.set( "OpenZone  Copyright © 2002-2012 Davorin Učakar\n\n%s",
                 OZ_GETTEXT( "This program comes with ABSOLUTELY NO WARRANTY. "
                             "This is free software, and you are welcome to redistribute it "
                             "under certain conditions; See COPYING file for details.\n\n"
                             "Data files come form different sources. See respective README and COPYING "
                             "files for details about copyrights and licences." ) );

  if( showAutosaved ) {
    add( new Button( OZ_GETTEXT( "Continue" ), continueAutosaved, 300, 20 ), 50, -40 );
  }
  if( showQuicksaved ) {
    add( new Button( OZ_GETTEXT( "Load Quicksave" ), continueQuicksaved, 300, 20 ), 50, -70 );
  }

  add( new Button( OZ_GETTEXT( "Tutorial" ), loadTutorial, 300, 20 ), 50, -130 );
  add( new Button( OZ_GETTEXT( "Test World" ), loadTest, 300, 20 ), 50, -160 );
  add( new Button( OZ_GETTEXT( "Mission 1: Cvicek" ), loadCvicek, 300, 20 ), 50, -190 );

  add( new Button( OZ_GETTEXT( "Settings" ), settings, 300, 20 ), 50, -250 );
  add( new Button( OZ_GETTEXT( "Exit" ), exit, 300, 20 ), 50, -280 );

  settingsFrame = new SettingsFrame( this );

  ui.root->add( settingsFrame, CENTRE, CENTRE );
  settingsFrame->show( false );
}

MainMenu::~MainMenu()
{
  ui.root->remove( settingsFrame );
}

}
}
}
