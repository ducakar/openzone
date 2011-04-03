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
#include "client/ui/DebugFrame.hpp"
#include "client/ui/BuildMenu.hpp"
#include "client/ui/MusicPlayer.hpp"

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
    private:

      bool isFreelook;

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
