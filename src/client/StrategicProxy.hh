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
 * @file client/StrategicProxy.hh
 */

#pragma once

#include <client/Proxy.hh>

namespace oz::client
{

class StrategicProxy : public Proxy
{
private:

  static constexpr float MIN_HEIGHT      = 2.00f;
  static constexpr float MAX_HEIGHT      = 50.0f;
  static constexpr float DEFAULT_HEIGHT  = 15.0f;
  static constexpr float DEFAULT_ANGLE   = 45.0f;
  static constexpr float FREE_LOW_SPEED  = 2.40f;
  static constexpr float FREE_HIGH_SPEED = 30.0f;
  static constexpr float RTS_LOW_SPEED   = 15.0f;
  static constexpr float RTS_HIGH_SPEED  = 45.0f;
  static constexpr float ZOOM_FACTOR     = 0.10f;

public:

  float h;
  float v;
  Point desiredPos;
  float height;

  bool  isFree;
  bool  isFreeFast;
  bool  isRTSFast;

  bool  hasBuildFrame;

public:

  void begin() override;
  void end() override;

  void prepare() override;
  void update() override;

  void reset() override;

  void read(Stream* is) override;
  void read(const Json& json) override;

  void write(Stream* os) const override;
  Json write() const override;

};

}
