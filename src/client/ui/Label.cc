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

#include <client/ui/Label.hh>

namespace oz::client::ui
{

void Label::onDraw()
{
  text.draw(this);
}

Label::Label(int width, int height, int align, Font* font, const char* s, ...)
  : Area(width, height), text(0, 0, width, align, font, "")
{
  va_list ap;
  va_start(ap, s);
  text.setTextv(s, ap);
  va_end(ap);
}

void Label::setWidth(int width)
{
  text.setWidth(width);
}

void Label::setAlign(int align)
{
  text.setAlign(align);
}

void Label::setFont(Font* font)
{
  text.setFont(font);
}

void Label::setText(const char* s, ...)
{
  va_list ap;
  va_start(ap, s);
  text.setTextv(s, ap);
  va_end(ap);
}

void Label::clear()
{
  text.clear();
}

}
