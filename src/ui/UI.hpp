/*
 *  UI.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "ui/Font.hpp"
#include "ui/Keyboard.hpp"
#include "ui/Mouse.hpp"

#include "ui/Area.hpp"
#include "ui/Frame.hpp"
#include "ui/Button.hpp"

#include "ui/HudArea.hpp"
#include "ui/StrategicArea.hpp"
#include "ui/DebugFrame.hpp"
#include "ui/InventoryMenu.hpp"
#include "ui/BuildMenu.hpp"
#include "ui/LoadingArea.hpp"

namespace oz
{
  class Bot;
}

namespace oz
{
namespace client
{
namespace ui
{

  class UI
  {
    public:

      Area*          root;
      HudArea*       hud;
      StrategicArea* strategic;
      LoadingArea*   loadingScreen;

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
