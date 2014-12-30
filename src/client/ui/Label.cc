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
 * @file client/ui/Label.cc
 */

#include <client/ui/Label.hh>

#include <client/Shader.hh>
#include <client/Shape.hh>
#include <client/ui/Area.hh>
#include <client/ui/Style.hh>

#include <cstdio>

namespace oz
{
namespace client
{
namespace ui
{

static const int EMPTY_HASH = hash("");

void Label::realign()
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

void Label::setTextv(const char* s, va_list ap)
{
  hard_assert(s != nullptr);

  char buffer[1024];
  vsnprintf(buffer, 1024, s, ap);
  buffer[1023] = '\0';

  if (buffer[0] == '\0' || (buffer[0] == ' ' && buffer[1] == '\0')) {
    clear();
  }
  else {
    int newHash = hash(buffer);

    if (newHash != lastHash) {
      lastHash = newHash;

      MainCall() << [&]
      {
        if (texId == 0) {
          glGenTextures(1, &texId);
        }

        glBindTexture(GL_TEXTURE_2D, texId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        texWidth = width;
        style.fonts[font].upload(buffer, &texWidth, &texHeight);

        glBindTexture(GL_TEXTURE_2D, shader.defaultTexture);

        realign();
      };
    }
  }
}

Label::Label() :
  x(0), y(0), width(0), align(Area::ALIGN_NONE), font(Font::MONO), lastHash(EMPTY_HASH),
  texX(0), texY(0), texWidth(0), texHeight(0), texId(0)
{}

Label::Label(int x_, int y_, int width_, int align_, Font::Type font_, const char* s, ...) :
  x(x_), y(y_), width(width_), align(align_), font(font_), lastHash(EMPTY_HASH),
  texX(0), texY(0), texWidth(0), texHeight(0), texId(0)
{
  va_list ap;
  va_start(ap, s);
  setTextv(s, ap);
  va_end(ap);
}

Label::~Label()
{
  clear();
}

Label::Label(Label&& l) :
  x(l.x), y(l.y), width(l.width), align(l.align), font(l.font), lastHash(l.lastHash),
  texX(l.texX), texY(l.texY), texWidth(l.texWidth), texHeight(l.texHeight), texId(l.texId)
{
  l.x         = 0;
  l.y         = 0;
  l.width     = 0;
  l.align     = Area::ALIGN_NONE;
  l.font      = Font::MONO;
  l.lastHash  = EMPTY_HASH;
  l.texX      = 0;
  l.texY      = 0;
  l.texWidth  = 0;
  l.texHeight = 0;
  l.texId     = 0;
}

Label& Label::operator = (Label&& l)
{
  if (&l == this) {
    return *this;
  }

  clear();

  x         = l.x;
  y         = l.y;
  width     = l.width;
  align     = l.align;
  font      = l.font;
  lastHash  = l.lastHash;
  texX      = l.texX;
  texY      = l.texY;
  texWidth  = l.texWidth;
  texHeight = l.texHeight;
  texId     = l.texId;

  l.x         = 0;
  l.y         = 0;
  l.width     = 0;
  l.align     = Area::ALIGN_NONE;
  l.font      = Font::MONO;
  l.lastHash  = EMPTY_HASH;
  l.texX      = 0;
  l.texY      = 0;
  l.texWidth  = 0;
  l.texHeight = 0;
  l.texId     = 0;

  return *this;
}

void Label::setPosition(int x_, int y_)
{
  x = x_;
  y = y_;

  realign();
}

void Label::setWidth(int width_)
{
  width = width_;

  realign();
}

void Label::setAlign(int align_)
{
  align = align_;

  realign();
}

void Label::setFont(Font::Type font_)
{
  font = font_;
}

void Label::setText(const char* s, ...)
{
  va_list ap;
  va_start(ap, s);
  setTextv(s, ap);
  va_end(ap);
}

void Label::draw(const Area* area)
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

void Label::clear()
{
  if (texId != 0) {
    MainCall() << [&]
    {
      glDeleteTextures(1, &texId);
    };

    lastHash  = EMPTY_HASH;
    texX      = x;
    texY      = y;
    texWidth  = 0;
    texHeight = 0;
    texId     = 0;
  }
}

}
}
}
