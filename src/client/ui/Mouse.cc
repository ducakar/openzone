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
 * @file client/ui/Mouse.cc
 */

#include "stable.hh"

#include "client/ui/Mouse.hh"

#include "client/Context.hh"
#include "client/Camera.hh"
#include "client/Shape.hh"
#include "client/OpenGL.hh"

#include "client/ui/Area.hh"

namespace oz
{
namespace client
{
namespace ui
{

Mouse mouse;

const char* const Mouse::NAMES[] = {
  "left_ptr",
  "ibeam",
  "pointing_hand",
  "openhand",
  "closedhand",
  "size_all"
};

void Mouse::reset()
{
  SDL_WarpMouse( ushort( camera.centreX ), ushort( camera.centreY ) );
  // Suppress mouse motion event generated by SDL_WarpMouse().
  SDL_PumpEvents();
  SDL_GetRelativeMouseState( null, null );
}

void Mouse::prepare()
{
  relX = 0;
  relY = 0;
  relZ = 0;
  relW = 0;

  oldButtons = buttons;
  buttons    = currButtons;

  icon = ARROW;
}

void Mouse::update( bool hasFocus )
{
  if( hasFocus ) {
    int inputX, inputY;
    SDL_GetRelativeMouseState( &inputX, &inputY );

    relX = +inputX;
    relY = -inputY;

    // If input is not grabbed we must centre mouse so it cannot move out of the window.
    if( isJailed ) {
      reset();
    }
  }

  if( doShow ) {
    icon = ARROW;

    int desiredX = x + relX;
    int desiredY = y + relY;

    x = clamp( desiredX, 0, camera.width  - 1 );
    y = clamp( desiredY, 0, camera.height - 1 );

    overEdgeX = x != desiredX ? desiredX - x : 0;
    overEdgeY = y != desiredY ? desiredY - y : 0;
  }
  else {
    x = camera.centreX;
    y = camera.centreY;

    overEdgeX = relX;
    overEdgeY = relY;
  }

  int clickedButtons = buttons & ~oldButtons;

  leftClick   = ( clickedButtons & SDL_BUTTON_LMASK  ) != 0;
  middleClick = ( clickedButtons & SDL_BUTTON_MMASK  ) != 0;
  rightClick  = ( clickedButtons & SDL_BUTTON_RMASK  ) != 0;
  wheelUp     = relW > 0;
  wheelDown   = relW < 0;
}

void Mouse::draw() const
{
  const Cursor& cur = cursors[icon];

  if( doShow ) {
    shader.colour( Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
    glBindTexture( GL_TEXTURE_2D, cur.texId );
    shape.fill( x - cur.hotspotX, y + 1 + cur.hotspotY - cur.size, cur.size, cur.size );
    glBindTexture( GL_TEXTURE_2D, 0 );
  }
}

void Mouse::load()
{
  Log::print( "Loading Mouse ..." );

  x = camera.centreX;
  y = camera.centreY;

  for( int i = 0; i < CURSORS_MAX; ++i ) {
    PFile file( String::str( "ui/cur/%s.ozCur", NAMES[i] ) );
    if( !file.map() ) {
      throw Exception( "Cursor loading failed" );
    }

    InputStream is = file.inputStream();

    cursors[i].size     = is.readInt();
    cursors[i].hotspotX = is.readInt();
    cursors[i].hotspotY = is.readInt();
    cursors[i].texId    = context.readTextureLayer( &is, file.path() );

    file.unmap();
  }

  Log::printEnd( " OK" );
}

void Mouse::unload()
{
  Log::print( "Unloading Mouse ..." );

  for( int i = 0; i < CURSORS_MAX; ++i ) {
    glDeleteTextures( 1, &cursors[i].texId );
    cursors[i].texId = 0;
  }

  Log::printEnd( " OK" );
}

void Mouse::init()
{
  doShow   = false;
  isJailed = true;
  icon     = ARROW;

  x = 0;
  y = 0;

  relX = 0;
  relY = 0;
  relZ = 0;
  relW = 0;

  buttons     = 0;
  oldButtons  = 0;
  currButtons = 0;

  leftClick   = false;
  middleClick = false;
  rightClick  = false;
  wheelUp     = false;
  wheelDown   = false;
}

}
}
}
