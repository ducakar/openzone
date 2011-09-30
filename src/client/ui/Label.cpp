/*
 *  Frame.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/ui/Label.hpp"

#include "client/Shader.hpp"
#include "client/Shape.hpp"
#include "client/ui/Area.hpp"

#include "client/OpenGL.hpp"

#include <SDL/SDL_ttf.h>

namespace oz
{
namespace client
{
namespace ui
{

  Label::Label() : x( 0 ), y( 0 ),align( Area::ALIGN_NONE ), font( Font::MONO ),
      offsetX( 0 ), offsetY( 0 ), width( 0 ), height( 0 ), activeTexId( 0 )
  {
    glGenTextures( 1, &texId );
    glBindTexture( GL_TEXTURE_2D, texId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  }

  Label::~Label()
  {
    glDeleteTextures( 1, &texId );
  }

  Label::Label( int x, int y, int align, Font::Type font, const char* s, ... )
  {
    glGenTextures( 1, &texId );
    glBindTexture( GL_TEXTURE_2D, texId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    va_list ap;
    va_start( ap, s );
    vset( x, y, align, font, s, ap );
    va_end( ap );
  }

  void Label::vset( int x_, int y_, int align_, Font::Type font_, const char* s, va_list ap )
  {
    hard_assert( s != null );

    x     = x_;
    y     = y_;
    align = align_;
    font  = font_;

    char buffer[1024];
    vsnprintf( buffer, 1024, s, ap );
    buffer[1023] = '\0';

    if( buffer[0] == '\0' ) {
      offsetX = 0;
      offsetY = 0;
      width   = 0;
      height  = 0;

      activeTexId = 0;
      return;
    }

    SDL_Surface* text = TTF_RenderUTF8_Blended( ui::font.fonts[font], buffer, Font::SDL_COLOUR_WHITE );

    glBindTexture( GL_TEXTURE_2D, texId );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, text->w, text->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                  text->pixels );
    glBindTexture( GL_TEXTURE_2D, 0 );

    activeTexId = texId;

    offsetX = x;
    offsetY = y;
    width   = text->w;
    height  = text->h;

    SDL_FreeSurface( text );

    if( align & Area::ALIGN_RIGHT ) {
      offsetX -= width;
    }
    else if( align & Area::ALIGN_HCENTRE ) {
      offsetX -= width / 2;
    }
    if( align & Area::ALIGN_TOP ) {
      offsetY -= height;
    }
    else if( align & Area::ALIGN_VCENTRE ) {
      offsetY -= height / 2;
    }
  }

  void Label::set( int x, int y, int align, Font::Type font, const char* s, ... )
  {
    va_list ap;
    va_start( ap, s );
    vset( x, y, align, font, s, ap );
    va_end( ap );
  }

  void Label::setText( const char* s, ... )
  {
    va_list ap;
    va_start( ap, s );
    vset( x, y, align, font, s, ap );
    va_end( ap );
  }

  void Label::draw( const Area* area ) const
  {
    glBindTexture( GL_TEXTURE_2D, width == 0 ? 0 : texId );

    int posX = area->x + ( x < 0 ? area->width  + offsetX : offsetX );
    int posY = area->y + ( y < 0 ? area->height + offsetY : offsetY );

    glUniform4f( param.oz_Colour, 0.0f, 0.0f, 0.0f, 1.0f );
    shape.fill( posX + 1, posY - 1, width, height );
    glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 1.0f );
    shape.fill( posX, posY, width, height );

    glBindTexture( GL_TEXTURE_2D, 0 );
  }

}
}
}
