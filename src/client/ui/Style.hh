/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file client/ui/Style.hh
 */

#pragma once

#include <client/ui/Font.hh>

namespace oz
{
namespace client
{
namespace ui
{

class Style
{
  public:

    struct Colours
    {
      Vec4 text;
      Vec4 textBackground;

      Vec4 button;
      Vec4 buttonHover;
      Vec4 buttonClicked;

      Vec4 tile;
      Vec4 tileHover;

      Vec4 frame;
      Vec4 background;

      Vec4 barBorder;
      Vec4 healthMin;
      Vec4 healthMax;
      Vec4 staminaMin;
      Vec4 staminaMax;
      Vec4 hullMin;
      Vec4 hullMax;
      Vec4 fuelMin;
      Vec4 fuelMax;

      Vec4 galileoNormal;
      Vec4 galileoMaximised;

      Vec4 menuStrip;
    };

    struct Bar
    {
      int x;
      int y;
      int w;
      int h;
    };

    struct Layout
    {
      int iconSize;

      Bar botHealth;
      Bar botStamina;
      Bar botWeapon;

      Bar vehicleHull;
      Bar vehicleFuel;
      Bar vehicleWeapon[4];
    };

    Font    fonts[Font::MAX];
    Colours colours;
    Layout  layout;

  public:

    void init();
    void free();

};

extern Style style;

}
}
}
