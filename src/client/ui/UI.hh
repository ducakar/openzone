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
 * @file client/ui/UI.hh
 */

#pragma once

#include <client/ui/Mouse.hh>

namespace oz
{
namespace client
{
namespace ui
{

class Text;
class Area;
class LoadingArea;
class StrategicArea;
class HudArea;
class QuestFrame;
class GalileoFrame;
class MusicPlayer;
class Inventory;
class InfoFrame;
class BuildFrame;
class DebugFrame;

class UI
{
  friend class SettingsMenu;

private:

  float          fps;
  Text*          fpsLabel;

  bool           isFreelook;
  bool           showFPS;
  bool           showDebug;

public:

  bool           isVisible;

  Area*          root;
  LoadingArea*   loadingScreen;
  HudArea*       hudArea;
  StrategicArea* strategicArea;
  QuestFrame*    questFrame;
  GalileoFrame*  galileoFrame;
  MusicPlayer*   musicPlayer;
  Inventory*     inventory;
  InfoFrame*     infoFrame;
  BuildFrame*    buildFrame;
  DebugFrame*    debugFrame;

  UI();

  void showLoadingScreen(bool doShow);

  void update();
  void draw();

  void load();
  void unload();

  void init();
  void destroy();

};

extern UI ui;

}
}
}
