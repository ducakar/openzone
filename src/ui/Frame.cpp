/*
 *  Frame.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "ui/Frame.hpp"

#include "client/Shader.hpp"

#include <GL/gl.h>

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

    if( !( flags & GRAB_BIT ) && passMouseEvents() ) {
      return true;
    }
    if( mouse.buttons & SDL_BUTTON_LMASK ) {
      if( mouse.leftClick ) {
        flags |= GRAB_BIT;
      }
      if( flags & GRAB_BIT ) {
        mouse.icon = Mouse::MOVE;
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
    glUniform4f( param.oz_Colour, 0.0f, 0.0f, 0.0f, 0.25f );
    fill( 0, 0, width, height );

    drawChildren();
  }

}
}
}
