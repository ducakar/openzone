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

#define SDL_BUTTON_WUMASK SDL_BUTTON( SDL_BUTTON_WHEELUP )
#define SDL_BUTTON_WDMASK SDL_BUTTON( SDL_BUTTON_WHEELDOWN )

namespace oz
{
namespace client
{
namespace ui
{

  Mouse mouse;

  void Mouse::init( int maxX, int maxY )
  {
    type = ARROW;
    doShow = false;
    setBounds( maxX, maxY );

    const char *x = config.get( "ui.cursor.x", "cursors/X_cursor.png" );
    cursors[X].texId    = context.loadTexture( x, false, GL_NEAREST, GL_NEAREST );
    cursors[X].size     = config.get( "ui.cursor.x.size", 32 );
    cursors[X].hotspotX = config.get( "ui.cursor.x.hotspot.x", 16 );
    cursors[X].hotspotY = config.get( "ui.cursor.x.hotspot.y", 16 );

    const char *arrow = config.get( "ui.cursor.arrow", "cursors/left_ptr.png" );
    cursors[ARROW].texId    = context.loadTexture( arrow, false, GL_NEAREST, GL_NEAREST );
    cursors[ARROW].size     = config.get( "ui.cursor.arrow.size", 32 );
    cursors[ARROW].hotspotX = config.get( "ui.cursor.arrow.hotspot.x", 1 );
    cursors[ARROW].hotspotY = config.get( "ui.cursor.arrow.hotspot.y", 1 );

    const char *move = config.get( "ui.cursor.move", "cursors/fleur.png" );
    cursors[MOVE].texId    = context.loadTexture( move, false, GL_NEAREST, GL_NEAREST );
    cursors[MOVE].size     = config.get( "ui.cursor.move.size", 32 );
    cursors[MOVE].hotspotX = config.get( "ui.cursor.move.hotspot.x", 16 );
    cursors[MOVE].hotspotY = config.get( "ui.cursor.move.hotspot.y", 16 );

    const char *text = config.get( "ui.cursor.text", "cursors/xterm.png" );
    cursors[TEXT].texId    = context.loadTexture( text, false, GL_NEAREST, GL_NEAREST );
    cursors[TEXT].size     = config.get( "ui.cursor.text.size", 32 );
    cursors[TEXT].hotspotX = config.get( "ui.cursor.text.hotspot.x", 16 );
    cursors[TEXT].hotspotY = config.get( "ui.cursor.text.hotspot.y", 16 );

    const char *hand = config.get( "ui.cursor.text", "cursors/hand2.png" );
    cursors[HAND].texId    = context.loadTexture( hand, false, GL_NEAREST, GL_NEAREST );
    cursors[HAND].size     = config.get( "ui.cursor.hand.size", 32 );
    cursors[HAND].hotspotX = config.get( "ui.cursor.hand.hotspot.x", 16 );
    cursors[HAND].hotspotY = config.get( "ui.cursor.hand.hotspot.y", 16 );
  }

  void Mouse::free()
  {
    context.freeTexture( cursors[X].texId );
    context.freeTexture( cursors[ARROW].texId );
    context.freeTexture( cursors[MOVE].texId );
    context.freeTexture( cursors[TEXT].texId );
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
      type = ARROW;

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

    leftClick   = !( b & SDL_BUTTON_LMASK )  && ( persButtons & SDL_BUTTON_LMASK );
    middleClick = !( b & SDL_BUTTON_MMASK )  && ( persButtons & SDL_BUTTON_MMASK );
    rightClick  = !( b & SDL_BUTTON_RMASK )  && ( persButtons & SDL_BUTTON_RMASK );
    wheelUp     = !( b & SDL_BUTTON_WUMASK ) && ( persButtons & SDL_BUTTON_WUMASK );
    wheelDown   = !( b & SDL_BUTTON_WDMASK ) && ( persButtons & SDL_BUTTON_WDMASK );

    b = persButtons;
    persButtons = currButtons;
  }

  void Mouse::draw() const
  {
    const Cursor &cur = cursors[type];

    if( doShow ) {
      glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
      glEnable( GL_TEXTURE_2D );
      glBindTexture( GL_TEXTURE_2D, cursors[type].texId );
      glBegin( GL_QUADS );
        glTexCoord2i( 0, 1 );
        glVertex2i( x - cur.hotspotX           , y + 1 + cur.hotspotY - cur.size );
        glTexCoord2i( 1, 1 );
        glVertex2i( x - cur.hotspotX + cur.size, y + 1 + cur.hotspotY - cur.size );
        glTexCoord2i( 1, 0 );
        glVertex2i( x - cur.hotspotX + cur.size, y + 1 + cur.hotspotY            );
        glTexCoord2i( 0, 0 );
        glVertex2i( x - cur.hotspotX           , y + 1 + cur.hotspotY            );
      glEnd();
      glDisable( GL_TEXTURE_2D );
    }
  }

}
}
}
