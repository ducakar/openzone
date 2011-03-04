/*
 *  Mouse.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "ui/Mouse.hpp"

#include "client/Camera.hpp"
#include "client/Context.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  Mouse mouse;

  void Mouse::prepare()
  {
    relX = 0;
    relY = 0;
    relZ = 0;

    oldButtons = buttons;
    buttons = currButtons;
  }

  void Mouse::update()
  {
    if( doShow ) {
      icon = ARROW;

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
    const Cursor& cur = cursors[icon];

    if( doShow ) {
      glUniform1i( param.oz_IsTextureEnabled, true );
      glBindTexture( cur.texId );
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
      glUniform1i( param.oz_IsTextureEnabled, false );
    }
  }

  void Mouse::load()
  {
    const char* x = config.getSet( "ui.cursor.x", "ui/X_cursor.ozcTex" );
    cursors[X].texId    = context.loadTexture( x );
    cursors[X].size     = config.getSet( "ui.cursor.x.size", 32 );
    cursors[X].hotspotX = config.getSet( "ui.cursor.x.hotspot.x", 16 );
    cursors[X].hotspotY = config.getSet( "ui.cursor.x.hotspot.y", 16 );

    const char* arrow = config.getSet( "ui.cursor.arrow", "ui/left_ptr.ozcTex" );
    cursors[ARROW].texId    = context.loadTexture( arrow );
    cursors[ARROW].size     = config.getSet( "ui.cursor.arrow.size", 32 );
    cursors[ARROW].hotspotX = config.getSet( "ui.cursor.arrow.hotspot.x", 1 );
    cursors[ARROW].hotspotY = config.getSet( "ui.cursor.arrow.hotspot.y", 1 );

    const char* move = config.getSet( "ui.cursor.move", "ui/fleur.ozcTex" );
    cursors[MOVE].texId    = context.loadTexture( move );
    cursors[MOVE].size     = config.getSet( "ui.cursor.move.size", 32 );
    cursors[MOVE].hotspotX = config.getSet( "ui.cursor.move.hotspot.x", 16 );
    cursors[MOVE].hotspotY = config.getSet( "ui.cursor.move.hotspot.y", 16 );

    const char* text = config.getSet( "ui.cursor.text", "ui/xterm.ozcTex" );
    cursors[TEXT].texId    = context.loadTexture( text );
    cursors[TEXT].size     = config.getSet( "ui.cursor.text.size", 32 );
    cursors[TEXT].hotspotX = config.getSet( "ui.cursor.text.hotspot.x", 16 );
    cursors[TEXT].hotspotY = config.getSet( "ui.cursor.text.hotspot.y", 16 );

    const char* hand = config.getSet( "ui.cursor.hand", "ui/hand2.ozcTex" );
    cursors[HAND].texId    = context.loadTexture( hand );
    cursors[HAND].size     = config.getSet( "ui.cursor.hand.size", 32 );
    cursors[HAND].hotspotX = config.getSet( "ui.cursor.hand.hotspot.x", 16 );
    cursors[HAND].hotspotY = config.getSet( "ui.cursor.hand.hotspot.y", 16 );
  }

  void Mouse::unload()
  {
    glDeleteTextures( 1, &cursors[X].texId );
    glDeleteTextures( 1, &cursors[ARROW].texId );
    glDeleteTextures( 1, &cursors[MOVE].texId );
    glDeleteTextures( 1, &cursors[TEXT].texId );
  }

  void Mouse::init()
  {
    icon = ARROW;
    doShow = false;

    x = camera.centreX;
    y = camera.centreY;
    relX = 0;
    relY = 0;

    buttons = 0;
    oldButtons = 0;
    currButtons = 0;
  }

  void Mouse::free()
  {}

}
}
}
