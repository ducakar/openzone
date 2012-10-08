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
 * @file client/ui/UI.hh
 */

#pragma once

#include <client/ui/Font.hh>
#include <client/ui/Mouse.hh>

namespace oz
{
namespace client
{
namespace ui
{

class Label;
class Area;
class LoadingArea;
class SelectionArea;
class HudArea;
class QuestFrame;
class GalileoFrame;
class MusicPlayer;
class Inventory;
class InfoFrame;
class BuildMenu;
class DebugFrame;

class UI
{
  private:

    float          fps;

    bool           isFreelook;
    bool           showFPS;
    bool           showBuild;
    bool           showDebug;

    Label*         fpsLabel;

  public:

    Area*          root;
    LoadingArea*   loadingScreen;
    HudArea*       hudArea;
    SelectionArea* selectionArea;
    QuestFrame*    questFrame;
    GalileoFrame*  galileoFrame;
    MusicPlayer*   musicPlayer;
    Inventory*     inventory;
    InfoFrame*     infoFrame;
    BuildMenu*     buildMenu;
    DebugFrame*    debugFrame;

    explicit UI();

    void showLoadingScreen( bool doShow );

    void update();
    void draw();

    void load();
    void unload();

    void init();
    void free();

};

extern UI ui;

}
}
}
