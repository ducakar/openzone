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
 * @file client/ui/BuildFrame.hh
 */

#pragma once

#include <client/ui/Button.hh>
#include <client/ui/ModelField.hh>
#include <client/ui/Frame.hh>

namespace oz::client::ui
{

class BuildFrame : public Frame
{
private:

  enum Mode
  {
    BUILDINGS,
    UNITS,
    ITEMS,
    OBJECTS
  };

  Mode               mode           = BUILDINGS;
  ModelField**       models         = nullptr;

  const BSP*         overlayBSP     = nullptr;
  const ObjectClass* overlayClass   = nullptr;
  Heading            overlayHeading = NORTH;

  int                rows           = 0;
  int                nScrollRows    = 0;
  int                scroll         = 0;
  bool               isOverModel    = false;
  bool               wasOverModel   = false;

private:

  static void overlayCallback(Area* area, const Vec3& ray);
  static void selectBuildings(Button* sender);
  static void selectUnits(Button* sender);
  static void selectItems(Button* sender);
  static void selectObjects(Button* sender);

  static void startPlacement(ModelField* sender, bool isClicked);

  void onRealign() override;
  bool onMouseEvent() override;
  void onDraw() override;

public:

  BuildFrame();
  ~BuildFrame() override;

};

}
