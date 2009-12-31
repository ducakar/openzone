/*
 *  Button.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "precompiled.h"

#include "Button.h"

#include <GL/gl.h>

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
      glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    }
    else if( isHighlighted ) {
      glColor4f( 0.8f, 0.8f, 0.8f, 0.4f );
    }
    else {
      glColor4f( 0.6f, 0.6f, 0.6f, 0.4f );
    }
    fill( 0, 0, width, height );
    setFont( SANS );
    setFontColor( 200, 200, 200 );
    printCentered( width / 2, height / 2, "%s", label.cstr() );

    isHighlighted = false;
    isClicked = false;
  }

}
}
}
