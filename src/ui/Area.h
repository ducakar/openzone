/*
 *  Area.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "Font.h"
#include "Mouse.h"

namespace oz
{
namespace client
{
namespace ui
{

  class Area
  {
    friend class DList<Area, 0>;
    friend class UI;

    protected:

      static const SDL_Color SDL_COLOR_WHITE;

      Area  *prev[1];
      Area  *next[1];

      TTF_Font  *currentFont;
      SDL_Color fontColor;

      DList<Area, 0> children;

    protected:

      static const int GRAB_BIT = 0x00000001;

      // absolute x and y, not relative to parent
      int x;
      int y;
      int width;
      int height;

      int flags;

      // width of the last printed text
      int textWidth;
      // height of the current font
      int textHeight;

      Area *parent;

      void setFont( FontType type );
      void setFontColor( ubyte r, ubyte g, ubyte b );

      void fill( int x, int y, int width, int height ) const;
      void rect( int x, int y, int width, int height ) const;
      void print( int x, int y, const char *s, ... );
      void printCentered( int baseX, int baseY, const char *s, ... );
      void printBaseline( int baseX, int baseY, const char *s, ... );

      void realign( int newX, int newY )
      {
        int dx = newX - x;
        int dy = newY - y;

        x = newX;
        y = newY;

        foreach( child, children.iterator() ) {
          child->x += dx;
          child->y += dy;
        }
      }

      void move( int moveX, int moveY )
      {
        moveX = bound( moveX, parent->x - x, parent->x + parent->width  - x - width  );
        moveY = bound( moveY, parent->y - y, parent->y + parent->height - y - height );

        x += moveX;
        y += moveY;

        foreach( child, children.iterator() ) {
          child->move( moveX, moveY );
        }
      }

      void checkMouse();

      void drawChildren()
      {
        // render in opposite order; last added child (the first one in the list) should be rendered
        // last
        for( Area *child = children.last(); child != null; child = child->prev[0] ) {
          child->onDraw();
        }
      }

      virtual void onMouseEvent();
      virtual void onDraw();

    public:

      explicit Area( int width_, int height_ ) :
        currentFont( null ), x( 0 ), y( 0 ), width( width_ ), height( height_ ), flags( 0 ),
        textWidth( 0 ), textHeight( font.monoHeight ), parent( null )
      {}

      explicit Area( int x_, int y_, int width_, int height_ ) :
          currentFont( null ), x( x_ ), y( y_ ), width( width_ ), height( height_ ), flags( 0 ),
          textWidth( 0 ), textHeight( font.monoHeight ), parent( null )
      {}

      virtual ~Area();

      void add( Area *area, int relativeX, int relativeY )
      {
        area->width  = bound( area->width,  1, width  );
        area->height = bound( area->height, 1, height );

        relativeX = relativeX < 0 ? width  + relativeX : relativeX;
        relativeY = relativeY < 0 ? height + relativeY : relativeY;

        relativeX = bound( relativeX, 0, width  - area->width  );
        relativeY = bound( relativeY, 0, height - area->height );

        area->realign( x + relativeX, y + relativeY );
        area->parent = this;

        children << area;
      }

      void add( Area *area )
      {
        add( area, area->x, area->y );
      }

      void remove( Area *area )
      {
        children.remove( area );
        delete area;
      }

  };

}
}
}
