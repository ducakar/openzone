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
 * @file client/ui/Button.cc
 */

#include "stable.hh"

#include "client/ui/Button.hh"

#include "client/Shader.hh"
#include "client/OpenGL.hh"

#include "client/ui/Keyboard.hh"

namespace oz
{
namespace client
{
namespace ui
{

bool Button::onMouseEvent()
{
  if( !keyboard.keys[SDLK_LALT] && !keyboard.keys[SDLK_RALT] ) {
    isHighlighted = true;

    if( mouse.leftClick ) {
      isClicked = true;

      if( callback != null ) {
        callback( this );
      }
    }
  }
  return true;
}

void Button::onDraw()
{
  if( isClicked ) {
    glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 1.0f );
  }
  else if( isHighlighted ) {
    glUniform4f( param.oz_Colour, 0.8f, 0.8f, 0.8f, 0.4f );
  }
  else {
    glUniform4f( param.oz_Colour, 0.6f, 0.6f, 0.6f, 0.4f );
  }

  fill( 0, 0, width, height );
  label.draw( this );

  isHighlighted = false;
  isClicked = false;
}

Button::Button( const char* text, Callback* callback, int width, int height ) :
  Area( width, height ),
  label( width / 2, height / 2, ALIGN_CENTRE, Font::SANS, text ),
  callback( callback ), isHighlighted( false ), isClicked( false )
{}

void Button::setCallback( Callback* callback_ )
{
  callback = callback_;
}

}
}
}
