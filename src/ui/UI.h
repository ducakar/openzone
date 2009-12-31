/*
 *  UI.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "Font.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Area.h"

#include "Frame.h"
#include "Button.h"

#include "HudArea.h"
#include "StrategicArea.h"
#include "DebugFrame.h"
#include "InventoryMenu.h"
#include "BuildMenu.h"

namespace oz
{
namespace client
{
namespace ui
{

  struct UI
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
