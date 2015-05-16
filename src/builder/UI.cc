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
 * @file builder/UI.cc
 */

#include <builder/UI.hh>

#include <builder/Context.hh>

namespace oz
{
namespace builder
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

  if (dir.type() != File::DIRECTORY) {
    return;
  }

  Set<String> builtIcons;

  Log::println("Building UI icons {");
  Log::indent();

  File::mkdir("ui");
  File::mkdir("ui/icon");

  for (const File& image : dir.ls()) {
    String name = image.baseName();

    if (image.type() != File::REGULAR || !image.hasExtension("png")) {
      continue;
    }

    Log::print("%s ...", image.name().c());

    if (!Arrays::contains(ICON_NAMES, Arrays::length(ICON_NAMES), name)) {
      OZ_ERROR("Unnecessary icon: %s", image.path().c());
    }

    ImageBuilder::options = 0;
    ImageBuilder::scale   = 1.0;

    if (!ImageBuilder::convertToDDS(image, "ui/icon")) {
      OZ_ERROR("Error converting '%s' to DDS: %s", image.name().c(), ImageBuilder::getError());
    }
    image.unmap();

    Log::printEnd(" OK");

    builtIcons.add(name);
  }

  for (int i = 0; i < Arrays::length(ICON_NAMES); ++i) {
    if (!builtIcons.contains(ICON_NAMES[i])) {
      OZ_ERROR("Mission icon: %s", ICON_NAMES[i]);
    }
  }

  File styleFile = "@ui/style.json";
  Json style;

  if (!style.load(styleFile)) {
    OZ_ERROR("Failed to load style '%s'", styleFile.path().c());
  }

  const Json& sounds = style["sounds"];

  for (const auto& sound : sounds.objectCIter()) {
    context.usedSounds.add(sound.value.get("?"), "UI style");
  }

  Log::unindent();
  Log::println("}");
}

}
}
