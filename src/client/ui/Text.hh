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
 * @file client/ui/Label.hh
 */

#pragma once

#include <client/ui/Font.hh>

namespace oz
{
namespace client
{
namespace ui
{

class Area;

class Text
{
private:

  int        x;
  int        y;
  int        width;
  int        align;
  Font::Type font;
  int        lastHash;

  int        texX;
  int        texY;
  int        texWidth;
  int        texHeight;
  uint       texId;

private:

  void realign();

public:

  Text();

  OZ_PRINTF_FORMAT(7, 8)
  explicit Text(int x, int y, int width, int align, Font::Type font, const char* s, ...);

  ~Text();
  Text(Text&& l);
  Text& operator = (Text&& l);

  void setPosition(int x, int y);
  void setWidth(int width);
  void setAlign(int align);
  void setFont(Font::Type font);
  OZ_PRINTF_FORMAT(2, 0)
  void setTextv(const char* s, va_list ap);
  OZ_PRINTF_FORMAT(2, 3)
  void setText(const char* s, ...);

  void draw(const Area* area);

  void clear();

};

}
}
}
