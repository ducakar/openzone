/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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

namespace oz
{
namespace client
{
namespace ui
{

class BuildFrame : public Frame
{
private:

  static const int  SLOT_SIZE      = 76;
  static const Mat4 OVERLAY_GREEN;
  static const Mat4 OVERLAY_YELLOW;
  static const Mat4 OVERLAY_RED;

  enum Mode
  {
    BUILDINGS,
    UNITS,
    ITEMS,
    OBJECTS
  };

  Mode               mode;
  ModelField**       models;

  const oz::BSP*     overlayBSP;
  const ObjectClass* overlayClass;
  Heading            overlayHeading;

  int                rows;
  int                nScrollRows;
  int                scroll;
  bool               isOverModel;
  bool               wasOverModel;

private:

  static void overlayCallback( Area* area, const Vec3& ray );
  static void selectBuildings( Button* sender );
  static void selectUnits( Button* sender );
  static void selectItems( Button* sender );
  static void selectObjects( Button* sender );

  static void startPlacement( ModelField* sender, bool isClicked );

  void onReposition() override;
  bool onMouseEvent() override;
  void onDraw() override;

public:

  explicit BuildFrame();
  ~BuildFrame() override;

};

}
}
}
