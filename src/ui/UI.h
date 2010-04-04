/*
 *  UI.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "ui/Font.h"
#include "ui/Keyboard.h"
#include "ui/Mouse.h"
#include "ui/Area.h"

#include "ui/Frame.h"
#include "ui/Button.h"

#include "ui/HudArea.h"
#include "ui/StrategicArea.h"
#include "ui/DebugFrame.h"
#include "ui/InventoryMenu.h"
#include "ui/BuildMenu.h"

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
