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

#include <client/ui/Frame.hh>

#include <client/Input.hh>
#include <client/Shape.hh>
#include <client/ui/Mouse.hh>
#include <client/ui/Style.hh>

namespace oz::client::ui
{

bool Frame::onMouseEvent()
{
  if (input.buttons != 0) {
    raise();
  }

  if (input.keys[Input::KEY_UI_ALT]) {
    mouse.icon = Mouse::OPENHAND;

    if (input.buttons & Input::LEFT_BUTTON) {
      if (input.leftPressed) {
        flags |= GRAB_BIT;
      }
      if (flags & GRAB_BIT) {
        mouse.icon = Mouse::CLOSEDHAND;
        move(mouse.dx, mouse.dy);
      }
    }
    else {
      flags &= ~GRAB_BIT;
    }
  }
  else {
    passMouseEvents();
  }

  return true;
}

bool Frame::onKeyEvent()
{
  return passKeyEvents();
}

void Frame::onDraw()
{
  shape.colour(style.colours.frame);
  shape.fill(x, y, width, height);

  title.draw(this);

  drawChildren();
}

Frame::Frame(int width_, int height_, const char* text)
  : Area(width_, height_ + HEADER_SIZE),
    title(width_ / 2, -HEADER_SIZE / 2, 0, ALIGN_HCENTRE | ALIGN_VCENTRE, &style.largeFont,
          "%s", text)
{}

}
