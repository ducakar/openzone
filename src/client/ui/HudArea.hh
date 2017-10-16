/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2016 Davorin Učakar
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

#include <matrix/Vehicle.hh>
#include <client/ui/Text.hh>
#include <client/ui/Bar.hh>
#include <client/ui/ModelField.hh>

namespace oz::client::ui
{

class HudArea : public Area
{
  friend class UI;

private:

  static const int   ICON_SIZE    = 32;
  static const int   VEHICLE_SIZE = 180;
  static const float VEHICLE_DIM;
  static const float CROSS_FADE_COEFF;

  Text        title;
  Text        weaponName;
  Text        weaponRounds;
  Text        vehicleWeaponNames[Vehicle::MAX_WEAPONS];
  Text        vehicleWeaponRounds[Vehicle::MAX_WEAPONS];

  Bar         taggedLife;
  Bar         taggedStatus;
  Bar         botLife;
  Bar         botStamina;
  Bar         vehicleHull;
  Bar         vehicleFuel;

  ModelField* vehicleModel;

  int         descTextX;
  int         descTextY;
  int         healthBarX;
  int         healthBarY;
  int         crossIconX;
  int         crossIconY;
  int         leftIconX;
  int         leftIconY;
  int         rightIconX;
  int         rightIconY;
  int         bottomIconX;
  int         bottomIconY;

  void drawBotCrosshair();
  void drawBotStatus();
  void drawVehicleStatus();

protected:

  void onRealign() override;
  bool onMouseEvent() override;
  void onDraw() override;

public:

  HudArea();

};

}
