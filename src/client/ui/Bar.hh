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
 * @file client/ui/Bar.hh
 */

#pragma once

#include <client/ui/Area.hh>
#include <client/ui/Style.hh>

namespace oz::client::ui
{

class Bar
{
private:

  const Style::Bar* style;

public:

  explicit Bar(const Style::Bar* style = nullptr);

  void setStyle(const Style::Bar* style);

  void draw(const Area* area, float ratio) const;
  void draw(const Area* area, int barX, int barY, int barWidth, int barHeight, float ratio) const;
  void drawAbs(int barX, int barY, int barWidth, int barHeight, float ratio) const;

};

}
