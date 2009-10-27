/*
 *  Frame.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Frame.h"

namespace oz
{
namespace client
{
namespace ui
{

  void Frame::onMouseEvent()
  {
    if( mouse.b & SDL_BUTTON_LMASK ) {
      if( mouse.leftClick ) {
        flags |= GRAB_BIT;
      }
      if( flags & GRAB_BIT ) {
        mouse.type = Mouse::MOVE;
        move( mouse.moveX, mouse.moveY );
      }
    }
    else {
      flags &= ~GRAB_BIT;
    }
  }

  void Frame::draw()
  {
    glColor4f( 0.0f, 0.0f, 0.0f, 0.25f );
    fill( 0, 0, width, height );

    drawChildren();
  }

}
}
}