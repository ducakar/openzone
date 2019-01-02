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
 * @file client/ui/ModelField.hh
 */

#pragma once

#include <client/ui/Area.hh>

namespace oz::client::ui
{

class ModelField : public Area
{
public:

  using Callback = void (ModelField* sender, bool isClicked);

private:

  static constexpr float DEFAULT_ROTATION   = 3.0f * Math::TAU / 8.0f;
  static constexpr float ROTATION_VEL       = 1.30f * Timer::TICK_TIME;
  static constexpr float ROTATION_SMOOTHING = 0.80f;

  Callback*  callback;
  const BSP* bsp;
  int        model;

  float      defaultRot;
  float      currRot;
  float      nextRot;

  int        clickMask;
  bool       isHighlighted;
  bool       isClicked;
  bool       wasClicked;

public:

  int        id; ///< Can be used to store e.g. object index.

protected:

  void onVisibilityChange(bool doShow) override;
  bool onMouseEvent() override;
  void onDraw() override;

public:

  explicit ModelField(Callback* callback_, int size);

  void setCallback(Callback* callback_);
  void setDefaultRotation(float defaultRotation);

  void setModel(const BSP* bsp_);
  void setModel(int model_);

  void setClickMask(int mask);

};

}
