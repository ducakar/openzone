/*
 *  HudArea.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "ui/Area.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  class HudArea : public Area
  {
    friend class UI;

    private:

      static const int ICON_SIZE = 32;

      int crossTexId;
      int useTexId;
      int mountTexId;
      int takeTexId;
      int liftTexId;
      int grabTexId;

      int crossIconX;
      int crossIconY;
      int useIconX;
      int useIconY;
      int mountIconX;
      int mountIconY;
      int takeIconX;
      int takeIconY;
      int liftIconX;
      int liftIconY;
      int grabIconX;
      int grabIconY;
      int healthBarX;
      int healthBarY;
      int descTextX;
      int descTextY;

    protected:

      virtual void onDraw();

    public:

      explicit HudArea();
      virtual ~HudArea();

  };

}
}
}
