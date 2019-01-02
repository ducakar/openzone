/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2019 Davorin Učakar
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
 * @file client/ui/StrategicArea.hh
 *
 * Real-time strategy interface layer for HUD.
 *
 * Provides unit selection and commanding and placement.
 */

#pragma once

#include <client/ui/Area.hh>
#include <client/ui/Text.hh>

namespace oz::client::ui
{

class StrategicArea : public Area
{
private:

  static constexpr float TAG_REACH_DIST     = 100.0f;
  static constexpr float TAG_CLIP_DIST      = 0.1f;
  static constexpr float TAG_CLIP_K         = 9.0f;
  static constexpr float TAG_MIN_PIXEL_SIZE = 4.0f; // size in pixels
  static constexpr float TAG_MAX_COEFF_SIZE = 4.0f; // size in coefficient

  using OverlayCallback = void (Area* sender, const Vec3& p);

  Text             unitName;

  float            pixelStep;
  float            stepPixel;

  OverlayCallback* overlayCallback = nullptr;
  Area*            overlaySender   = nullptr;

  int              dragStartX;
  int              dragStartY;

  int              hoverStr = -1;
  int              hoverEnt = -1;
  int              hoverObj = -1;
  Set<int>         dragObjs;

public:

  int              taggedStr = -1;
  Set<int>         taggedObjs;

  float            mouseW = 0.0f;

private:

  bool projectPoint(const Point& p, int* x, int* y) const;
  bool projectBounds(const AABB& bb, Span* span) const;
  Vec3 getRay(int x, int y);
  void collectHovers();

  void drawHoverTitle(const Span& span, const char* title);
  void drawHoverRect(const Span& span, const Struct* str, const Object* obj);
  void drawTagRect(const Span& span, const Struct* str, const Object* obj, bool isHovered);

protected:

  void onUpdate() override;
  void onVisibilityChange(bool doShow) override;
  void onRealign() override;
  bool onMouseEvent() override;
  void onDraw() override;

public:

  StrategicArea();

  void setOverlay(OverlayCallback* callback, Area* sender);
  void clearOverlay();

};

}
