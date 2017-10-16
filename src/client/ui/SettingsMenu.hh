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

/**
 * @file client/ui/SettingsMenu.hh
 */

#pragma once

#include <client/ui/Button.hh>
#include <client/ui/CheckBox.hh>
#include <client/ui/Slider.hh>

namespace oz::client::ui
{

class SettingsMenu : public Area
{
private:

  Area*     profile;
  Area*     graphics;
  Area*     sound;
  Area*     debug;

  CheckBox* showAim;
  CheckBox* showBounds;
  CheckBox* showDebug;
  CheckBox* showFPS;

  Slider*   distance;

private:

  static void back(Button* sender);
  static void apply(Button* sender);

  static void openProfile(Button* sender);
  static void openGraphics(Button* sender);
  static void openSound(Button* sender);
  static void openDebug(Button* sender);

protected:

  void onRealign() override;
  bool onMouseEvent() override;
  bool onKeyEvent() override;
  void onDraw() override;

public:

  SettingsMenu();
  ~SettingsMenu() override;

};

}
