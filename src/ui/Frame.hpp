/*
 *  Frame.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

#include "ui/Area.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  class Frame : public Area
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
