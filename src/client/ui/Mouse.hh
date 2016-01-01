/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2016 Davorin Učakar
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

#include <client/common.hh>

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

  Cursor cursors[CURSORS_MAX];
  Icon   oldIcon;
  bool   wasVisible;

  float  fineX;
  float  fineY;

public:

  int  x;
  int  y;
  int  dx;
  int  dy;

  Icon icon;
  bool isVisible;

  void update();

  void draw();

  void init();
  void destroy();

};

extern Mouse mouse;

}
}
}
