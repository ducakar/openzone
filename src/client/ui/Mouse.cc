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
 * @file client/ui/Mouse.cc
 */

#include <client/ui/Mouse.hh>

#include <common/Timer.hh>
#include <client/Shader.hh>
#include <client/Shape.hh>
#include <client/Camera.hh>
#include <client/Input.hh>

namespace oz
{
namespace client
{
namespace ui
{

const char* const Mouse::NAMES[] = {
  "left_ptr",
  "ibeam",
  "pointing_hand",
  "openhand",
  "closedhand",
  "size_all"
};

void Mouse::update()
{
  icon = ARROW;

  if( doShow ) {
    int newX = x + Math::lround( input.mouseX );
    int newY = y + Math::lround( input.mouseY );

    if( newX < 0 || camera.width <= newX ) {
      newX = clamp( newX, 0, camera.width - 1 );
      input.lookX -= input.mouseX * input.mouseSensX;
    }
    if( newY < 0 || camera.height <= newY ) {
      newY = clamp( newY, 0, camera.height - 1 );
      input.lookY += input.mouseY * input.mouseSensY;
    }

    dx = newX - x;
    dy = newY - y;
    x  = newX;
    y  = newY;
  }
  else {
    input.lookX -= input.mouseX * input.mouseSensX;
    input.lookY += input.mouseY * input.mouseSensY;

    dx = 0;
    dy = 0;
    x  = camera.centreX;
    y  = camera.centreY;
  }
}

void Mouse::draw()
{
  Cursor& cursor = cursors[icon];

  if( doShow ) {
    if( icon != oldIcon || !wasShown ) {
      cursor.reset();
    }

    shape.colour( 1.0f, 1.0f, 1.0f, 1.0f );
    glBindTexture( GL_TEXTURE_2D, cursor.textureId() );
    shape.fill( x - cursor.hotspotLeft(), y - cursor.height() + 1 + cursor.hotspotTop(),
                cursor.width(), cursor.height() );
    glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );

    cursor.advance( timer.frameMicros / 1000 );
  }

  oldIcon  = icon;
  wasShown = doShow;
}

void Mouse::init()
{
  Log::print( "Initialising Mouse ..." );

  x        = camera.centreX;
  y        = camera.centreY;
  dx       = 0;
  dy       = 0;

  icon     = ARROW;
  oldIcon  = ARROW;
  doShow   = false;
  wasShown = false;

  for( int i = 0; i < CURSORS_MAX; ++i ) {
    File file( String::str( "@ui/cur/%s", NAMES[i] ) );

    if( !cursors[i].load( file ) ) {
      OZ_ERROR( "Cursor loading failed" );
    }
  }

  Log::printEnd( " OK" );
}

void Mouse::destroy()
{
  Log::print( "Destroying Mouse ..." );

  for( int i = 0; i < CURSORS_MAX; ++i ) {
    cursors[i].destroy();
  }

  Log::printEnd( " OK" );
}

Mouse mouse;

}
}
}
