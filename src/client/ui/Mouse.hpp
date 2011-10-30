/*
 *  Mouse.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "client/common.hpp"

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

      enum Icon
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
        int  size;
        int  hotspotX;
        int  hotspotY;
        uint texId;
      };

      static const char* NAMES[MAX];

#ifndef OZ_TOOLS

      Cursor cursors[MAX];

    public:

      int  x;
      int  y;

      int   relX;
      int   relY;
      int   relZ;

      char  buttons;
      char  oldButtons;
      char  currButtons;

      int   overEdgeX;
      int   overEdgeY;

      bool  leftClick;
      bool  rightClick;
      bool  middleClick;
      bool  wheelUp;
      bool  wheelDown;

      bool  doShow;
      bool  isGrabOn;
      float accelFactor;

      int   icon;

      void prepare();
      void update();

      void draw() const;

      void load();
      void unload();

      void init();
      void free();

#else

    public:

      static void prebuild();

#endif

  };

  extern Mouse mouse;

}
}
}
