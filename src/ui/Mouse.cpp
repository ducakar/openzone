/*
 *  Mouse.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Mouse.h"

#include "client/Context.h"

namespace oz
{
namespace client
{
namespace ui
{

  Mouse mouse;

  void Mouse::init( int maxX, int maxY )
  {
    doShow = false;
    setBounds( maxX, maxY );

    const char *file = config.get( "ui.cursor", "base/OpenZone/source/left_ptr.png" );
    size = config.get( "ui.cursor.size", 32 );
    hotspotX = config.get( "ui.cursor.hotspot.x", 0 );
    hotspotY = config.get( "ui.cursor.hotspot.y", 0 );

    ptrTexId = context.loadTexture( file, false, GL_NEAREST, GL_NEAREST );
  }

  void Mouse::free()
  {
    context.freeTexture( ptrTexId );
  }

  void Mouse::setBounds( int maxX_, int maxY_ )
  {
    maxX = maxX_;
    maxY = maxY_;
  }

  void Mouse::show()
  {
    doShow = true;
  }

  void Mouse::hide()
  {
    doShow = false;
  }

  void Mouse::update()
  {
    if( doShow ) {
      int desiredX = x + moveX;
      int desiredY = y + moveY;

      x = bound( desiredX, 0, maxX - 1 );
      y = bound( desiredY, 0, maxY - 1 );

      overEdgeX = x != desiredX ? desiredX - x : 0;
      overEdgeY = y != desiredY ? desiredY - y : 0;
    }
    else {
      x = maxX / 2;
      y = maxY / 2;

      overEdgeX = moveX;
      overEdgeY = moveY;
    }

    leftClick   = !( b & SDL_BUTTON_LMASK ) && ( newButtons & SDL_BUTTON_LMASK );
    middleClick = !( b & SDL_BUTTON_MMASK ) && ( newButtons & SDL_BUTTON_MMASK );
    rightClick  = !( b & SDL_BUTTON_RMASK ) && ( newButtons & SDL_BUTTON_RMASK );
    wheelUp     = newButtons & SDL_BUTTON( SDL_BUTTON_WHEELUP   );
    wheelDown   = newButtons & SDL_BUTTON( SDL_BUTTON_WHEELDOWN );

    b = newButtons;
  }

  void Mouse::draw() const
  {
    if( doShow ) {
      glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
      glEnable( GL_TEXTURE_2D );
      glBindTexture( GL_TEXTURE_2D, ptrTexId );
      glBegin( GL_QUADS );
        glTexCoord2i( 0, 1 );
        glVertex2i( x - hotspotX       , y + 1 + hotspotY - size );
        glTexCoord2i( 1, 1 );
        glVertex2i( x - hotspotX + size, y + 1 + hotspotY - size );
        glTexCoord2i( 1, 0 );
        glVertex2i( x - hotspotX + size, y + 1 + hotspotY        );
        glTexCoord2i( 0, 0 );
        glVertex2i( x - hotspotX       , y + 1 + hotspotY        );
      glEnd();
      glDisable( GL_TEXTURE_2D );
    }
  }

}
}
}
