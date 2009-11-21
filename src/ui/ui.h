/*
 *  ui.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "Font.h"
#include "Mouse.h"
#include "Area.h"

#include "Frame.h"
#include "Button.h"

#include "HudArea.h"
#include "DebugArea.h"
#include "InventoryMenu.h"
#include "BuildMenu.h"

namespace oz
{
namespace client
{
namespace ui
{

  extern HudArea *hud;

  void init( int screenX, int screenY );
  void free();

  void update();
  void draw();

}
}
}
