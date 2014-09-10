/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file client/ui/GalileoFrame.hh
 */

#pragma once

#include <client/ui/Frame.hh>

namespace oz
{
namespace client
{
namespace ui
{

class GalileoFrame : public Frame
{
private:

  uint      mapTexId;

  int       normalX;
  int       normalY;
  int       normalWidth;
  int       normalHeight;

  int       maximisedX;
  int       maximisedY;
  int       maximisedWidth;
  int       maximisedHeight;

  Vec4      colour;

public:

  // World coordinate of a mouse click, NaN if no click.
  float     clickX;
  float     clickY;

  bool      isMaximised;

protected:

  void onReposition() override;
  void onUpdate() override;
  bool onMouseEvent() override;
  void onDraw() override;

public:

  GalileoFrame();
  ~GalileoFrame();

  void setMaximised(bool doMaximise);

};

}
}
}
