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

#include <builder/Caelum.hh>

#include <client/Caelum.hh>

#include <builder/Context.hh>

namespace oz::builder
{

void Caelum::build(const char* name)
{
  Log::println("Prebuilding Caelum '%s' {", name);
  Log::indent();

  ImageBuilder::options = ImageBuilder::MIPMAPS_BIT;
  ImageBuilder::scale   = 1.0;

  if (context.useS3TC) {
    ImageBuilder::options |= ImageBuilder::COMPRESSION_BIT;
  }

  File dir = String::format("caelum/%s", name);
  dir.mkdir();

  for (int i = 0; i < 6; ++i) {
    File path = String::format("@caelum/%s/%s", name, client::Caelum::SKYBOX_FACES[i]);

    context.buildTexture(path, path.toNative(), false);
  }

  if (!ImageBuilder::convertToDDS("@caelum/sun.png", "caelum")) {
    OZ_ERROR("Failed to build sun texture");
  }
  if (!ImageBuilder::convertToDDS("@caelum/moon.png", "caelum")) {
    OZ_ERROR("Failed to build moon texture");
  }

  Log::unindent();
  Log::println("}");
}

Caelum caelum;

}
