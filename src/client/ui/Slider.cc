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

#include <client/ui/Slider.hh>

#include <client/Input.hh>
#include <client/Shape.hh>
#include <client/Context.hh>
#include <client/ui/Style.hh>
#include <client/ui/Mouse.hh>

namespace oz::client::ui
{

void Slider::onVisibilityChange(bool)
{
  isHighlighted = false;
  isClicked     = false;
  wasClicked    = false;
}

bool Slider::onMouseEvent()
{
  if (input.keys[Input::KEY_UI_ALT]) {
    return false;
  }

  isHighlighted = true;
  isClicked     = wasClicked && (input.buttons & Input::LEFT_BUTTON);

  if (isClicked) {
    float fineValue = float(mouse.x - x - 4) / float(width - 8);
    fineValue = minValue + valueStep / 2.0f + fineValue * (maxValue - minValue);
    fineValue = clamp(fineValue, minValue, maxValue);

    value = fineValue - Math::mod(fineValue, valueStep);

    text.setText("%g", value);
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

void Slider::onDraw()
{
  int barWidth = Math::lround(float(width - 8) * (value - minValue) / (maxValue - minValue));

  if (isClicked) {
    shape.colour(style.colours.sliderClicked);
  }
  else if (isHighlighted) {
    shape.colour(style.colours.sliderHover);
    wasClicked = false;
  }
  else {
    shape.colour(style.colours.slider);
    wasClicked = false;
  }
  shape.fill(x + 5, y + 1, barWidth, height - 2);

  shape.colour(style.colours.text);
  shape.rect(x + 4, y, width - 8, height);

  text.draw(this);

  isHighlighted = false;
  isClicked     = false;
}

Slider::Slider(float min, float max, float step, float value_, int width, int height)
  : Area(width, height),
    text(width / 2, height / 2, 0, ALIGN_CENTRE, &style.sansFont, "%g", value_),
    isHighlighted(false), isClicked(false), wasClicked(false),
    minValue(min), maxValue(max), valueStep(step), value(value_)
{}

}
