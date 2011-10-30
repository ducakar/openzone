/*
 *  Frame.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/ui/Frame.hpp"

#include "client/Shader.hpp"
#include "client/OpenGL.hpp"

#include "client/ui/Keyboard.hpp"

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

  if( ( keyboard.keys[SDLK_LALT] || keyboard.keys[SDLK_RALT] ) &&
      ( mouse.buttons & SDL_BUTTON_LMASK ) )
  {
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
  if( !( flags & GRAB_BIT ) && passMouseEvents() ) {
    return true;
  }
  return true;
}

void Frame::onDraw()
{
  glUniform4f( param.oz_Colour, 0.0f, 0.0f, 0.0f, 0.3f );
  fill( 0, 0, width, height );

  title.draw( this );

  drawChildren();
}

Frame::Frame( int width, int height, const char* title_ ) :
    Area( width, height + Font::INFOS[Font::LARGE].height + 8 )
{
  int textHeight = font.INFOS[Font::LARGE].height;

  title.set( width / 2, -textHeight - 8, ALIGN_HCENTRE, Font::LARGE, title_ );
}

Frame::Frame( int x, int y, int width, int height, const char* title_ ) :
    Area( x, y, width, height + Font::INFOS[Font::LARGE].height + 8 )
{
  title.set( width / 2, -textHeight - 8, ALIGN_HCENTRE, Font::LARGE, title_ );
}

}
}
}
