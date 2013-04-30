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
 * @file client/ui/Style.cc
 */

#include <stable.hh>
#include <client/ui/Style.hh>

#define OZ_READ_COLOUR( var, r, g, b, a ) \
  colours.var = coloursConfig[#var].get( Vec4( r, g, b, a ) )

#define OZ_READ_AREA( area, x_, y_, w_, h_ ) \
  ( \
    area.x = x_, \
    area.y = y_, \
    area.w = w_, \
    area.h = h_, \
    config[#area].get( &area.x, 4 ) \
  )

#define OZ_READ_BAR( bar, x_, y_, w_, h_, minColour_, maxColour_ ) \
  ( \
    bar.x         = x_, \
    bar.y         = y_, \
    bar.w         = w_, \
    bar.h         = h_, \
    bar.minColour = minColour_, \
    bar.maxColour = maxColour_, \
    config[#bar].get( &bar.x, 16 ) \
  )

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
  { "mono",  "DroidSansMono", 13 },
  { "sans",  "DroidSans",     13 },
  { "small", "DroidSans",     11 },
  { "large", "DroidSans",     14 },
  { "title", "DroidSans",     18 }
};

void Style::init()
{
  Log::print( "Initialising Style ..." );

  File configFile( "@ui/style/style.json" );
  JSON config( configFile );

  const JSON& fontsConfig = config["fonts"];

  for( int i = 0; i < Font::MAX; ++i ) {
    const JSON& fontConfig = fontsConfig[ FONT_INFOS[i].key ];

    const char* name   = fontConfig["name"].get( FONT_INFOS[i].name );
    int         height = fontConfig["height"].get( FONT_INFOS[i].height );

    fonts[i].init( name, height );
  }

  const JSON& coloursConfig = config["colours"];

  OZ_READ_COLOUR( text,             1.00f, 1.00f, 1.00f, 1.00f );
  OZ_READ_COLOUR( textBackground,   0.00f, 0.00f, 0.00f, 1.00f );

  OZ_READ_COLOUR( button,           0.40f, 0.40f, 0.40f, 0.40f );
  OZ_READ_COLOUR( buttonHover,      0.60f, 0.60f, 0.60f, 0.40f );
  OZ_READ_COLOUR( buttonClicked,    1.00f, 1.00f, 1.00f, 0.40f );

  OZ_READ_COLOUR( tile,             0.40f, 0.40f, 0.40f, 0.60f );
  OZ_READ_COLOUR( tileHover,        0.60f, 0.60f, 0.60f, 0.60f );

  OZ_READ_COLOUR( frame,            0.00f, 0.00f, 0.00f, 0.30f );
  OZ_READ_COLOUR( background,       0.05f, 0.05f, 0.05f, 1.00f );

  OZ_READ_COLOUR( barBorder,        1.00f, 1.00f, 1.00f, 0.50f );
  OZ_READ_COLOUR( barBackground,    0.00f, 0.00f, 0.00f, 0.10f );

  OZ_READ_COLOUR( galileoNormal,    1.00f, 1.00f, 1.00f, 0.60f );
  OZ_READ_COLOUR( galileoMaximised, 1.00f, 1.00f, 1.00f, 0.80f );

  OZ_READ_COLOUR( menuStrip,        0.00f, 0.00f, 0.00f, 1.00f );

  int weaponBarHeight = fonts[Font::LARGE].height + 8;

  OZ_READ_BAR( botStamina,          8,  8, 200, 14,
                                    Vec4( 0.70f, 0.30f, 0.40f, 0.50f ),
                                    Vec4( 0.00f, 0.30f, 1.00f, 0.50f ) );

  OZ_READ_BAR( botHealth,           8, 30, 200, 14,
                                    Vec4( 1.00f, 0.00f, 0.00f, 0.50f ),
                                    Vec4( 0.00f, 1.00f, 0.00f, 0.50f ) );

  OZ_READ_AREA( botWeapon,          8, 52, 200, weaponBarHeight );

  OZ_READ_BAR( vehicleFuel,         -8,  8, 200, 14,
                                    Vec4( 0.70f, 0.30f, 0.40f, 0.50f ),
                                    Vec4( 0.00f, 0.30f, 1.00f, 0.50f ) );

  OZ_READ_BAR( vehicleHull,         -8, 30, 200, 14,
                                    Vec4( 1.00f, 0.00f, 0.00f, 0.50f ),
                                    Vec4( 0.00f, 1.00f, 0.00f, 0.50f ) );

  OZ_READ_AREA( vehicleWeapon[0],   -8, 52 + 0 * ( weaponBarHeight + 2 ), 200, weaponBarHeight );
  OZ_READ_AREA( vehicleWeapon[1],   -8, 52 + 1 * ( weaponBarHeight + 2 ), 200, weaponBarHeight );
  OZ_READ_AREA( vehicleWeapon[2],   -8, 52 + 2 * ( weaponBarHeight + 2 ), 200, weaponBarHeight );
  OZ_READ_AREA( vehicleWeapon[3],   -8, 52 + 3 * ( weaponBarHeight + 2 ), 200, weaponBarHeight );

  config.clear( true );

  Log::printEnd( " OK" );
}

void Style::destroy()
{
  for( int i = 0; i < Font::MAX; ++i ) {
    fonts[i].destroy();
  }
}

Style style;

}
}
}
