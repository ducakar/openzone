/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file client/ui/Mouse.hh
 */

#pragma once

#include "client/common.hh"

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

    static const char* NAMES[MAX];

  private:

    struct Cursor
    {
      int  size;
      int  hotspotX;
      int  hotspotY;
      uint texId;
    };

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

};

extern Mouse mouse;

}
}
}
