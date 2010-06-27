/*
 *  UI.hh
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

#include "ui/Font.hh"
#include "ui/Keyboard.hh"
#include "ui/Mouse.hh"
#include "ui/Area.hh"

#include "ui/Frame.hh"
#include "ui/Button.hh"

#include "ui/HudArea.hh"
#include "ui/StrategicArea.hh"
#include "ui/DebugFrame.hh"
#include "ui/InventoryMenu.hh"
#include "ui/BuildMenu.hh"

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

      void init();
      void free();

      void update();
      void draw();

  };

  extern UI ui;

}
}
}
