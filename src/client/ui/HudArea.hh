/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file client/ui/HudArea.hh
 */

#pragma once

#include "matrix/Vehicle.hh"

#include "client/ui/Area.hh"
#include "client/ui/Label.hh"

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
    static const float VEHICLE_DIM;
    static const float CROSS_FADE_COEFF;

    Label title;
    Label weaponName;
    Label weaponRounds;
    Label vehicleWeaponNames[Vehicle::MAX_WEAPONS];
    Label vehicleWeaponRounds[Vehicle::MAX_WEAPONS];

    // cache last ids so we know when to re-generate labels
    int   lastObjectId;
    int   lastEntityId;
    int   lastWeaponId;
    int   lastWeaponRounds;
    int   lastVehicleId;
    int   lastVehicleWeaponRounds[Vehicle::MAX_WEAPONS];

    uint  crossTexId;
    uint  useTexId;
    uint  deviceTexId;
    uint  equipTexId;
    uint  mountTexId;
    uint  takeTexId;
    uint  browseTexId;
    uint  liftTexId;
    uint  grabTexId;
    uint  lockedTexId;
    uint  unlockedTexId;

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

    void onReposition() override;
    void onUpdate() override;
    void onDraw() override;

  public:

    HudArea();
    ~HudArea() override;

};

}
}
}
