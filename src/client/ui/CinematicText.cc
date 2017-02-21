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

#include <client/ui/CinematicText.hh>

#include <client/ui/Style.hh>
#include <client/Camera.hh>

namespace oz
{
namespace client
{
namespace ui
{

void CinematicText::onRealign()
{
  x     = camera.width / 8;
  y     = camera.height / 12;
  width = (camera.width * 3) / 4;

  text_.setWidth(width);
}

void CinematicText::onDraw()
{
  text_.draw(this);
}

CinematicText::CinematicText()
  : Area(600, 400), text_(0, 0, 600, Area::ALIGN_NONE, &style.titleFont, "")
{
  flags |= PINNED_BIT;
}

void CinematicText::set(const char* text)
{
  text_.setText("%s", text);
}

}
}
}
