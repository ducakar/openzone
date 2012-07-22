/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 */

/**
 * @file client/ui/Mouse.hh
 */

#pragma once

#include "client/common.hh"

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
      ARROW,
      TEXT,
      HAND,
      OPENHAND,
      CLOSEDHAND,
      SCROLL,
      CURSORS_MAX
    };

    static const char* const NAMES[CURSORS_MAX];

  private:

    struct Cursor
    {
      int  size;
      int  hotspotX;
      int  hotspotY;
      uint texId;
    };

    Cursor cursors[CURSORS_MAX];

  public:

    int  x;
    int  y;

    int  overEdgeX;
    int  overEdgeY;

    Icon icon;
    bool doShow;

    void update();

    void draw() const;

    void init();
    void free();

};

extern Mouse mouse;

}
}
}
