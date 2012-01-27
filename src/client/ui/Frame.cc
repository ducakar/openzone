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

#include "client/Shader.hh"
#include "client/OpenGL.hh"

#include "client/ui/Keyboard.hh"

namespace oz
{
namespace client
{
namespace ui
{

bool Frame::onMouseEvent()
{
  if( mouse.buttons & ( SDL_BUTTON_LMASK | SDL_BUTTON_MMASK | SDL_BUTTON_RMASK ) ) {
    parent->focus( this );
  }

  if( keyboard.keys[SDLK_LALT] || keyboard.keys[SDLK_RALT] ) {
    mouse.icon = Mouse::OPENHAND;

    if( mouse.buttons & SDL_BUTTON_LMASK ) {
      if( mouse.leftClick ) {
        flags |= GRAB_BIT;
      }
      if( flags & GRAB_BIT ) {
        mouse.icon = Mouse::CLOSEDHAND;
        move( mouse.relX, mouse.relY );
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
  glUniform4f( param.oz_Colour, 0.0f, 0.0f, 0.0f, 0.3f );
  fill( 0, 0, width, height );

  title.draw( this, false );

  drawChildren();
}

Frame::Frame( int width, int height, const char* title_ ) :
  Area( width, height + Font::INFOS[Font::LARGE].height + 8 )
{
  int textHeight = font.INFOS[Font::LARGE].height;

  title.set( width / 2, -textHeight - 8, ALIGN_HCENTRE, Font::LARGE, "%s", title_ );
}

Frame::Frame( int x, int y, int width, int height, const char* title_ ) :
  Area( x, y, width, height + Font::INFOS[Font::LARGE].height + 8 )
{
  title.set( width / 2, -Font::INFOS[Font::LARGE].height - 8, ALIGN_HCENTRE, Font::LARGE,
             "%s", title_ );
}

}
}
}
