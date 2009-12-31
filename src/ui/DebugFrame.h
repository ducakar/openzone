/*
 *  DebugFrame.h
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

  struct DebugFrame : Frame
  {
    protected:

      virtual void onDraw();

    public:

      explicit DebugFrame();

  };

}
}
}
