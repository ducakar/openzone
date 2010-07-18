/*
 *  Frame.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "ui/Frame.hpp"

#include <SDL_opengl.h>

namespace oz
{
namespace client
{
namespace ui
{

  bool Frame::onMouseEvent()
  {
    if( ( ~flags & GRAB_BIT ) && passMouseEvents() ) {
      return true;
    }
    if( mouse.buttons & SDL_BUTTON_LMASK ) {
      if( mouse.leftClick ) {
        flags |= GRAB_BIT;
      }
      if( flags & GRAB_BIT ) {
        mouse.type = Mouse::MOVE;
        move( mouse.relX, mouse.relY );
      }
    }
    else {
      flags &= ~GRAB_BIT;
    }
    return true;
  }

  void Frame::onDraw()
  {
    glColor4f( 0.0f, 0.0f, 0.0f, 0.25f );
    fill( 0, 0, width, height );

    drawChildren();
  }

}
}
}
