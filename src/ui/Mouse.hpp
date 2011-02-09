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

      struct Icon
      {
        static const int X     = 0;
        static const int ARROW = 1;
        static const int MOVE  = 2;
        static const int TEXT  = 3;
        static const int HAND  = 4;
        static const int MAX   = 5;
      };

    private:

      struct Cursor
      {
        uint texId;
        int  size;
        int  hotspotX;
        int  hotspotY;
      };

      Cursor cursors[Icon::MAX];

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

      void show();
      void hide();

      void prepare();
      void update();

      void draw() const;

  };

  extern Mouse mouse;

}
}
}
