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

/**
 * @file client/ui/MissionMenu.hh
 */

#pragma once

#include <client/ui/Button.hh>

namespace oz::client::ui
{

class MissionMenu;

class MissionButton : public Button
{
private:

  MissionMenu* missionMenu;

public:

  const int index;

protected:

  bool onMouseEvent() override;

public:

  explicit MissionButton(const char* text, Callback* callback, MissionMenu* missionMenu_,
                         int index_, int width, int height);

};

class MissionMenu : public Area
{
  friend class MissionButton;

private:

  struct MissionInfo
  {
    String name;
    String title;
    String description;
    uint   imageId;
  };

  List<MissionInfo> missions;
  int               nSelections;
  int               selection;
  int               scroll;

  Text              description;

  uint              imageId     = 0;
  int               imageX;
  int               imageY;
  int               imageWidth;
  int               imageHeight;

  static void loadMission(Button* sender);

protected:

  void onRealign() override;
  bool onMouseEvent() override;
  bool onKeyEvent() override;
  void onDraw() override;

public:

  MissionMenu();
  ~MissionMenu() override;

};

}
