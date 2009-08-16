/*
 *  Button.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Button.h"

namespace oz
{
namespace client
{
namespace ui
{

  void Button::onMouseEvent()
  {
    isHighlighted = true;

    if( mouse.leftClick ) {
      isClicked = true;

      if( callback != null ) {
        callback();
      }
    }
  }

  void Button::draw()
  {
    if( isClicked ) {
      glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    }
    else if( isHighlighted ) {
      glColor4f( 1.0f, 1.0f, 1.0f, 0.7f );
    }
    else {
      glColor4f( 0.8f, 0.8f, 0.8f, 0.7f );
    }
    fill( 0, 0, width, height );
    setFont( SANS );
    setFontColor( 0, 0, 0 );
    printCentered( width / 2, height / 2, label.cstr() );

    isHighlighted = false;
    isClicked = false;
  }

}
}
}
