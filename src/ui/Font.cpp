/*
 *  Font.cpp
 *
 *  Bitmap font loader and print function
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "ui/Font.hpp"

#include <SDL_ttf.h>
#include <GL/gl.h>

namespace oz
{
namespace client
{
namespace ui
{

  Font font;

  const Font::Info Font::INFOS[MAX] = {
    { "mono",   "ui/font/DejaVuSansMono.ttf", 13 },
    { "sans",   "ui/font/DejaVuSans.ttf",     13 },
    { "small",  "ui/font/DejaVuSans.ttf",     11 },
    { "large",  "ui/font/DejaVuSans.ttf",     14 },
    { "title",  "ui/font/DejaVuSans.ttf",     15 },
    { "symbol", "ui/font/DejaVuSans.ttf",     14 }
  };

  bool Font::init()
  {
    const char* path;

    if( TTF_Init() == -1 ) {
      return false;
    }

    textTexId = 0;

    for( int i = 0; i < MAX; ++i ) {
      path = config.getSet( "ui.font." + String( INFOS[i].name ) + ".file", INFOS[i].file );

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
