/*
 *  Area.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Font.h"

namespace oz
{
namespace client
{
namespace ui
{

  class Area
  {
    friend class DList<Area, 0>;
    friend void draw();

    private:

      static const SDL_Color SDL_COLOR_WHITE;

      Area  *prev[1];
      Area  *next[1];

      TTF_Font  *currentFont;
      SDL_Color fontColor;

      DList<Area, 0> children;

    protected:

      void drawChildren()
      {
        foreach( childArea, children.iterator() ) {
          childArea->draw();
        }
      }

      int x;
      int y;
      int width;
      int height;

      // width of the last printed text
      int textWidth;
      // height of the current font
      int textHeight;

      void setFont( FontType type );
      void setFontColor( ubyte r, ubyte g, ubyte b );

      void fill( int x, int y, int width, int height ) const;
      void rect( int x, int y, int width, int height ) const;
      void print( int x, int y, const char *s, ... );

    public:

      explicit Area() {}
      explicit Area( int x_, int y_, int width_, int height_ ) :
          x( x_ ), y( y_ ), width( width_ ), height( height_ ),
          textWidth( 0 ), textHeight( font.monoHeight )
      {
      }

      virtual ~Area();

      void add( Area *area )
      {
        area->x = area->x < 0 ? x + width  + area->x : x + area->x;
        area->y = area->y < 0 ? y + height + area->y : y + area->y;
        children << area;
      }

      void remove( Area *area )
      {
        children.remove( area );
        delete area;
      }

      virtual void draw();

  };

}
}
}
