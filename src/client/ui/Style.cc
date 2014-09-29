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
 * @file client/ui/Style.cc
 */

#include <client/ui/Style.hh>

#include <client/Context.hh>

#define OZ_READ_COLOUR(var, r, g, b, a) \
  colours.var = coloursConfig[#var].get(Vec4(r, g, b, a))

#define OZ_READ_SOUND(var, name_) \
  { \
    const char* name = soundsConfig[#var].get(name_); \
    sounds.var = liber.soundIndex(name); \
  }

#define OZ_READ_AREA(area, x_, y_, w_, h_) \
  (\
    area.x = x_, \
    area.y = y_, \
    area.w = w_, \
    area.h = h_, \
    config[#area].getArray(&area.x, 4, 0) \
 )

#define OZ_READ_BAR(bar, x_, y_, w_, h_, border_, background_, minColour_, maxColour_) \
  (\
    bar.x          = x_, \
    bar.y          = y_, \
    bar.w          = w_, \
    bar.h          = h_, \
    bar.border     = border_, \
    bar.background = background_, \
    bar.minColour  = minColour_, \
    bar.maxColour  = maxColour_, \
    config[#bar].getArray(&bar.x, 20, 0) \
 )

#define OZ_LOAD_IMAGE(name) \
  { \
    File file = "@ui/icon/" #name ".dds"; \
    glBindTexture(GL_TEXTURE_2D, images.name); \
    if(GL::textureDataFromFile(file, 0) == 0) { \
      OZ_ERROR("Failed to load texture '%s'", file.path().cstr()); \
    } \
  }

namespace oz
{
namespace client
{
namespace ui
{

struct FontInfo
{
  const char* key;
  const char* name;
  int         height;
};

static const FontInfo FONT_INFOS[Font::MAX] = {
  { "mono",  "", 13 },
  { "sans",  "", 13 },
  { "small", "", 10 },
  { "large", "", 14 },
  { "title", "", 18 }
};

void Style::init()
{
  Log::print("Initialising Style ...");

  File configFile = "@ui/style.json";
  JSON config(configFile);

  const JSON& fontsConfig = config["fonts"];

  for (int i = 0; i < Font::MAX; ++i) {
    const JSON& fontConfig = fontsConfig[FONT_INFOS[i].key];

    const char* name   = fontConfig["name"].get(FONT_INFOS[i].name);
    int         height = fontConfig["height"].get(FONT_INFOS[i].height);

    fonts[i].init(name, height);
  }

  const JSON& coloursConfig = config["colours"];

  OZ_READ_COLOUR(text,             0.70f, 0.90f, 0.80f, 1.00f);
  OZ_READ_COLOUR(textBackground,   0.00f, 0.00f, 0.00f, 1.00f);

  OZ_READ_COLOUR(button,           0.20f, 0.30f, 0.25f, 0.40f);
  OZ_READ_COLOUR(buttonHover,      0.40f, 0.60f, 0.50f, 0.40f);
  OZ_READ_COLOUR(buttonClicked,    0.80f, 1.00f, 0.90f, 0.40f);

  OZ_READ_COLOUR(box,              0.00f, 0.00f, 0.00f, 0.00f);
  OZ_READ_COLOUR(boxHover,         0.40f, 0.60f, 0.50f, 0.10f);
  OZ_READ_COLOUR(boxClicked,       0.80f, 1.00f, 0.90f, 0.10f);

  OZ_READ_COLOUR(slider,           0.20f, 0.30f, 0.25f, 0.40f);
  OZ_READ_COLOUR(sliderHover,      0.40f, 0.60f, 0.50f, 0.40f);
  OZ_READ_COLOUR(sliderClicked,    0.80f, 1.00f, 0.90f, 0.40f);

  OZ_READ_COLOUR(tile,             0.24f, 0.30f, 0.27f, 0.60f);
  OZ_READ_COLOUR(tileHover,        0.48f, 0.60f, 0.54f, 0.60f);

  OZ_READ_COLOUR(frame,            0.00f, 0.00f, 0.00f, 0.30f);
  OZ_READ_COLOUR(background,       0.04f, 0.06f, 0.05f, 1.00f);

  OZ_READ_COLOUR(galileoNormal,    1.00f, 1.00f, 1.00f, 0.60f);
  OZ_READ_COLOUR(galileoMaximised, 1.00f, 1.00f, 1.00f, 1.00f);

  OZ_READ_COLOUR(menuStrip,        0.00f, 0.00f, 0.00f, 1.00f);

  const JSON& soundsConfig = config["sounds"];

  OZ_READ_SOUND(bell,              "");
  OZ_READ_SOUND(click,             "");
  OZ_READ_SOUND(nextWeapon,        "");

  OZ_READ_BAR(taggedLife,          0, 0, 0, 0,
                                   Vec4(1.00f, 1.00f, 1.00f, 1.00f),
                                   Vec4(0.00f, 0.00f, 0.00f, 0.10f),
                                   Vec4(1.00f, 0.00f, 0.00f, 0.60f),
                                   Vec4(0.00f, 1.00f, 0.00f, 0.60f));

  OZ_READ_BAR(taggedStatus,        0, 0, 0, 0,
                                   Vec4(1.00f, 1.00f, 1.00f, 1.00f),
                                   Vec4(0.00f, 0.00f, 0.00f, 0.10f),
                                   Vec4(0.20f, 0.40f, 1.00f, 0.60f),
                                   Vec4(0.20f, 0.40f, 1.00f, 0.60f));

  int weaponBarHeight = fonts[Font::LARGE].height + 8;

  OZ_READ_BAR(botLife,             8, 30, 200, 14,
                                   Vec4(0.80f, 1.00f, 0.90f, 0.60f),
                                   Vec4(0.00f, 0.00f, 0.00f, 0.10f),
                                   Vec4(1.00f, 0.00f, 0.00f, 0.60f),
                                   Vec4(0.00f, 1.00f, 0.00f, 0.60f));

  OZ_READ_BAR(botStamina,          8, 8, 200, 14,
                                   Vec4(0.80f, 1.00f, 0.90f, 0.60f),
                                   Vec4(0.00f, 0.00f, 0.00f, 0.10f),
                                   Vec4(0.70f, 0.30f, 0.40f, 0.60f),
                                   Vec4(0.20f, 0.40f, 1.00f, 0.60f));

  OZ_READ_AREA(botWeapon,          8, 52, 200, weaponBarHeight);

  OZ_READ_BAR(vehicleHull,         -8, 30, 200, 14,
                                   Vec4(0.80f, 1.00f, 0.90f, 0.60f),
                                   Vec4(0.00f, 0.00f, 0.00f, 0.10f),
                                   Vec4(1.00f, 0.00f, 0.00f, 0.60f),
                                   Vec4(0.00f, 1.00f, 0.00f, 0.60f));

  OZ_READ_BAR(vehicleFuel,         -8, 8, 200, 14,
                                   Vec4(0.80f, 1.00f, 0.90f, 0.60f),
                                   Vec4(0.00f, 0.00f, 0.00f, 0.10f),
                                   Vec4(0.70f, 0.30f, 0.40f, 0.60f),
                                   Vec4(0.20f, 0.40f, 1.00f, 0.60f));

  OZ_READ_AREA(vehicleWeapon[0],   -8, 52 + 0 * (weaponBarHeight + 2), 200, weaponBarHeight);
  OZ_READ_AREA(vehicleWeapon[1],   -8, 52 + 1 * (weaponBarHeight + 2), 200, weaponBarHeight);
  OZ_READ_AREA(vehicleWeapon[2],   -8, 52 + 2 * (weaponBarHeight + 2), 200, weaponBarHeight);
  OZ_READ_AREA(vehicleWeapon[3],   -8, 52 + 3 * (weaponBarHeight + 2), 200, weaponBarHeight);

  OZ_READ_AREA(vehicleField,       -8, 52 + 4 * (weaponBarHeight + 2), 180, 180);

  OZ_READ_BAR(selectedLife,        0, 0, 0, 0,
                                   Vec4(1.00f, 1.00f, 1.00f, 0.80f),
                                   Vec4(0.00f, 0.00f, 0.00f, 0.10f),
                                   Vec4(1.00f, 0.00f, 0.00f, 0.60f),
                                   Vec4(0.00f, 1.00f, 0.00f, 0.60f));

  OZ_READ_BAR(selectedStatus,      0, 0, 0, 0,
                                   Vec4(1.00f, 1.00f, 1.00f, 0.80f),
                                   Vec4(0.00f, 0.00f, 0.00f, 0.10f),
                                   Vec4(0.20f, 0.40f, 1.00f, 0.60f),
                                   Vec4(0.20f, 0.40f, 1.00f, 0.60f));

  OZ_READ_BAR(hoverLife,           0, 0, 0, 0,
                                   Vec4(1.00f, 1.00f, 1.00f, 1.00f),
                                   Vec4(0.00f, 0.00f, 0.00f, 0.10f),
                                   Vec4(1.00f, 0.00f, 0.00f, 0.80f),
                                   Vec4(0.00f, 1.00f, 0.00f, 0.80f));

  OZ_READ_BAR(hoverStatus,         0, 0, 0, 0,
                                   Vec4(1.00f, 1.00f, 1.00f, 1.00f),
                                   Vec4(0.00f, 0.00f, 0.00f, 0.10f),
                                   Vec4(0.20f, 0.40f, 1.00f, 0.80f),
                                   Vec4(0.20f, 0.40f, 1.00f, 0.80f));

  config.clear(true);

  MainCall() << [&]
  {
    glGenTextures(GLsizei(sizeof(images) / sizeof(images.crosshair)), &images.crosshair);

    OZ_LOAD_IMAGE(crosshair);
    OZ_LOAD_IMAGE(use);
    OZ_LOAD_IMAGE(equip);
    OZ_LOAD_IMAGE(unequip);
    OZ_LOAD_IMAGE(device);
    OZ_LOAD_IMAGE(mount);
    OZ_LOAD_IMAGE(take);
    OZ_LOAD_IMAGE(browse);
    OZ_LOAD_IMAGE(lift);
    OZ_LOAD_IMAGE(grab);
    OZ_LOAD_IMAGE(locked);
    OZ_LOAD_IMAGE(unlocked);
    OZ_LOAD_IMAGE(scrollUp);
    OZ_LOAD_IMAGE(scrollDown);
    OZ_LOAD_IMAGE(arrow);
    OZ_LOAD_IMAGE(marker);
  };

  context.requestSound(sounds.bell);
  context.requestSound(sounds.click);
  context.requestSound(sounds.nextWeapon);

  Log::printEnd(" OK");
}

void Style::destroy()
{
  context.releaseSound(sounds.nextWeapon);
  context.releaseSound(sounds.click);
  context.releaseSound(sounds.bell);

  MainCall() << [&]
  {
    glDeleteTextures(GLsizei(sizeof(images) / sizeof(images.crosshair)), &images.crosshair);
  };

  for (int i = 0; i < Font::MAX; ++i) {
    fonts[i].destroy();
  }
}

Style style;

}
}
}
