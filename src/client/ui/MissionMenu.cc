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
 * @file client/ui/MissionMenu.cc
 */

#include "stable.hh"

#include "client/ui/MissionMenu.hh"

#include "client/Camera.hh"
#include "client/Shape.hh"
#include "client/GameStage.hh"

namespace oz
{
namespace client
{
namespace ui
{

static void back( Button* sender )
{
  MissionMenu* missionMenu = static_cast<MissionMenu*>( sender->parent );

  missionMenu->parent->remove( missionMenu );
}

bool MissionButton::onMouseEvent()
{
  int selection = missionMenu->scroll + index;

  if( missionMenu->selection != selection ) {
    missionMenu->selection = selection;

    if( selection == -1 ) {
      missionMenu->description.set( " " );
    }
    else {
      missionMenu->description.set( "%s", missionMenu->missions[selection].description.cstr() );
    }
  }

  return Button::onMouseEvent();
}

MissionButton::MissionButton( const char* text, Callback* callback, MissionMenu* missionMenu_,
                              int index_, int width, int height ) :
  Button( text, callback, width, height ), missionMenu( missionMenu_ ), index( index_ )
{
}

void MissionMenu::loadMission( Button* sender )
{
  MissionButton* button      = static_cast<MissionButton*>( sender );
  MissionMenu*   missionMenu = static_cast<MissionMenu*>( sender->parent );

  Stage::nextStage = &gameStage;
  gameStage.stateFile = "";
  gameStage.mission = missionMenu->missions[missionMenu->scroll + button->index].name;
}

void MissionMenu::onReposition()
{
  width  = camera.width;
  height = camera.height;

  description.resize( width - 320 );

  if( selection != -1 ) {
    description.set( "%s", missions[selection].description.cstr() );
  }
}

bool MissionMenu::onMouseEvent()
{
  return passMouseEvents();
}

void MissionMenu::onDraw()
{
  shape.colour( 0.0f, 0.0f, 0.0f, 1.0f );
  shape.fill( camera.width - 240, 0, 240, camera.height );

  description.draw( this, true );

  drawChildren();
}

MissionMenu::MissionMenu() :
  Area( camera.width, camera.height ),
  description( 40, 100, camera.width - 320, 8, Font::SANS, ALIGN_NONE )
{
  Button* backButton = new Button( OZ_GETTEXT( "Back" ), back, 200, 30 );
  add( backButton, -20, 30 );

  PFile missionRootDir( "mission" );
  DArray<PFile> missionDirs = missionRootDir.ls();

  foreach( missionDir, missionDirs.citer() ) {
    PFile descriptionFile( missionDir->path() + "/description.json" );

    JSON descriptionConfig;
    if( !descriptionConfig.load( &descriptionFile ) ) {
      continue;
    }

    const String& title       = descriptionConfig["title"].asString();
    const String& description = descriptionConfig["description"].asString();

    MissionInfo mission = { missionDir->baseName(), title, description };
    missions.add( mission );
  }

  nSelections = ( camera.height - 150 ) / 40;
  nSelections = min( nSelections, missions.length() );
  selection   = -1;
  scroll      = 0;

  for( int i = 0; i < nSelections; ++i ) {
    Button* missionButton = new MissionButton( missions[i].title, loadMission, this, i, 200, 30 );
    add( missionButton, -20, -( i + 1 ) * 40 );
  }
}

}
}
}
