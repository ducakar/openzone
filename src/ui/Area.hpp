/*
 *  Area.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

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

    public:

      // if onUpdate should be called
      static const int UPDATE_BIT      = 0x01;
      // ignore when passing events
      static const int IGNORE_BIT      = 0x02;
      // a child with GRAB_BIT get exclusive focus for events
      static const int GRAB_BIT        = 0x04;
      // do not draw
      static const int HIDDEN_BIT      = 0x08;
      // pin (show in both freelook and interface mode)
      static const int PINNED_BIT      = 0x10;

      // text alignment
      static const int ALIGN_LEFT      = 0x00;
      static const int ALIGN_RIGHT     = 0x01;
      static const int ALIGN_HCENTRE   = 0x02;
      static const int ALIGN_BOTTOM    = 0x00;
      static const int ALIGN_TOP       = 0x04;
      static const int ALIGN_VCENTRE   = 0x08;
      static const int ALIGN_NONE      = ALIGN_LEFT | ALIGN_BOTTOM;
      static const int ALIGN_CENTRE    = ALIGN_HCENTRE | ALIGN_VCENTRE;

    protected:

      static const SDL_Color SDL_COLOUR_WHITE;

      static Vector<Area*> updateAreas;

      Area* prev[1];
      Area* next[1];

      int flags;

      TTF_Font* currentFont;

    public:

      Area* parent;
      DList<Area> children;

      // absolute x and y, not relative to parent
      int x;
      int y;
      int width;
      int height;

      // width of the last printed text
      int textWidth;
      // height of the current font
      int textHeight;

    protected:

      void fill( int x, int y, int width, int height ) const;
      void rect( int x, int y, int width, int height ) const;
      void print( int x, int y, int align, const char* s, ... );

      // return true if event has been caught
      bool passMouseEvents();
      void drawChildren();

      // return true if event has been caught
      virtual bool onMouseEvent();
      virtual void onUpdate();
      virtual void onDraw();

      static void update();

    public:

      explicit Area( int width, int height );
      explicit Area( int x, int y, int width, int height );
      virtual ~Area();

      void setFont( Font::Type type );

      void realign( int newX, int newY );
      void move( int moveX, int moveY );

      void show( bool doShow );

      void add( Area* area, int relativeX, int relativeY );
      void add( Area* area );
      void remove( Area* area );
      // bring to front
      void focus( Area* area );

  };

}
}
}
