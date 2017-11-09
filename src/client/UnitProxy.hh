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
 * @file client/UnitProxy.hh
 */

#pragma once

#include <client/Proxy.hh>

namespace oz::client
{

class UnitProxy : public Proxy
{
private:

  static constexpr float CAMERA_Z_SMOOTHING       = 0.40f;
  static constexpr float CAMERA_Z_TOLERANCE       = 0.30f;
  static constexpr float CAMERA_DIST_SMOOTHING    = 0.80f;
  static constexpr float EXTERNAL_CAM_DIST        = 2.75f;
  // Leave this much space between obstacle and camera if camera is brought closer to the eyes
  // because of an obstacle.
  static constexpr float EXTERNAL_CAM_CLIP_DIST   = 0.10f;
  static constexpr float SHOULDER_CAM_RIGHT       = 0.25f;
  static constexpr float SHOULDER_CAM_UP          = 0.25f;
  static constexpr float VEHICLE_CAM_UP_FACTOR    = 0.15f;
  static constexpr float BOB_SUPPRESSION_COEF     = 0.80f;
  static constexpr float BINOCULARS_MAGNIFICATION = 0.20f;
  static constexpr float INJURY_SUPPRESSION_COEF  = 0.98f;
  static constexpr float INJURY_CLAMP             = 2.00f;
  static constexpr float DEATH_CAM_DIST           = 4.00f;

public:

  Quat  baseRot;
  Quat  headRot;
  float headH;
  float headV;

private:

  Point botEye;
  float bobTheta;
  float bobBias;

  float injuryRatio;
  float camDist;

  int   oldBot;

  bool  isExternal;
  bool  isFreelook;

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
