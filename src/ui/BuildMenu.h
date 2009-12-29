/*
 *  BuildMenu.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "Frame.h"

namespace oz
{
namespace client
{
namespace ui
{

  struct BuildMenu : public Frame
  {
    protected:

      virtual void onMouseEvent();
      virtual void onDraw();

    public:

      explicit BuildMenu();

  };

}
}
}
