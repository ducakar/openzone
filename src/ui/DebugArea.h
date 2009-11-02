/*
 *  DebugArea.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
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

      DebugArea() : Area( 5, -15 - 6 * font.monoHeight, 480, 10 + 6 * font.monoHeight )
      {
        setFont( MONO );
        setFontColor( 0xff, 0xff, 0xff );
      }

  };

}
}
}
