/*
 *  Area.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "ui/Font.hpp"
#include "ui/Mouse.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  class Area
  {
    friend class DList<Area>;
    friend class UI;

    protected:

      static const SDL_Color SDL_COLOR_WHITE;

      static Vector<Area*> updateAreas;

      Area* prev[1];
      Area* next[1];

      TTF_Font* currentFont;
      SDL_Color fontColor;

      DList<Area> children;

    protected:

      static const int UPDATE_FUNC_BIT = 0x00000001;
      // if onUpdate should be called
      static const int UPDATE_BIT      = 0x00000002;
      // ignore when passing events
      static const int IGNORE_BIT      = 0x00000004;
      // a child with GRAB_BIT get exclusive focus for events
      static const int GRAB_BIT        = 0x00000008;
      // do not draw
      static const int HIDDEN_BIT      = 0x00000010;
      // element has onUpdate handler implemented

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

      Area* parent;

      void setFont( FontType type );
      void setFontColor( ubyte r, ubyte g, ubyte b );

      void fill( int x, int y, int width, int height ) const;
      void rect( int x, int y, int width, int height ) const;
      void print( int x, int y, const char* s, ... );
      void printCentered( int baseX, int baseY, const char* s, ... );
      void printBaseline( int baseX, int baseY, const char* s, ... );

      void realign( int newX, int newY )
      {
        int dx = newX - x;
        int dy = newY - y;

        x = newX;
        y = newY;

        foreach( child, children.iter() ) {
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

        foreach( child, children.iter() ) {
          child->move( moveX, moveY );
        }
      }

      // return true if event has been caught
      bool passMouseEvents();

      static void update();

      void drawChildren()
      {
        // render in opposite order; last added child (the first one in the list) should be rendered
        // last
        for( Area* child = children.last(); child != null; child = child->prev[0] ) {
          if( ~child->flags & HIDDEN_BIT ) {
            child->onDraw();
          }
        }
      }

      // return true if event has been caught
      virtual bool onMouseEvent();
      virtual void onUpdate();
      virtual void onDraw();

      explicit Area( int width_, int height_ ) :
        currentFont( null ), x( 0 ), y( 0 ), width( width_ ), height( height_ ), flags( 0 ),
        textWidth( 0 ), textHeight( font.monoHeight ), parent( null )
      {}

      explicit Area( int x_, int y_, int width_, int height_ ) :
          currentFont( null ), x( x_ ), y( y_ ), width( width_ ), height( height_ ), flags( 0 ),
          textWidth( 0 ), textHeight( font.monoHeight ), parent( null )
      {}

      virtual ~Area();

    public:

      void show( bool doShow )
      {
        if( doShow ) {
          flags &= ~( IGNORE_BIT | HIDDEN_BIT );

          if( flags & UPDATE_FUNC_BIT ) {
            flags |= UPDATE_BIT;
          }
        }
        else {
          flags |= IGNORE_BIT | HIDDEN_BIT;
          flags &= ~UPDATE_BIT;
        }
      }

      void add( Area* area, int relativeX, int relativeY )
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
        if( area->flags & UPDATE_FUNC_BIT ) {
          updateAreas << area;
        }
      }

      void add( Area* area )
      {
        add( area, area->x, area->y );
      }

      void remove( Area* area )
      {
        if( area->flags & UPDATE_FUNC_BIT ) {
          updateAreas.exclude( area );
        }
        children.remove( area );
        delete area;
      }

  };

}
}
}
