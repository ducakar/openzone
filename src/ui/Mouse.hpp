/*
 *  Mouse.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  class Mouse
  {
    public:

      enum Icon : int
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

      int  x;
      int  y;
      int  relX;
      int  relY;

      char buttons;
      char oldButtons;
      char currButtons;

      int  overEdgeX;
      int  overEdgeY;

      bool leftClick;
      bool rightClick;
      bool middleClick;
      bool wheelUp;
      bool wheelDown;

      int  icon;
      bool doShow;

      void init();
      void free();

      void prepare();
      void update();

      void draw() const;

  };

  extern Mouse mouse;

}
}
}
