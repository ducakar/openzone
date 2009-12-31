/*
 *  Frame.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "Area.h"

namespace oz
{
namespace client
{
namespace ui
{

  struct Frame : Area
  {
    protected:

      virtual bool onMouseEvent();
      virtual void onDraw();

    public:

      explicit Frame( int width, int height ) : Area( width, height ) {}
      explicit Frame( int x, int y, int width, int height ) : Area( x, y, width, height ) {}

  };

}
}
}
