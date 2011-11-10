/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file client/ui/UI.hpp
 */

#pragma once

#include "client/ui/Font.hpp"
#include "client/ui/Keyboard.hpp"
#include "client/ui/Mouse.hpp"

#include "client/ui/Area.hpp"
#include "client/ui/Frame.hpp"
#include "client/ui/Button.hpp"

#include "client/ui/LoadingArea.hpp"
#include "client/ui/HudArea.hpp"
#include "client/ui/StrategicArea.hpp"
#include "client/ui/InventoryMenu.hpp"
#include "client/ui/InfoFrame.hpp"
#include "client/ui/BuildMenu.hpp"
#include "client/ui/DebugFrame.hpp"

namespace oz
{
namespace client
{
namespace ui
{

class UI
{
  public:

    bool isFreelook;
    bool showBuild;
    bool showDebug;

  public:

    Area*          root;
    LoadingArea*   loadingScreen;
    HudArea*       hudArea;
    StrategicArea* strategicArea;
    InventoryMenu* inventoryMenu;
    InventoryMenu* browseMenu;
    InfoFrame*     infoFrame;
    BuildMenu*     buildMenu;
    DebugFrame*    debugFrame;

    UI();

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
