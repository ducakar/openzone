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

#include <stable.hh>
#include <client/ui/Mouse.hh>

#include <client/Shape.hh>
#include <client/Camera.hh>
#include <client/Context.hh>
#include <client/Window.hh>
#include <client/Input.hh>
#include <client/OpenGL.hh>

#include <client/ui/Area.hh>

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

    if( 0 <= newX && newX < camera.width ) {
      input.lookX = 0.0f;
    }
    else {
      newX = clamp( newX, 0, camera.width - 1 );
    }

    if( 0 < newY && newY < camera.height ) {
      input.lookY = 0.0f;
    }
    else {
      newY = clamp( newY, 0, camera.height - 1 );
    }

    dx = newX - x;
    dy = newY - y;
    x  = newX;
    y  = newY;
  }
  else {
    dx = 0;
    dy = 0;
    x  = camera.centreX;
    y  = camera.centreY;
  }
}

void Mouse::draw() const
{
  if( doShow ) {
    const Cursor& cur = cursors[icon];

    shape.colour( 1.0f, 1.0f, 1.0f, 1.0f );
    glBindTexture( GL_TEXTURE_2D, cur.texId );
    shape.fill( x - cur.hotspotX, y + 1 + cur.hotspotY - cur.size, cur.size, cur.size );
    glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
  }
}

void Mouse::init()
{
  Log::print( "Initialising Mouse ..." );

  x      = camera.centreX;
  y      = camera.centreY;
  dx     = 0;
  dy     = 0;

  icon   = ARROW;
  doShow = false;

  for( int i = 0; i < CURSORS_MAX; ++i ) {
    File file( File::VIRTUAL, String::str( "ui/cur/%s.ozCur", NAMES[i] ) );

    Buffer buffer = file.read();
    if( buffer.isEmpty() ) {
      OZ_ERROR( "Cursor loading failed" );
    }

    InputStream is = buffer.inputStream();

    cursors[i].size     = is.readInt();
    cursors[i].hotspotX = is.readInt();
    cursors[i].hotspotY = is.readInt();
    cursors[i].texId    = context.readTextureLayer( &is );
  }

  Log::printEnd( " OK" );
}

void Mouse::destroy()
{
  Log::print( "Destroying Mouse ..." );

  for( int i = 0; i < CURSORS_MAX; ++i ) {
    glDeleteTextures( 1, &cursors[i].texId );
    cursors[i].texId = 0;
  }

  Log::printEnd( " OK" );
}

Mouse mouse;

}
}
}
