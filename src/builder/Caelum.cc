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
 * @file builder/Caelum.cc
 */

#include <builder/Caelum.hh>

#include <client/Caelum.hh>

#include <builder/Context.hh>

namespace oz
{
namespace builder
{

void Caelum::build(const char* name)
{
  Log::println("Prebuilding Caelum '%s' {", name);
  Log::indent();

  int texOptions = ImageBuilder::MIPMAPS_BIT;
  if (context.useS3TC) {
    texOptions |= ImageBuilder::COMPRESSION_BIT;
  }

  String path = String::str("caelum/%s", name);
  File::mkdir(path);

  for (int i = 0; i < 6; ++i) {
    String path = String::str("@caelum/%s/%s", name, client::Caelum::SKYBOX_FACES[i]);

    context.buildTexture(path, &path[1], false);
  }

  if (!ImageBuilder::convertToDDS("@caelum/sun.png", texOptions, "caelum")) {
    OZ_ERROR("Failed to build sun texture");
  }
  if (!ImageBuilder::convertToDDS("@caelum/moon.png", texOptions, "caelum")) {
    OZ_ERROR("Failed to build moon texture");
  }

  Log::unindent();
  Log::println("}");
}

Caelum caelum;

}
}
