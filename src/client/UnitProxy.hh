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
 * @file client/UnitProxy.hh
 */

#pragma once

#include <client/Proxy.hh>

namespace oz
{
namespace client
{

class UnitProxy : public Proxy
{
private:

  static const float CAMERA_Z_SMOOTHING;
  static const float CAMERA_Z_TOLERANCE;
  static const float CAMERA_DIST_SMOOTHING;
  static const float EXTERNAL_CAM_DIST;
  static const float EXTERNAL_CAM_CLIP_DIST; // Leave this much space between obstacle and camera
                                             // if camera is brought closer to the eyes because of
                                             // an obstacle.
  static const float SHOULDER_CAM_RIGHT;
  static const float SHOULDER_CAM_UP;
  static const float VEHICLE_CAM_UP_FACTOR;
  static const float BOB_SUPPRESSION_COEF;
  static const float BINOCULARS_MAGNIFICATION;
  static const float INJURY_SUPPRESSION_COEF;
  static const float INJURY_CLAMP;
  static const float DEATH_CAM_DIST;

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

  void read(InputStream* is) override;
  void read(const Json& json) override;

  void write(OutputStream* os) const override;
  Json write() const override;

};

}
}
