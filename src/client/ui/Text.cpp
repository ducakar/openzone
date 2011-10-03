/*
 *  Text.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/ui/Text.hpp"

#include "client/Shader.hpp"
#include "client/Shape.hpp"
#include "client/ui/Area.hpp"

#include "client/OpenGL.hpp"

#include <SDL/SDL_ttf.h>

#ifdef OZ_MINGW
static char* strchrnul( const char* s, int c )
{
  char* p = strchr( s, c );
  return p == null ? const_cast<char*>( s ) + strlen( s ) : p;
}
#endif

namespace oz
{
namespace client
{
namespace ui
{

  char Text::buffer[2048];

  Text::Text( int x_, int y_, int width_, int nLines_, Font::Type font_ ) :
      x( x_ ), y( y_ ), width( width_ ), nLines( nLines_ ), font( ui::font.fonts[font_] )
  {
    labels = new Label[nLines];

    for( int i = 0; i < nLines; ++i ) {
      labels[i].set( x, y + ( nLines - i - 1 ) * Font::INFOS[font_].height,
                     Area::ALIGN_NONE, font_, "" );
    }
  }

  Text::~Text()
  {
    delete[] labels;
  }

  void Text::setText( const char* s, ... )
  {
    va_list ap;
    va_start( ap, s );
    vsnprintf( buffer, 2048, s, ap );
    va_end( ap );

    int line = 0;

    char* pos = buffer;
    char* end = min( strchrnul( buffer, ' ' ), strchrnul( buffer, '\n' ) );

    while( *end != '\0' && line < nLines - 1 ) {
      char* next;

      while( *end == ' ' ) {
        next = min( strchrnul( end + 1, ' ' ), strchrnul( end + 1, '\n' ) );

        char ch = *next;
        *next = '\0';

        int w;
        TTF_SizeUTF8( font, pos, &w, null );

        *next = ch;

        if( w > width ) {
          break;
        }

        end = next;
      }

      if( *end == '\0' ) {
        break;
      }

      char ch = *end;
      *end = '\0';

      labels[line].setText( pos );

      *end = ch;

      pos = end + 1;
      end = min( strchrnul( pos, ' ' ), strchrnul( pos, '\n' ) );

      ++line;
    }

    labels[line].setText( pos );
    ++line;

    while( line < nLines ) {
      labels[line].setText( "" );
      ++line;
    }
  }

  void Text::clear()
  {
    for( int i = 0; i < nLines; ++i ) {
      labels[i].setText( "" );
    }
  }

  void Text::draw( const Area* area ) const
  {
    for( int i = 0; i < nLines; ++i ) {
      labels[i].draw( area );
    }
  }

}
}
}
