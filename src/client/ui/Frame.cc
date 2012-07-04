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
 * @file client/ui/Frame.cc
 */

#include "stable.hh"

#include "client/ui/Frame.hh"

#include "client/Input.hh"
#include "client/Shader.hh"
#include "client/OpenGL.hh"

namespace oz
{
namespace client
{
namespace ui
{

bool Frame::onMouseEvent()
{
  if( input.buttons != 0 ) {
    raise();
  }

  if( input.keys[SDLK_LALT] || input.keys[SDLK_RALT] ) {
    mouse.icon = Mouse::OPENHAND;

    if( input.buttons & Input::LEFT_BUTTON ) {
      if( input.leftClick ) {
        flags |= GRAB_BIT;
      }
      if( flags & GRAB_BIT ) {
        mouse.icon = Mouse::CLOSEDHAND;
        move( input.mouseX, input.mouseY );
      }
    }
    else {
      flags &= ~GRAB_BIT;
    }
  }
  if( !( flags & GRAB_BIT ) && passMouseEvents() ) {
    return true;
  }
  return true;
}

void Frame::onDraw()
{
  shader.colour( Vec4( 0.0f, 0.0f, 0.0f, 0.3f ) );
  fill( 0, 0, width, height );

  title.draw( this, false );

  drawChildren();
}

Frame::Frame( int width, int height, const char* text ) :
  Area( width, height + HEADER_SIZE )
{
  title.set( width / 2, -HEADER_SIZE / 2, ALIGN_HCENTRE | ALIGN_VCENTRE, Font::LARGE, "%s", text );
}

}
}
}
