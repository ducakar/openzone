/*
 *  Frame.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/ui/Area.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  class Frame : public Area
  {
    protected:

      String title;

      virtual bool onMouseEvent();
      virtual void onDraw();

    public:

      explicit Frame( int width, int height, const char* title );
      explicit Frame( int x, int y, int width, int height, const char* title );

  };

}
}
}
