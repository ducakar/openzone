/*
 *  Frame.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Area.h"

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
    else {
      currentFont = font.sansFont;
      textHeight  = font.sansHeight;
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

    glBegin( GL_QUADS );
      glVertex2i( x        , y          );
      glVertex2i( x + width, y          );
      glVertex2i( x + width, y + height );
      glVertex2i( x        , y + height );
    glEnd();
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
    uint *pixels = (uint*) text->pixels;
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

  void Area::onClick( int, int )
  {}

  void Area::draw()
  {
    drawChildren();
  }

}
}
}
