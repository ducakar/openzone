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
    Vec4 textHover;
    Vec4 textClicked;

    Vec4 button;
    Vec4 buttonHover;
    Vec4 buttonClicked;

    Vec4 box;
    Vec4 boxHover;
    Vec4 boxClicked;

    Vec4 slider;
    Vec4 sliderHover;
    Vec4 sliderClicked;

    Vec4 tile;
    Vec4 tileHover;

    Vec4 frame;
    Vec4 background;

    Vec4 galileoNormal;
    Vec4 galileoMaximised;

    Vec4 menuStrip;
  };

  struct Images
  {
    uint crosshair;
    uint use;
    uint equip;
    uint unequip;
    uint device;
    uint mount;
    uint take;
    uint browse;
    uint lift;
    uint grab;
    uint locked;
    uint unlocked;
    uint scrollUp;
    uint scrollDown;
    uint arrow;
    uint marker;
  };

  struct Sounds
  {
    int bell;
    int click;
    int nextWeapon;
  };

  struct Area
  {
    int x;
    int y;
    int w;
    int h;
  };

  struct Bar : Area
  {
    Vec4 border;
    Vec4 background;
    Vec4 minColour;
    Vec4 maxColour;
  };

public:

  Font    fonts[Font::MAX];
  Colours colours;
  Images  images;
  Sounds  sounds;
  int     iconSize;

  Bar     taggedLife;
  Bar     taggedStatus;
  Bar     botLife;
  Bar     botStamina;
  Bar     botWeapon;
  Bar     vehicleHull;
  Bar     vehicleFuel;
  Area    vehicleWeapon[4];
  Area    vehicleField;

  Bar     selectedLife;
  Bar     selectedStatus;
  Bar     hoverLife;
  Bar     hoverStatus;

public:

  void init();
  void destroy();

};

extern Style style;

}
}
}
