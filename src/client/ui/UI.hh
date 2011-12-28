/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2011 Davorin Učakar
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

#include "client/ui/Font.hh"
#include "client/ui/Keyboard.hh"
#include "client/ui/Mouse.hh"

#include "client/ui/LoadingArea.hh"
#include "client/ui/BuildMenu.hh"
#include "client/ui/DebugFrame.hh"

namespace oz
{
namespace client
{
namespace ui
{

class UI
{
  public:

    float uiScale;
    float uiAspect;

    bool  isFreelook;
    bool  showBuild;
    bool  showDebug;

  public:

    Area*        root;
    LoadingArea* loadingScreen;
    BuildMenu*   buildMenu;
    DebugFrame*  debugFrame;

    UI();

    void showLoadingScreen( bool doShow );
    void prepare();

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
