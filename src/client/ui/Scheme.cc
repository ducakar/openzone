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
 * @file client/ui/Scheme.cc
 */

#include "stable.hh"

#include "client/ui/Scheme.hh"

namespace oz
{
namespace client
{
namespace ui
{

Scheme scheme;

void Scheme::init()
{
  Log::print( "Initialising Scheme ..." );

  PFile configFile( "ui/scheme.json" );

  JSON config;
  config.load( &configFile );

  const JSON& textArray           = config["text"];
  const JSON& textBackgroundArray = config["textBackground"];

  const JSON& buttonArray         = config["button"];
  const JSON& buttonHoverArray    = config["buttonHover"];
  const JSON& buttonClickedArray  = config["buttonClicked"];

  const JSON& tileArray           = config["tile"];
  const JSON& tileHoverArray      = config["tileHover"];

  const JSON& frameArray          = config["frame"];
  const JSON& backgroundArray     = config["background"];

  const JSON& menuStripArray      = config["menuStrip"];

  text           = Vec4( textArray[0].get( 1.00f ),
                         textArray[1].get( 1.00f ),
                         textArray[2].get( 1.00f ),
                         textArray[3].get( 1.00f ) );
  textBackground = Vec4( textBackgroundArray[0].get( 0.00f ),
                         textBackgroundArray[1].get( 0.00f ),
                         textBackgroundArray[2].get( 0.00f ),
                         textBackgroundArray[3].get( 1.00f ) );

  button         = Vec4( buttonArray[0].get( 0.20f ),
                         buttonArray[1].get( 0.20f ),
                         buttonArray[2].get( 0.20f ),
                         buttonArray[3].get( 0.40f ) );
  buttonHover    = Vec4( buttonHoverArray[0].get( 0.60f ),
                         buttonHoverArray[1].get( 0.60f ),
                         buttonHoverArray[2].get( 0.60f ),
                         buttonHoverArray[3].get( 0.40f ) );
  buttonClicked  = Vec4( buttonClickedArray[0].get( 1.00f ),
                         buttonClickedArray[1].get( 1.00f ),
                         buttonClickedArray[2].get( 1.00f ),
                         buttonClickedArray[3].get( 0.40f ) );

  tile           = Vec4( tileArray[0].get( 0.40f ),
                         tileArray[1].get( 0.40f ),
                         tileArray[2].get( 0.40f ),
                         tileArray[3].get( 0.60f ) );
  tileHover      = Vec4( tileHoverArray[0].get( 0.60f ),
                         tileHoverArray[1].get( 0.60f ),
                         tileHoverArray[2].get( 0.60f ),
                         tileHoverArray[3].get( 0.60f ) );

  frame          = Vec4( frameArray[0].get( 0.00f ),
                         frameArray[1].get( 0.00f ),
                         frameArray[2].get( 0.00f ),
                         frameArray[3].get( 0.30f ) );
  background     = Vec4( backgroundArray[0].get( 0.05f ),
                         backgroundArray[1].get( 0.05f ),
                         backgroundArray[2].get( 0.05f ),
                         backgroundArray[3].get( 1.00f ) );

  menuStrip      = Vec4( menuStripArray[0].get( 0.00f ),
                         menuStripArray[1].get( 0.00f ),
                         menuStripArray[2].get( 0.00f ),
                         menuStripArray[3].get( 1.00f ) );

  Log::printEnd( " OK" );
}

void Scheme::free()
{}

}
}
}
