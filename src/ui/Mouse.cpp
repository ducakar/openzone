/*
 *  Mouse.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "ui/Mouse.hpp"

#include "client/Camera.hpp"
#include "client/Context.hpp"

#define SDL_BUTTON_WUMASK SDL_BUTTON( SDL_BUTTON_WHEELUP )
#define SDL_BUTTON_WDMASK SDL_BUTTON( SDL_BUTTON_WHEELDOWN )

namespace oz
{
namespace client
{
namespace ui
{

  Mouse mouse;

  void Mouse::init()
  {
    type = ARROW;
    doShow = false;

    x = camera.centreX;
    y = camera.centreY;
    relX = 0;
    relY = 0;

    buttons = 0;
    oldButtons = 0;
    currButtons = 0;

    const char* x = config.getSet( "ui.cursor.x", "cursors/X_cursor.png" );
    cursors[X].texId    = context.loadTexture( x, false, GL_NEAREST, GL_NEAREST );
    cursors[X].size     = config.getSet( "ui.cursor.x.size", 32 );
    cursors[X].hotspotX = config.getSet( "ui.cursor.x.hotspot.x", 16 );
    cursors[X].hotspotY = config.getSet( "ui.cursor.x.hotspot.y", 16 );

    const char* arrow = config.getSet( "ui.cursor.arrow", "cursors/left_ptr.png" );
    cursors[ARROW].texId    = context.loadTexture( arrow, false, GL_NEAREST, GL_NEAREST );
    cursors[ARROW].size     = config.getSet( "ui.cursor.arrow.size", 32 );
    cursors[ARROW].hotspotX = config.getSet( "ui.cursor.arrow.hotspot.x", 1 );
    cursors[ARROW].hotspotY = config.getSet( "ui.cursor.arrow.hotspot.y", 1 );

    const char* move = config.getSet( "ui.cursor.move", "cursors/fleur.png" );
    cursors[MOVE].texId    = context.loadTexture( move, false, GL_NEAREST, GL_NEAREST );
    cursors[MOVE].size     = config.getSet( "ui.cursor.move.size", 32 );
    cursors[MOVE].hotspotX = config.getSet( "ui.cursor.move.hotspot.x", 16 );
    cursors[MOVE].hotspotY = config.getSet( "ui.cursor.move.hotspot.y", 16 );

    const char* text = config.getSet( "ui.cursor.text", "cursors/xterm.png" );
    cursors[TEXT].texId    = context.loadTexture( text, false, GL_NEAREST, GL_NEAREST );
    cursors[TEXT].size     = config.getSet( "ui.cursor.text.size", 32 );
    cursors[TEXT].hotspotX = config.getSet( "ui.cursor.text.hotspot.x", 16 );
    cursors[TEXT].hotspotY = config.getSet( "ui.cursor.text.hotspot.y", 16 );

    const char* hand = config.getSet( "ui.cursor.hand", "cursors/hand2.png" );
    cursors[HAND].texId    = context.loadTexture( hand, false, GL_NEAREST, GL_NEAREST );
    cursors[HAND].size     = config.getSet( "ui.cursor.hand.size", 32 );
    cursors[HAND].hotspotX = config.getSet( "ui.cursor.hand.hotspot.x", 16 );
    cursors[HAND].hotspotY = config.getSet( "ui.cursor.hand.hotspot.y", 16 );
  }

  void Mouse::free()
  {
    context.freeTexture( cursors[X].texId );
    context.freeTexture( cursors[ARROW].texId );
    context.freeTexture( cursors[MOVE].texId );
    context.freeTexture( cursors[TEXT].texId );
  }

  void Mouse::prepare()
  {
    relX = 0;
    relY = 0;

    oldButtons = buttons;
    buttons = currButtons;
  }

  void Mouse::update()
  {
    if( doShow ) {
      type = ARROW;

      int desiredX = x + relX;
      int desiredY = y + relY;

      x = bound( desiredX, 0, camera.width - 1 );
      y = bound( desiredY, 0, camera.height - 1 );

      overEdgeX = x != desiredX ? desiredX - x : 0;
      overEdgeY = y != desiredY ? desiredY - y : 0;
    }
    else {
      x = camera.centreX;
      y = camera.centreY;

      overEdgeX = relX;
      overEdgeY = relY;
    }

    leftClick   = ( buttons & ~oldButtons & SDL_BUTTON_LMASK ) != 0;
    middleClick = ( buttons & ~oldButtons & SDL_BUTTON_MMASK ) != 0;
    rightClick  = ( buttons & ~oldButtons & SDL_BUTTON_RMASK ) != 0;
    wheelUp     = ( buttons & ~oldButtons & SDL_BUTTON_WUMASK ) != 0;
    wheelDown   = ( buttons & ~oldButtons & SDL_BUTTON_WDMASK ) != 0;
  }

  void Mouse::draw() const
  {
    const Cursor& cur = cursors[type];

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
