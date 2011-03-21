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

      static const SDL_Color SDL_COLOUR_WHITE;

      static Vector<Area*> updateAreas;

      Area* prev[1];
      Area* next[1];
      Area* parent;

      DList<Area> children;

      // absolute x and y, not relative to parent
      int x;
      int y;
      int width;
      int height;

      int flags;

      TTF_Font* currentFont;

      // width of the last printed text
      int textWidth;
      // height of the current font
      int textHeight;

      explicit Area( int width, int height );
      explicit Area( int x, int y, int width, int height );
      virtual ~Area();

      void setFont( Font::Type type );

      void fill( int x, int y, int width, int height ) const;
      void rect( int x, int y, int width, int height ) const;
      void print( int x, int y, const char* s, ... );
      void printCentred( int baseX, int baseY, const char* s, ... );
      void printBaseline( int baseX, int baseY, const char* s, ... );

      void realign( int newX, int newY );
      void move( int moveX, int moveY );

      // return true if event has been caught
      bool passMouseEvents();
      void drawChildren();

      // return true if event has been caught
      virtual bool onMouseEvent();
      virtual void onUpdate();
      virtual void onDraw();

      static void update();

    public:

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
