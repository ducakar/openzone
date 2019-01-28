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
 * @file client/Caelum.hh
 */

#pragma once

#include <client/common.hh>

namespace oz::client
{

class Caelum
{
public:

  static constexpr const char* SKYBOX_FACES[]        = {"+x", "-x", "+y", "-y", "+z", "-z"};
  static constexpr Vec4        GLOBAL_AMBIENT_COLOUR = Vec4(0.12f, 0.12f, 0.15f, 1.00f);

private:

  uint  sunTexId          = 0;
  uint  moonTexId         = 0;
  uint  skyboxTexIds[6]   = {};
  int   celestialShaderId = -1;

public:

  float angle            = {};
  float ratio            = 0.0f;
  Vec3  originalLightDir = Vec3(-1.0f, 0.0f, 0.0f);
  Vec3  axis             = Vec3(0.0f, 1.0f, 0.0f);
  Vec3  lightDir         = Vec3(0.0f, 0.0f, 1.0f);

  Vec4  diffuseColour    = Vec4::ONE;
  Vec4  ambientColour    = Vec4::ONE;
  Vec4  caelumColour     = Vec4::ONE;
  Vec4  nightColour      = Vec4::ONE;
  float nightLuminance   = 1.0f;

  int   id               = -1;

public:

  void update();
  void draw();

  void load();
  void unload();

};

extern Caelum caelum;

}
