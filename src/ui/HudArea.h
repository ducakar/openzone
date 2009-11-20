/*
 *  HudArea.h
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

  class HudArea : public Area
  {
    friend void update();
    friend void draw();

    private:

      static const int ICON_SIZE = 32;

      int crossTexId;
      int useTexId;
      int mountTexId;
      int grabTexId;

      int crossIconX;
      int crossIconY;
      int useIconX;
      int useIconY;
      int mountIconX;
      int mountIconY;
      int grabIconX;
      int grabIconY;
      int healthBarX;
      int healthBarY;

    protected:

      virtual void onDraw();

    public:

      int taggedObjIndex;

      explicit HudArea( int width, int height );
      virtual ~HudArea();

  };

}
}
}
