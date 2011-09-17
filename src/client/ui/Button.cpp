/*
 *  Button.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/ui/Button.hpp"

#include "client/Shader.hpp"

#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  bool Button::onMouseEvent()
  {
    isHighlighted = true;

    if( mouse.leftClick ) {
      isClicked = true;

      if( callback != null ) {
        callback( this );
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
    print( width / 2, height / 2, ALIGN_CENTRE, "%s", label.cstr() );

    isHighlighted = false;
    isClicked = false;
  }

  Button::Button( const char* label_, Callback* callback, int width, int height ) :
      Area( width, height ), callback( callback ), isHighlighted( false ), isClicked( false ),
      label( label_ )
  {}

  void Button::setCallback( Callback* callback_ )
  {
    callback = callback_;
  }

}
}
}
