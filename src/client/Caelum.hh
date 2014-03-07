/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file client/Caelum.hh
 */

#pragma once

#include <client/common.hh>

namespace oz
{
namespace client
{

class Caelum
{
public:

  static const char* const SKYBOX_FACES[];
  static const int         MAX_STARS = 2048;
  static const Vec4        GLOBAL_AMBIENT_COLOUR;

private:

  static const float DAY_BIAS;
  static const float AMBIENT_COEF;

  static const float RED_COEF;
  static const float GREEN_COEF;
  static const float BLUE_COEF;

  static const Vec4  DAY_COLOUR;
  static const Vec4  NIGHT_COLOUR;

  uint  sunTexId;
  uint  moonTexId;
  uint  skyboxTexIds[6];
  int   celestialShaderId;

public:

  float angle;
  float ratio;
  Vec3  originalLightDir;
  Vec3  axis;
  Vec3  lightDir;

  Vec4  diffuseColour;
  Vec4  ambientColour;
  Vec4  caelumColour;
  Vec4  nightColour;
  float nightLuminance;

  int   id;

public:

  explicit Caelum();

  void update();
  void draw();

  void load();
  void unload();

};

extern Caelum caelum;

}
}
