/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2019 Davorin Učakar
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

#include <client/ui/Text.hh>

#include <client/Shader.hh>
#include <client/Shape.hh>
#include <client/ui/Area.hh>
#include <client/ui/Style.hh>

namespace oz::client::ui
{

void Text::realign()
{
  texX = x;
  texY = y;

  if (align & Area::ALIGN_RIGHT) {
    texX -= texWidth;
  }
  else if (align & Area::ALIGN_HCENTRE) {
    texX -= texWidth / 2;
  }
  if (align & Area::ALIGN_TOP) {
    texY -= texHeight;
  }
  else if (align & Area::ALIGN_VCENTRE) {
    texY -= texHeight / 2;
  }
}

Text::~Text()
{
  clear();
}

Text::Text(int x_, int y_, int width_, int align_, Font* font_, const char* s, ...)
  : x(x_), y(y_), width(width_), align(align_), font(font_)
{
  va_list ap;
  va_start(ap, s);
  setTextv(s, ap);
  va_end(ap);
}

void Text::setPosition(int x_, int y_)
{
  x = x_;
  y = y_;

  realign();
}

void Text::setWidth(int width_)
{
  width    = width_;
  lastHash = Hash<const char*>::EMPTY;

  realign();
}

void Text::setAlign(int align_)
{
  align    = align_;
  lastHash = Hash<const char*>::EMPTY;

  realign();
}

void Text::setFont(Font* font_)
{
  font = font_;
}

void Text::setTextv(const char* s, va_list ap)
{
  OZ_ASSERT(s != nullptr);

  char buffer[1024];
  vsnprintf(buffer, 1024, s, ap);
  buffer[1023] = '\0';

  if (buffer[0] == '\0') {
    clear();
  }
  else {
    uint newHash = hash(buffer);

    if (newHash != lastHash) {
      lastHash = newHash;
      texWidth = width;

      MainCall() << [&]
      {
        if (texId == 0) {
          glGenTextures(1, &texId);
        }

        glBindTexture(GL_TEXTURE_2D, texId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        font->upload(buffer, &texWidth, &texHeight);

        glBindTexture(GL_TEXTURE_2D, shader.defaultTexture);

        realign();
      };
    }
  }
}

void Text::setText(const char* s, ...)
{
  va_list ap;
  va_start(ap, s);
  setTextv(s, ap);
  va_end(ap);
}

void Text::draw(const Area* area)
{
  if (texId == 0) {
    return;
  }

  int posX = area->x + (x < 0 ? area->width  + texX : texX);
  int posY = area->y + (y < 0 ? area->height + texY : texY);

  glBindTexture(GL_TEXTURE_2D, texId);

  shape.colour(style.colours.textBackground);
  shape.fill(posX + 1, posY - 1, texWidth, texHeight);
  shape.colour(style.colours.text);
  shape.fill(posX, posY, texWidth, texHeight);

  glBindTexture(GL_TEXTURE_2D, shader.defaultTexture);
}

void Text::clear()
{
  if (texId != 0) {
    MainCall() << [&]
    {
      glDeleteTextures(1, &texId);
    };

    lastHash  = Hash<const char*>::EMPTY;
    texX      = x;
    texY      = y;
    texWidth  = 0;
    texHeight = 0;
    texId     = 0;
  }
}

}
