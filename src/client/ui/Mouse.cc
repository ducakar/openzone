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

#include <client/ui/Mouse.hh>

#include <client/Shader.hh>
#include <client/Shape.hh>
#include <client/Camera.hh>
#include <client/Input.hh>

namespace oz::client::ui
{

const char* const Mouse::NAMES[] = {
  "left_ptr",
  "ibeam",
  "pointing_hand",
  "openhand",
  "closedhand",
  "size_all"
};

void Mouse::update()
{
  icon = ARROW;

  if (isVisible) {
    float minX = 0.0f;
    float minY = 0.0f;
    float maxX = float(camera.width) - 1.0f;
    float maxY = float(camera.height) - 1.0f;

    fineX += input.mouseX;
    fineY += input.mouseY;

    if (fineX < minX) {
      input.lookX += (minX - fineX) * input.mouseSensX;
      fineX = minX;
    }
    else if (fineX > maxX) {
      input.lookX -= (fineX - maxX) * input.mouseSensX;
      fineX = maxX;
    }

    if (fineY < minY) {
      input.lookY -= (minY - fineY) * input.mouseSensY;
      fineY = minY;
    }
    else if (fineY > maxY) {
      input.lookY += (fineY - maxY) * input.mouseSensY;
      fineY = maxY;
    }

    int newX = Math::lround(fineX);
    int newY = Math::lround(fineY);

    dx = newX - x;
    dy = newY - y;
    x  = newX;
    y  = newY;
  }
  else {
    input.lookX -= input.mouseX * input.mouseSensX;
    input.lookY += input.mouseY * input.mouseSensY;

    fineX = float(camera.centreX);
    fineY = float(camera.centreY);
    dy    = 0;
    dx    = 0;
    x     = camera.centreX;
    y     = camera.centreY;
  }
}

void Mouse::draw()
{
  Cursor& cursor = cursors[icon];

  if (isVisible) {
    if (icon != oldIcon || !wasVisible) {
      cursor.reset();
      oldIcon = icon;
    }

    shape.colour(1.0f, 1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, cursor.textureId());
    shape.fill(x - cursor.hotspotLeft(), y - cursor.height() + 1 + cursor.hotspotTop(),
               cursor.width(), cursor.height());
    glBindTexture(GL_TEXTURE_2D, shader.defaultTexture);

    cursor.update(timer.frameDuration);
  }

  wasVisible = isVisible;
}

void Mouse::init()
{
  Log::print("Initialising Mouse ...");

  fineX      = float(camera.centreX);
  fineY      = float(camera.centreY);
  x          = camera.centreX;
  y          = camera.centreY;
  dx         = 0;
  dy         = 0;

  icon       = ARROW;
  oldIcon    = ARROW;
  isVisible  = false;
  wasVisible = false;

  for (int i = 0; i < CURSORS_MAX; ++i) {
    File file = String("@ui/cur/", NAMES[i]);

    cursors[i] = Cursor(file, Cursor::TEXTURE);
    if (!cursors[i].isValid()) {
      OZ_ERROR("Cursor loading failed");
    }
  }

  Log::printEnd(" OK");
}

void Mouse::destroy()
{
  Log::print("Destroying Mouse ...");

  for (Cursor& cursor : cursors) {
    cursor.destroy();
  }

  Log::printEnd(" OK");
}

Mouse mouse;

}
