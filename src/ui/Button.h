/*
 *  Button.h
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

  class Button : public Area
  {
    protected:

      virtual void onClick( int x, int y );

    public:

      Button() : Area( 5, -15 - font.monoHeight * 4, 600, 10 + font.monoHeight * 4 )
      {
        setFont( MONO );
        setFontColor( 0xff, 0xff, 0xff );
      }

      virtual void draw();

  };

}
}
}
