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

#include "client/Camera.hh"
#include "client/Context.hh"
#include "client/Shape.hh"
#include "client/Window.hh"
#include "client/Input.hh"
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

void Mouse::update()
{
  if( doShow ) {
    icon = ARROW;

    int moveX = input.mouseX;
    int moveY = input.mouseY;

#ifdef __linux__

    // Compensate lack of mouse acceleration in fullscreen mode on X server. This code is not based
    // on actual code from X.Org, but experimentally tuned to match default X server acceleration
    // as closely as possible.
    if( window.isFull ) {
      float move2  = Math::sqrt( float( moveX*moveX + moveY*moveY ) );
      float factor = min( 0.5f + max( move2 - 8.0f, 0.0f ) * 0.04f, 2.0f );

      moveX = int( float( input.mouseX ) * factor );
      moveY = int( float( input.mouseY ) * factor );
    }

#endif

    int desiredX = x + moveX;
    int desiredY = y + moveY;

    x = clamp( desiredX, 0, camera.width  - 1 );
    y = clamp( desiredY, 0, camera.height - 1 );

    overEdgeX = x != desiredX ? desiredX - x : 0;
    overEdgeY = y != desiredY ? desiredY - y : 0;
  }
  else {
    x = camera.centreX;
    y = camera.centreY;

    overEdgeX = input.mouseX;
    overEdgeY = input.mouseY;
  }
}

void Mouse::draw() const
{
  if( doShow ) {
    const Cursor& cur = cursors[icon];

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
  x         = 0;
  y         = 0;

  overEdgeX = 0;
  overEdgeY = 0;

  icon      = ARROW;
  doShow    = false;
}

}
}
}
