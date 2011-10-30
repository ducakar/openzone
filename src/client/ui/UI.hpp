/*
 *  UI.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
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
