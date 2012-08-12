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
 * @file client/ui/CreditsMenu.cc
 */

#include "stable.hh"

#include "client/ui/CreditsMenu.hh"

#include "client/Shape.hh"
#include "client/Camera.hh"
#include "client/Context.hh"
#include "client/Input.hh"
#include "client/OpenGL.hh"
#include "client/NaCl.hh"

#include "client/ui/Style.hh"
#include "client/ui/Button.hh"

namespace oz
{
namespace client
{
namespace ui
{

static void back( Button* sender )
{
  OZ_MAIN_CALL( sender, {
    Button*      _sender     = static_cast<Button*>( _this );
    CreditsMenu* creditsMenu = static_cast<CreditsMenu*>( _sender->parent );

    creditsMenu->parent->remove( creditsMenu );
  } )
}

void CreditsMenu::onReposition()
{
  width  = camera.width;
  height = camera.height;

  int nLabels = ( height - 140 ) / stride;
  labels.resize( nLabels );

  for( int i = 0; i < nLabels; ++i ) {
    int line = scroll + i;

    if( line < nLabels ) {
      labels[i].set( 0, 0, ALIGN_HCENTRE, Font::SANS, " " );
    }
    else {
      labels[i].set( 0, 0, ALIGN_HCENTRE, Font::SANS, "%s", lines[line - nLabels].cstr() );
    }
  }
}

void CreditsMenu::onUpdate()
{
  int nEntries = ( labels.length() + lines.length() );

  if( !isPaused || bias == stride - 1 ) {
    ++bias;
  }

  if( bias == stride ) {
    scroll = ( scroll + 1 ) % nEntries;
    bias   = 0;
  }
  else if( bias == stride - 1 ) {
    for( int i = 0; i < labels.length(); ++i ) {
      int line = ( scroll + 1 + i ) % nEntries;

      if( line < labels.length() ) {
        labels[i].set( " " );
      }
      else {
        labels[i].set( "%s", lines[ line - labels.length() ].cstr() );
      }
    }
  }
}

bool CreditsMenu::onMouseEvent()
{
  if( mouse.x < width - 240 ) {
    if( input.leftClick ) {
      isPaused = !isPaused;
    }
    else if( input.mouseW != 0 ) {
      isPaused = input.mouseW >= 0;
    }
  }

  passMouseEvents();
  return true;
}

void CreditsMenu::onDraw()
{
  shape.colour( 0.0f, 0.0f, 0.0f, 1.0f );
  shape.fill( width - 240, 0, 240, height - 40 );

  for( int i = 0; i < labels.length(); ++i ) {
    int x = ( width - 240 ) / 2;
    int y = height - 60 - ( i + 1 )*stride + bias;

    labels[i].set( x, y );
    labels[i].draw( this, true );
  }

  drawChildren();
}

CreditsMenu::CreditsMenu() :
  Area( camera.width, camera.height ),
  stride( style.fonts[Font::SANS].height ), scroll( 0 ), bias( 0 ), isPaused( false )
{
  flags = UPDATE_BIT;

  Button* backButton = new Button( OZ_GETTEXT( "Back" ), back, 200, 30 );
  add( backButton, -20, 20 );

  PFile creditsDir( "credits" );
  DArray<PFile> creditsFiles = creditsDir.ls();

  lines.add( "OpenZone " OZ_VERSION );
  lines.add( "" );
  lines.add( OZ_GETTEXT( "engine developer" ) );
  lines.add( "Davorin Učakar" );

  foreach( creditsFile, creditsFiles.iter() ) {
    if( !creditsFile->map() ) {
      throw Exception( "Failed to map '%s'", creditsFile->path().cstr() );
    }

    lines.add( "" );
    lines.add( "" );
    lines.add( "" );
    lines.add( "" );
    lines.add( "" );
    lines.add( String::str( ">> %s \"%s\" <<", OZ_GETTEXT( "package" ),
                            creditsFile->baseName().cstr() ) );
    lines.add( "" );
    lines.add( "" );

    String contents = "";
    InputStream is = creditsFile->inputStream();

    while( is.isAvailable() ) {
      contents += is.readLine() + "\n";
    }
    contents = lingua.get( contents );

    DArray<String> fileLines = contents.split( '\n' );
    lines.takeAll( fileLines, fileLines.length() );

    if( !lines.isEmpty() && lines.last().isEmpty() ) {
      lines.popLast();
    }
  }
}

}
}
}
