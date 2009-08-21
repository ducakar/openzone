/*
 *  DebugArea.h
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

  class DebugArea : public Area
  {
    protected:

      virtual void draw();

    public:

      DebugArea() : Area( 5, -15 - 4 * font.monoHeight, 480, 10 + 4 * font.monoHeight )
      {
        setFont( MONO );
        setFontColor( 0xff, 0xff, 0xff );
      }

  };

}
}
}
