/*
 *  Frame.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "Area.h"

#include <GL/gl.h>

namespace oz
{
namespace client
{
namespace ui
{

  const SDL_Color Area::SDL_COLOR_WHITE = { 0xff, 0xff, 0xff, 0xff };

  Area::~Area()
  {
    children.free();
  }

  void Area::setFont( FontType type )
  {
    if( type == MONO ) {
      currentFont = font.monoFont;
      textHeight  = font.monoHeight;
    }
    else if( type == SANS ) {
      currentFont = font.sansFont;
      textHeight  = font.sansHeight;
    }
    else {
      currentFont = font.titleFont;
      textHeight  = font.titleHeight;
    }
  }

  void Area::setFontColor( ubyte r, ubyte g, ubyte b )
  {
    fontColor.r = r;
    fontColor.g = g;
    fontColor.b = b;
  }

  void Area::fill( int x, int y, int width, int height ) const
  {
    x = x < 0 ? this->x + this->width  + x : this->x + x;
    y = y < 0 ? this->y + this->height + y : this->y + y;

    glRecti( x, y, x + width, y + height );
  }

  void Area::rect( int x, int y, int width, int height ) const
  {
    x = x < 0 ? this->x + this->width + x  : this->x + x;
    y = y < 0 ? this->y + this->height + y : this->y + y;

    glBegin( GL_LINE_LOOP );
      glVertex2f( x         + 0.5f, y          + 0.5f );
      glVertex2f( x + width - 0.5f, y          + 0.5f );
      glVertex2f( x + width - 0.5f, y + height - 0.5f );
      glVertex2f( x         + 0.5f, y + height - 0.5f );
    glEnd();
  }

  void Area::print( int x, int y, const char *s, ... )
  {
    char buffer[1024];
    va_list ap;

    va_start( ap, s );
    vsnprintf( buffer, 1024, s, ap );
    va_end( ap );
    buffer[1023] = '\0';

    SDL_Surface *text = TTF_RenderUTF8_Blended( currentFont, buffer, fontColor );

    // flip
    uint *pixels = reinterpret_cast<uint*>( text->pixels );
    for( int i = 0; i < text->h / 2; i++ ) {
      for( int j = 0; j < text->w; j++ ) {
        swap( pixels[i * text->w + j], pixels[( text->h - i - 1 ) * text->w + j] );
      }
    }

    x = x < 0 ? this->x + this->width  + x : this->x + x;
    y = y < 0 ? this->y + this->height + y : this->y + y;

    glRasterPos2i( x, y );
    glDrawPixels( text->w, text->h, GL_RGBA, GL_UNSIGNED_BYTE, text->pixels );

    textWidth = text->w;
    SDL_FreeSurface( text );
  }

  void Area::printCentered( int baseX, int baseY, const char *s, ... )
  {
    char buffer[1024];
    va_list ap;

    va_start( ap, s );
    vsnprintf( buffer, 1024, s, ap );
    va_end( ap );
    buffer[1023] = '\0';

    SDL_Surface *text = TTF_RenderUTF8_Blended( currentFont, buffer, fontColor );

    // flip
    uint *pixels = reinterpret_cast<uint*>( text->pixels );
    for( int i = 0; i < text->h / 2; i++ ) {
      for( int j = 0; j < text->w; j++ ) {
        swap( pixels[i * text->w + j], pixels[( text->h - i - 1 ) * text->w + j] );
      }
    }

    baseX = baseX < 0 ? this->x + this->width  + baseX : this->x + baseX;
    baseY = baseY < 0 ? this->y + this->height + baseY : this->y + baseY;

    glRasterPos2i( baseX - text->w / 2, baseY - text->h / 2 );
    glDrawPixels( text->w, text->h, GL_RGBA, GL_UNSIGNED_BYTE, text->pixels );

    textWidth = text->w;
    SDL_FreeSurface( text );
  }

  void Area::checkMouse()
  {
    if( flags & GRAB_BIT ) {
      onMouseEvent();
      return;
    }
    foreach( child, children.iterator() ) {
      if( child->flags & GRAB_BIT ) {
        child->checkMouse();
        return;
      }
    }
    foreach( child, children.iterator() ) {
      if( child->x <= mouse.x && mouse.x < child->x + child->width &&
          child->y <= mouse.y && mouse.y < child->y + child->height )
      {
        child->checkMouse();
        // If event is passed to a child, we won't handle in on parent. Of course we assume
        // children do not overlap, so event can only be passed to one of them.
        return;
      }
    }
    onMouseEvent();
  }

  void Area::onMouseEvent()
  {}

  void Area::onDraw()
  {
    drawChildren();
  }

}
}
}
