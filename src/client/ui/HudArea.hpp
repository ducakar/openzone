/*
 *  HudArea.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Vehicle.hpp"

#include "client/ui/Area.hpp"

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

      Label title;
      Label weaponName;
      Label weaponRounds;
      Label vehicleWeaponNames[Vehicle::WEAPONS_MAX];
      Label vehicleWeaponRounds[Vehicle::WEAPONS_MAX];

      int   lastTaggedId;
      int   lastWeaponId;
      int   lastWeaponRounds;
      int   lastVehicleId;
      int   lastVehicleWeaponRounds[3];

      uint  crossTexId;
      uint  useTexId;
      uint  equipTexId;
      uint  mountTexId;
      uint  takeTexId;
      uint  browseTexId;
      uint  liftTexId;
      uint  grabTexId;

      int   descTextX;
      int   descTextY;
      int   healthBarX;
      int   healthBarY;
      int   crossIconX;
      int   crossIconY;
      int   leftIconX;
      int   leftIconY;
      int   rightIconX;
      int   rightIconY;
      int   bottomIconX;
      int   bottomIconY;

      void drawBotCrosshair();
      void drawBotStatus();
      void drawVehicleStatus();

    protected:

      virtual void onUpdate();
      virtual void onDraw();

    public:

      HudArea();
      virtual ~HudArea();

  };

}
}
}
