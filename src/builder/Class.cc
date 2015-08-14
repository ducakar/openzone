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

#include <builder/Class.hh>

#include <builder/Context.hh>

namespace oz
{
namespace builder
{

void Class::scanObjClass(const char* className)
{
  String name       = className;
  File   configFile = "@class/" + name + ".json";

  if (!config.load(configFile)) {
    OZ_ERROR("Failed to load '%s'", configFile.c());
  }

  context.usedModels.include(config["imagoModel"].get(""), name + " (Object class)");

  const Json& soundsConfig = config["audioSounds"];
  for (const auto& sound : soundsConfig.objectCIter()) {
    context.usedSounds.include(sound.value.get("?"), name + " (Object class)");
  }
}

void Class::scanFragPool(const char* poolName)
{
  String name       = poolName;
  File   configFile = "@frag/" + name + ".json";

  Json config;
  if (!config.load(configFile)) {
    OZ_ERROR("Failed to load '%s'", configFile.c());
  }

  const Json& modelsConfig = config["models"];
  for (const Json& model : modelsConfig.arrayCIter()) {
    context.usedModels.include(model.get("?"), name + " (Object class)");
  }
}

Class clazz;

}
}
