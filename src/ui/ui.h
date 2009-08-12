/*
 *  ui.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Font.h"
#include "Mouse.h"
#include "Area.h"
#include "Button.h"
#include "DebugArea.h"
#include "HealthArea.h"

namespace oz
{
namespace client
{
namespace ui
{

  extern Area root;

  void draw();

  void init( int screenX, int screenY );
  void free();

}
}
}
