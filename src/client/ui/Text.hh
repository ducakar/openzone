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

#include <client/ui/Area.hh>

namespace oz::client::ui
{

class Text
{
private:

  int   x         = 0;
  int   y         = 0;
  int   width     = 0;
  int   align     = Area::ALIGN_NONE;
  Font* font      = nullptr;
  int   lastHash  = Hash<const char*>::EMPTY;

  int   texX      = 0;
  int   texY      = 0;
  int   texWidth  = 0;
  int   texHeight = 0;
  uint  texId     = 0;

private:

  void realign();

public:

  Text() = default;

  OZ_PRINTF_FORMAT(7, 8)
  explicit Text(int x_, int y_, int width_, int align_, Font* font_, const char* s, ...);

  ~Text();
  Text(Text&& other) noexcept;
  Text& operator=(Text&& other) noexcept;

  void setPosition(int x_, int y_);
  void setWidth(int width_);
  void setAlign(int align_);
  void setFont(Font* font_);
  OZ_PRINTF_FORMAT(2, 0)
  void setTextv(const char* s, va_list ap);
  OZ_PRINTF_FORMAT(2, 3)
  void setText(const char* s, ...);

  void draw(const Area* area);

  void clear();

};

}
