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
 * @file client/ui/Label.hh
 */

#pragma once

#include <client/ui/Text.hh>
#include <client/ui/Area.hh>

namespace oz
{
namespace client
{
namespace ui
{

class Label : public Area
{
private:

  Text text;

protected:

  void onDraw() override;

public:

  OZ_PRINTF_FORMAT(6, 7)
  explicit Label(int width, int height, int align, Font* font, const char* s, ...);

  void setWidth(int width);
  void setAlign(int align);
  void setFont(Font* font);
  OZ_PRINTF_FORMAT(2, 3)
  void setText(const char* s, ...);

  void clear();

};

}
}
}
