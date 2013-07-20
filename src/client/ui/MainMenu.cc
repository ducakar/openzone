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
 * @file client/ui/MainMenu.cc
 */

#include <client/ui/MainMenu.hh>

#include <client/Camera.hh>
#include <client/Shape.hh>
#include <client/GameStage.hh>
#include <client/MenuStage.hh>
#include <client/BuildInfo.hh>
#include <client/ui/Style.hh>
#include <client/ui/MissionMenu.hh>
#include <client/ui/SettingsFrame.hh>
#include <client/ui/CreditsMenu.hh>
#include <client/ui/UI.hh>

#if defined( __ANDROID__ ) || defined( __native_client__ )
#elif defined( _WIN32 )
# include <shellapi.h>
#else
# include <unistd.h>
#endif

namespace oz
{
namespace client
{
namespace ui
{

static void loadAutosaved( Button* )
{
  Stage::nextStage = &gameStage;
  gameStage.stateFile = gameStage.autosaveFile;
  gameStage.mission = "";
}

static void loadQuicksaved( Button* )
{
  Stage::nextStage = &gameStage;
  gameStage.stateFile = gameStage.quicksaveFile;
  gameStage.mission = "";
}

static void openMissions( Button* sender )
{
  MainMenu* mainMenu = static_cast<MainMenu*>( sender->parent );
  mainMenu->add( new MissionMenu(), 0, 0 );
}

static void openSettings( Button* sender )
{
  MainMenu* mainMenu = static_cast<MainMenu*>( sender->parent );
  mainMenu->add( new SettingsFrame(), Area::CENTRE, Area::CENTRE );
}

static void openCredits( Button* sender )
{
  MainMenu* mainMenu = static_cast<MainMenu*>( sender->parent );
  mainMenu->add( new CreditsMenu(), 0, 0 );
}

static void openWeb( Button* )
{
#if defined( __ANDROID__ )
#elif defined( __native_client__ )
  Pepper::post( "navi:http://ducakar.github.io/openzone/" );
#elif defined( _WIN32 )
  ShellExecute( nullptr, "open", "http://ducakar.github.io/openzone/", nullptr, nullptr,
                SW_SHOWNORMAL );
#else
  if( fork() == 0 ) {
    execl( "/bin/sh", "sh", "xdg-open", "http://ducakar.github.io/openzone/", nullptr );
    _Exit( 0 );
  }
#endif

  Window::minimise();
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
  copyright.setText( "OpenZone © 2002-2013 Davorin Učakar. %s",
                     OZ_GETTEXT( "Licensed under GNU GPL 3.0. Game data archives and libraries"
                                 " distributed with OpenZone are work of various authors and use"
                                 " separate licences. For more details see doc/README.html and"
                                 " files named README.txt and COPYING.txt inside game data"
                                 " archives." ) );

  foreach( child, children.iter() ) {
    child->reposition();
  }
}

void MainMenu::onUpdate()
{
  gameStage.autosaveFile.stat();
  gameStage.quicksaveFile.stat();

  if( gameStage.autosaveFile.type() == File::REGULAR ) {
    Button* continueButton  = new Button( OZ_GETTEXT( "Continue" ), loadAutosaved,  200, 30 );
    add( continueButton, -20, 360 );
  }

  if( gameStage.quicksaveFile.type() == File::REGULAR ) {
    Button* quickLoadButton = new Button( OZ_GETTEXT( "Quickload" ), loadQuicksaved, 200, 30 );
    add( quickLoadButton, -20, 320 );
  }

  flags &= ~UPDATE_BIT;
}

bool MainMenu::onMouseEvent()
{
  return passMouseEvents();
}

bool MainMenu::onKeyEvent()
{
  return passKeyEvents();
}

void MainMenu::onDraw()
{
  shape.colour( style.colours.background );
  shape.fill( 0, 0, camera.width, camera.height );

  shape.colour( style.colours.menuStrip );
  shape.fill( camera.width - 240, 0, 240, camera.height );

  copyright.draw( this );
  title.draw( this );

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
  Button* webButton       = new Button( OZ_GETTEXT( "Web" ),       openWeb,            200, 30 );
  Button* quitButton      = new Button( OZ_GETTEXT( "Exit" ),      quit,               200, 30 );

  add( missionsButton,  -20, 260 );
  add( settingsButton,  -20, 200 );
  add( creditsButton,   -20, 160 );
  add( webButton,       -20, 120 );
  add( quitButton,      -20,  60 );
}

}
}
}
