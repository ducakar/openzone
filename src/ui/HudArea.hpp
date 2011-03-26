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

      static const int   ICON_SIZE    = 32;
      static const int   VEHICLE_SIZE = 180;
      static const float VEHICLE_DIMF;

      uint crossTexId;
      uint useTexId;
      uint mountTexId;
      uint takeTexId;
      uint liftTexId;
      uint grabTexId;

      int  crossIconX;
      int  crossIconY;
      int  useIconX;
      int  useIconY;
      int  mountIconX;
      int  mountIconY;
      int  takeIconX;
      int  takeIconY;
      int  liftIconX;
      int  liftIconY;
      int  grabIconX;
      int  grabIconY;
      int  healthBarX;
      int  healthBarY;
      int  descTextX;
      int  descTextY;

      void drawBotCrosshair();
      void drawBotStatus();
      void drawVehicleStatus();

    protected:

      virtual void onDraw();

    public:

      explicit HudArea();
      virtual ~HudArea();

  };

}
}
}
