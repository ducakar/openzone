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
 * @file client/Caelum.hh
 */

#pragma once

#include <client/common.hh>

namespace oz::client
{

class Caelum
{
public:

  static constexpr int  MAX_STARS             = 2048;
  static constexpr Vec4 GLOBAL_AMBIENT_COLOUR = Vec4(0.12f, 0.12f, 0.15f, 1.00f);

  static const char* const SKYBOX_FACES[];

private:

  static constexpr float DAY_BIAS     = 0.40f;
  static constexpr float DIFFUSE_COEF = 0.80f;
  static constexpr float AMBIENT_COEF = 0.40f;

  static constexpr float RED_COEF     = +0.05f;
  static constexpr float GREEN_COEF   = -0.05f;
  static constexpr float BLUE_COEF    = -0.10f;

  static constexpr Vec4  DAY_COLOUR   = Vec4(0.45f, 0.55f, 0.95f, 1.0f);
  static constexpr Vec4  NIGHT_COLOUR = Vec4(0.02f, 0.02f, 0.05f, 1.0f);

  uint  sunTexId          = 0;
  uint  moonTexId         = 0;
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

  int   id = -1;

public:

  Caelum();

  void update();
  void draw();

  void load();
  void unload();

};

extern Caelum caelum;

}
