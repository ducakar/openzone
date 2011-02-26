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

#define SDL_BUTTON_WUMASK SDL_BUTTON( SDL_BUTTON_WHEELUP )
#define SDL_BUTTON_WDMASK SDL_BUTTON( SDL_BUTTON_WHEELDOWN )

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
      int  relZ;

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

      void prepare();
      void update();

      void draw() const;

      void load();
      void unload();

      void init();
      void free();

  };

  extern Mouse mouse;

}
}
}
