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
#include <client/Context.hh>
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
  wasClicked    = false;
}

bool Button::onMouseEvent()
{
  if( input.keys[Input::KEY_UI_ALT] ) {
    return false;
  }

  bool buttonDown = input.buttons & ~input.oldButtons & Input::LEFT_BUTTON;
  bool buttonUp   = ~input.buttons & input.oldButtons & Input::LEFT_BUTTON;

  isHighlighted = true;
  isClicked     = wasClicked && ( input.buttons & Input::LEFT_BUTTON );

  if( wasClicked && buttonUp && callback != nullptr ) {
    callback( this );
  }
  else if( buttonDown ) {
    isClicked  = true;
    wasClicked = true;

    if( style.sounds.click >= 0 ) {
      context.playSample( style.sounds.click );
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
    wasClicked = false;
  }
  else {
    shape.colour( style.colours.button );
    wasClicked = false;
  }

  shape.fill( x, y, width, height );
  label.draw( this );

  isHighlighted = false;
  isClicked     = false;
}

Button::Button( const char* text, Callback* callback_, int width, int height ) :
  Area( width, height ),
  label( width / 2, height / 2, ALIGN_CENTRE, Font::SANS, "%s", text ),
  callback( callback_ ), isHighlighted( false ), isClicked( false ), wasClicked( false )
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
