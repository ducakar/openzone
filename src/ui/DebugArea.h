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
    public:

      DebugArea() : Area( 5, -15 - font.monoHeight * 4, 600, 10 + font.monoHeight * 4 )
      {
        setFont( MONO );
        setFontColor( 0xff, 0xff, 0xff );
      }

      virtual void draw();

  };

}
}
}
