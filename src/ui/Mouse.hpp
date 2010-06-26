/*
 *  Mouse.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

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

    public:

      int    x;
      int    y;
      int    relX;
      int    relY;

      ubyte  buttons;
      ubyte  oldButtons;
      ubyte  currButtons;

      int    overEdgeX;
      int    overEdgeY;

      bool   leftClick;
      bool   rightClick;
      bool   middleClick;
      bool   wheelUp;
      bool   wheelDown;

      int    type;
      bool   doShow;

      void   init();
      void   free();

      void   show();
      void   hide();

      void   prepare();
      void   update();

      void   draw() const;

  };

  extern Mouse mouse;

}
}
}
