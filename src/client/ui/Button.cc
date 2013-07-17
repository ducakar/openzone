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
 * @file client/ui/Button.cc
 */

#include <client/ui/Button.hh>

#include <client/Input.hh>
#include <client/Shape.hh>
#include <client/ui/Style.hh>

namespace oz
{
namespace client
{
namespace ui
{

void Button::onVisibilityChange( bool )
{
  isHighlighted = false;
  isClicked     = false;
}

bool Button::onMouseEvent()
{
  if( !input.keys[Input::KEY_UI_ALT] ) {
    isHighlighted = true;

    if( callback != nullptr && ( clickMask == -1 || input.buttons & clickMask ) ) {
      isClicked = callback( this );
    }
  }
  return true;
}

void Button::onDraw()
{
  if( isClicked ) {
    shape.colour( style.colours.buttonClicked );
  }
  else if( isHighlighted ) {
    shape.colour( style.colours.buttonHover );
  }
  else {
    shape.colour( style.colours.button );
  }

  shape.fill( x, y, width, height );
  label.draw( this );

  isHighlighted = false;
  isClicked     = false;
}

Button::Button( const char* text, Callback* callback_, int width, int height ) :
  Area( width, height ),
  label( width / 2, height / 2, ALIGN_CENTRE, Font::SANS, "%s", text ),
  callback( callback_ ),
  clickMask( Input::LEFT_BUTTON ), isHighlighted( false ), isClicked( false )
{}

void Button::setLabel( const char* text )
{
  label.setText( "%s", text );
}

void Button::setCallback( Callback* callback_ )
{
  callback = callback_;
}

}
}
}
