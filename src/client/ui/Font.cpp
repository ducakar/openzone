/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file client/ui/Font.cpp
 */

#include "stable.hpp"

#include "client/ui/Font.hpp"

#include "client/OpenGL.hpp"

#include <SDL/SDL_ttf.h>

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
};

Font::Font() : textTexId( 0 )
{
  for( int i = 0; i < MAX; ++i ) {
    fonts[i] = null;
  }
}

bool Font::init()
{
  const char* path;

  if( TTF_Init() == -1 ) {
    return false;
  }

  for( int i = 0; i < MAX; ++i ) {
    path = INFOS[i].file;

    log.print( "Opening font '%s' %d px ...", path, INFOS[i].height );

    fonts[i] = TTF_OpenFont( path, INFOS[i].height );
    if( fonts[i] == null ) {
      log.printEnd( " Error: %s", TTF_GetError() );
      return false;
    }

    log.printEnd( " OK" );
  }

  log.print( "Generating text texture ..." );

  glGenTextures( 1, &textTexId );
  glBindTexture( GL_TEXTURE_2D, textTexId );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glBindTexture( GL_TEXTURE_2D, 0 );

  log.printEnd( " OK" );

  return true;
}

void Font::free()
{
  log.print( "Deleting text texture ..." );

  glDeleteTextures( 1, &textTexId );
  textTexId = 0;

  log.printEnd( " OK" );

  log.print( "Closing fonts ..." );

  if( TTF_WasInit() == 0 ) {
    log.printEnd( " Not initialised" );
    return;
  }

  for( int i = 0; i < MAX; ++i ) {
    if( fonts[i] == null ) {
      TTF_CloseFont( fonts[i] );
      fonts[i] = null;
    }
  }

  TTF_Quit();

  log.printEnd( " OK" );
}

}
}
}
