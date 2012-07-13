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
 * @file client/ui/Font.cc
 */

#include "stable.hh"

#include "client/ui/Font.hh"

#include "client/OpenGL.hh"

#include <SDL_ttf.h>

namespace oz
{
namespace client
{
namespace ui
{

Font font;

const SDL_Colour Font::SDL_COLOUR_WHITE = { 0xff, 0xff, 0xff, 0xff };

const Font::Info Font::INFOS[MAX] = {
  { "ui/font/DroidSansMono.ttf", 13 },
  { "ui/font/DroidSans.ttf",     13 },
  { "ui/font/DroidSans.ttf",     11 },
  { "ui/font/DroidSans.ttf",     14 },
  { "ui/font/DroidSans.ttf",     18 },
};

void Font::load()
{
  Log::print( "Loading fonts ..." );

  if( TTF_Init() < 0 ) {
    throw Exception( "Failed to initialise SDL_TTF" );
  }

  for( int i = 0; i < MAX; ++i ) {
    fontFile[i].setPath( INFOS[i].file );

    if( !fontFile[i].map() ) {
      throw Exception( "Failed to read font file '%s'", fontFile[i].path().cstr() );
    }

    InputStream istream = fontFile[i].inputStream();

    fonts[i] = TTF_OpenFontRW( SDL_RWFromConstMem( istream.begin(), istream.capacity() ), true,
                               INFOS[i].height );
    if( fonts[i] == null ) {
      throw Exception( "%s", TTF_GetError() );
    }

  }

  Log::printEnd( " OK" );
}

void Font::unload()
{
  Log::print( "Unloading fonts ..." );

  for( int i = 0; i < MAX; ++i ) {
    if( fonts[i] != null ) {
      TTF_CloseFont( fonts[i] );
      fonts[i] = null;

      fontFile[i].unmap();
      fontFile[i].setPath( "" );
    }
  }

  TTF_Quit();

  Log::printEnd( " OK" );
}

}
}
}
