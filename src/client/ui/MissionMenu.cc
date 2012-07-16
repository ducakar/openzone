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
#include "client/Context.hh"
#include "client/GameStage.hh"
#include "client/OpenGL.hh"

namespace oz
{
namespace client
{
namespace ui
{

static void back( Button* sender )
{
  OZ_MAIN_CALL( sender, {
    Button*      sender      = static_cast<Button*>( _this );
    MissionMenu* missionMenu = static_cast<MissionMenu*>( sender->parent );

    missionMenu->parent->remove( missionMenu );
  } )
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
      missionMenu->imageId = missionMenu->missions[selection].imageId;
    }
  }

  return Button::onMouseEvent();
}

MissionButton::MissionButton( const char* text, Callback* callback, MissionMenu* missionMenu_,
                              int index_, int width, int height ) :
  Button( text, callback, width, height ), missionMenu( missionMenu_ ), index( index_ )
{}

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
  height = camera.height - 40;

  description.resize( width - 320 );

  if( selection != -1 ) {
    description.set( "%s", missions[selection].description.cstr() );
  }

  imageX      = 40;
  imageY      = 100 + 8 * Font::INFOS[Font::SANS].height;
  imageWidth  = width - 320;
  imageHeight = height - imageY;

  float aspect = float( imageWidth ) / float( imageHeight );
  if( aspect < 16.0f / 9.0f ) {
    imageHeight = int( float( imageWidth ) / 16.0f * 9.0f );
  }
  else if( aspect > 16.0f / 9.0f ) {
    imageWidth = int( float( imageHeight ) / 9.0f * 16.0f );
    imageX = ( width - 240 - imageWidth ) / 2;
  }
}

bool MissionMenu::onMouseEvent()
{
  return passMouseEvents();
}

void MissionMenu::onDraw()
{
  shape.colour( 0.0f, 0.0f, 0.0f, 1.0f );
  shape.fill( width - 240, 0, 240, height );

  if( imageId != 0 ) {
    glBindTexture( GL_TEXTURE_2D, imageId );
    shape.colour( 1.0f, 1.0f, 1.0f, 1.0f );
    shape.fill( imageX, imageY, imageWidth, imageHeight );
    glBindTexture( GL_TEXTURE_2D, 0 );
  }

  description.draw( this, true );

  drawChildren();
}

MissionMenu::MissionMenu() :
  Area( camera.width, camera.height ),
  description( 40, 80, camera.width - 320, 8, Font::SANS, ALIGN_NONE ),
  imageId( 0 )
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

    String missionName = missionDir->baseName();

    Lingua lingua;
    lingua.initMission( missionName );

    const char* title       = lingua.get( descriptionConfig["title"].get( missionName ) );
    const char* description = lingua.get( descriptionConfig["description"].get( "" ) );

    PFile image( missionDir->path() + "/description.ozThumbnail" );

    uint imageId = 0;
    if( image.stat() ) {
      imageId = context.loadTextureLayer( image.path() );
    }

    MissionInfo mission = { missionName, title, description, imageId };
    missions.add( mission );

    lingua.free();
  }

  nSelections    = ( camera.height - 150 ) / 40;
  nSelections    = min( nSelections, missions.length() );
  selection      = -1;
  scroll         = 0;

  for( int i = 0; i < nSelections; ++i ) {
    Button* missionButton = new MissionButton( missions[i].title, loadMission, this, i, 200, 30 );
    add( missionButton, -20, -( i + 1 ) * 40 );
  }

  onReposition();
}

MissionMenu::~MissionMenu()
{
  for( int i = 0; i < missions.length(); ++i ) {
    glDeleteTextures( 1, &missions[i].imageId );
  }
}

}
}
}
