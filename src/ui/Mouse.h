/*
 *  Mouse.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "ui.h"

namespace oz
{
namespace client
{
namespace ui
{

  class Mouse
  {
    public:

      enum CursorType
      {
        X,
        ARROW,
        MOVE,
        TEXT,
        HAND,
        MAX
      };

    private:

      struct Cursor
      {
        uint texId;
        int  size;
        int  hotspotX;
        int  hotspotY;
      };

      Cursor cursors[MAX];

      int    maxX;
      int    maxY;

    public:

      // read-write
      int    moveX;
      int    moveY;
      byte   persButtons;
      byte   currButtons;

      // read-only
      int    x;
      int    y;
      byte   b;

      int    overEdgeX;
      int    overEdgeY;

      bool   leftClick;
      bool   rightClick;
      bool   middleClick;
      bool   wheelUp;
      bool   wheelDown;

      int    type;
      bool   doShow;

      void   init( int maxX, int maxY );
      void   free();

      void   setBounds( int maxX, int maxY );
      void   show();
      void   hide();

      void   update();
      void   draw() const;

  };

  extern Mouse mouse;

}
}
}
