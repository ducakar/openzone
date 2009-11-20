/*
 *  BuildMenu.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "Frame.h"

namespace oz
{
namespace client
{
namespace ui
{

  class BuildMenu : public Frame
  {
    protected:

      virtual void onDraw();

    public:

      explicit BuildMenu();

  };

}
}
}
