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

#include <client/ui/CheckBox.hh>

#include <client/Input.hh>
#include <client/Shape.hh>
#include <client/Context.hh>
#include <client/ui/Style.hh>

namespace oz
{
namespace client
{
namespace ui
{

void CheckBox::onVisibilityChange(bool)
{
  isHighlighted = false;
  isClicked     = false;
  wasClicked    = false;
}

bool CheckBox::onMouseEvent()
{
  if (input.keys[Input::KEY_UI_ALT]) {
    return false;
  }

  isHighlighted = true;
  isClicked     = wasClicked && (input.buttons & Input::LEFT_BUTTON);

  if (wasClicked && input.leftReleased) {
    isChecked = !isChecked;
  }
  else if (input.leftPressed) {
    isClicked  = true;
    wasClicked = true;

    if (style.sounds.click != -1) {
      context.playSample(style.sounds.click);
    }
  }
  return true;
}

void CheckBox::onDraw()
{
  if (isClicked) {
    shape.colour(style.colours.boxClicked);
  }
  else if (isHighlighted) {
    shape.colour(style.colours.boxHover);
    wasClicked = false;
  }
  else {
    shape.colour(style.colours.box);
    wasClicked = false;
  }

  shape.fill(x, y, width, height);
  label.draw(this);

  shape.colour(style.colours.text);
  shape.rect(x + 4, y + 4, height - 8, height - 8);

  if (isChecked) {
    shape.fill(x + 7, y + 7, height - 14, height - 14);
  }

  isHighlighted = false;
  isClicked     = false;
}

CheckBox::CheckBox(const char* text, int width, int height)
  : Area(width, height),
    label(height + 2, height / 2, 0, ALIGN_VCENTRE, &style.sansFont, "%s", text),
    isHighlighted(false), isClicked(false), wasClicked(false), isChecked(false)
{}

void CheckBox::setLabel(const char* text)
{
  label.setText("%s", text);
}

}
}
}
