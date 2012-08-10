/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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

#include "stable.hh"

#include "client/ui/Style.hh"

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

  PFile configFile( "ui/style.json" );

  JSON config;
  config.load( &configFile );

  const JSON& coloursConfig       = config["colours"];

  const JSON& textArray           = coloursConfig["text"];
  const JSON& textBackgroundArray = coloursConfig["textBackground"];

  const JSON& buttonArray         = coloursConfig["button"];
  const JSON& buttonHoverArray    = coloursConfig["buttonHover"];
  const JSON& buttonClickedArray  = coloursConfig["buttonClicked"];

  const JSON& tileArray           = coloursConfig["tile"];
  const JSON& tileHoverArray      = coloursConfig["tileHover"];

  const JSON& frameArray          = coloursConfig["frame"];
  const JSON& backgroundArray     = coloursConfig["background"];

  const JSON& menuStripArray      = coloursConfig["menuStrip"];

  colours.text           = Vec4( textArray[0].get( 1.00f ),
                                 textArray[1].get( 1.00f ),
                                 textArray[2].get( 1.00f ),
                                 textArray[3].get( 1.00f ) );
  colours.textBackground = Vec4( textBackgroundArray[0].get( 0.00f ),
                                 textBackgroundArray[1].get( 0.00f ),
                                 textBackgroundArray[2].get( 0.00f ),
                                 textBackgroundArray[3].get( 1.00f ) );

  colours.button         = Vec4( buttonArray[0].get( 0.20f ),
                                 buttonArray[1].get( 0.20f ),
                                 buttonArray[2].get( 0.20f ),
                                 buttonArray[3].get( 0.40f ) );
  colours.buttonHover    = Vec4( buttonHoverArray[0].get( 0.60f ),
                                 buttonHoverArray[1].get( 0.60f ),
                                 buttonHoverArray[2].get( 0.60f ),
                                 buttonHoverArray[3].get( 0.40f ) );
  colours.buttonClicked  = Vec4( buttonClickedArray[0].get( 1.00f ),
                                 buttonClickedArray[1].get( 1.00f ),
                                 buttonClickedArray[2].get( 1.00f ),
                                 buttonClickedArray[3].get( 0.40f ) );

  colours.tile           = Vec4( tileArray[0].get( 0.40f ),
                                 tileArray[1].get( 0.40f ),
                                 tileArray[2].get( 0.40f ),
                                 tileArray[3].get( 0.60f ) );
  colours.tileHover      = Vec4( tileHoverArray[0].get( 0.60f ),
                                 tileHoverArray[1].get( 0.60f ),
                                 tileHoverArray[2].get( 0.60f ),
                                 tileHoverArray[3].get( 0.60f ) );

  colours.frame          = Vec4( frameArray[0].get( 0.00f ),
                                 frameArray[1].get( 0.00f ),
                                 frameArray[2].get( 0.00f ),
                                 frameArray[3].get( 0.30f ) );
  colours.background     = Vec4( backgroundArray[0].get( 0.05f ),
                                 backgroundArray[1].get( 0.05f ),
                                 backgroundArray[2].get( 0.05f ),
                                 backgroundArray[3].get( 1.00f ) );

  colours.menuStrip      = Vec4( menuStripArray[0].get( 0.00f ),
                                 menuStripArray[1].get( 0.00f ),
                                 menuStripArray[2].get( 0.00f ),
                                 menuStripArray[3].get( 1.00f ) );

  const JSON& fontsConfig = config["fonts"];

  for( int i = 0; i < Font::MAX; ++i ) {
    const JSON& fontConfig = fontsConfig[ FONT_INFOS[i].key ];

    const char* name   = fontConfig["name"].get( FONT_INFOS[i].name );
    int         height = fontConfig["height"].get( FONT_INFOS[i].height );

    fonts[i].init( name, height );
  }

  Log::printEnd( " OK" );
}

void Style::free()
{
  for( int i = 0; i < Font::MAX; ++i ) {
    fonts[i].free();
  }
}

Style style;

}
}
}
