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

  static const float TAG_REACH_DIST;
  static const float TAG_CLIP_DIST;
  static const float TAG_CLIP_K;
  static const float TAG_MIN_PIXEL_SIZE;
  static const float TAG_MAX_COEFF_SIZE;

  typedef void OverlayCallback(Area* sender, const Vec3& p);

  Text             unitName;

  float            pixelStep;
  float            stepPixel;

  OverlayCallback* overlayCallback;
  Area*            overlaySender;

  int              dragStartX;
  int              dragStartY;

  int              hoverStr;
  int              hoverEnt;
  int              hoverObj;
  Set<int>         dragObjs;

public:

  int              taggedStr;
  Set<int>         taggedObjs;

  float            mouseW;

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
