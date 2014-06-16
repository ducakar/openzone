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
 * @file builder/Context.hh
 */

#pragma once

#include <builder/common.hh>

namespace oz
{
namespace builder
{

class Context
{
public:

  HashMap<String, String> usedTextures;
  HashMap<String, String> usedSounds;
  HashMap<String, String> usedModels;

  bool useS3TC;

public:

  bool isBaseTexture(const char* path);
  void buildTexture(const char* path, const char* destPath, bool allLayers = true);

  void init();
  void destroy();

};

extern Context context;

}
}
