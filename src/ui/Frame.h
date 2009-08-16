/*
 *  Frame.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Area.h"

namespace oz
{
namespace client
{
namespace ui
{

  class Frame : public Area
  {
    protected:

      virtual void onMouseEvent();
      virtual void draw();

    public:

      Frame( int width, int height ) : Area( width, height ) {}

  };

}
}
}
