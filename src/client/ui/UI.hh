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
 * @file client/ui/UI.hh
 */

#pragma once

#include <client/ui/Mouse.hh>

namespace oz::client::ui
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

  float          fps        = 0.0f;
  Text*          fpsLabel   = nullptr;

  bool           isFreelook = false;
  bool           showFPS    = false;
  bool           showDebug  = false;

public:

  bool           isVisible     = true;

  Area*          root          = nullptr;
  LoadingArea*   loadingScreen = nullptr;
  HudArea*       hudArea       = nullptr;
  StrategicArea* strategicArea = nullptr;
  QuestFrame*    questFrame    = nullptr;
  GalileoFrame*  galileoFrame  = nullptr;
  MusicPlayer*   musicPlayer   = nullptr;
  Inventory*     inventory     = nullptr;
  InfoFrame*     infoFrame     = nullptr;
  BuildFrame*    buildFrame    = nullptr;
  DebugFrame*    debugFrame    = nullptr;

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
