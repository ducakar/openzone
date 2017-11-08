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

#include <builder/UI.hh>

#include <builder/Context.hh>

namespace oz::builder
{

const char* const UI::ICON_NAMES[] = {
  "crosshair",
  "use",
  "device",
  "equip",
  "unequip",
  "mount",
  "take",
  "browse",
  "lift",
  "grab",
  "locked",
  "unlocked",
  "scrollUp",
  "scrollDown",
  "marker",
  "arrow"
};

void UI::buildIcons()
{
  File dir = "@ui/icon";

  if (!dir.isDirectory()) {
    return;
  }

  Set<String> builtIcons;

  Log::println("Building UI icons {");
  Log::indent();

  File("ui").mkdir();
  File("ui/icon").mkdir();

  for (const File& image : dir.list()) {
    String name = image.baseName();

    if (!image.isRegular() || !image.hasExtension("png")) {
      continue;
    }

    Log::print("%s ...", image.name().c());

    if (!Arrays::contains(ICON_NAMES, Arrays::size(ICON_NAMES), name)) {
      OZ_ERROR("Unnecessary icon: %s", image.c());
    }

    ImageBuilder::options = 0;
    ImageBuilder::scale   = 1.0;

    if (!ImageBuilder::convertToDDS(image, "ui/icon")) {
      OZ_ERROR("Error converting '%s' to DDS: %s", image.name().c(), ImageBuilder::getError());
    }

    Log::printEnd(" OK");

    builtIcons.add(name);
  }

  for (int i = 0; i < Arrays::size(ICON_NAMES); ++i) {
    if (!builtIcons.contains(ICON_NAMES[i])) {
      OZ_ERROR("Mission icon: %s", ICON_NAMES[i]);
    }
  }

  File styleFile = "@ui/style.json";
  Json style;

  if (!style.load(styleFile)) {
    OZ_ERROR("Failed to load style '%s'", styleFile.c());
  }

  const Json& sounds = style["sounds"];

  for (const auto& sound : sounds.objectCRange()) {
    context.usedSounds.add(sound.value.get("?"), "UI style");
  }

  Log::unindent();
  Log::println("}");
}

}
