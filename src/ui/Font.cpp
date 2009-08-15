/*
 *  Font.cpp
 *
 *  Bitmap font loader and print function
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Font.h"

#include <stdarg.h>

namespace oz
{
namespace client
{
namespace ui
{

  Font font;

  bool Font::init()
  {
    const char *path;

    if( TTF_Init() == -1 ) {
      return false;
    }

    path = config.get( "ui.font.mono.file", "/usr/share/fonts/TTF/DejaVuSansMono.ttf" );
    monoHeight = config.get( "ui.font.mono.height", 16 );

    log.print( "Opening font '%s' %d px ...", path, monoHeight );
    monoFont = TTF_OpenFont( path, monoHeight );
    if( monoFont == null ) {
      log.printEnd( " %s", TTF_GetError() );
      return false;
    }
    monoHeight = TTF_FontHeight( monoFont );

    path = config.get( "ui.font.sans.file", "/usr/share/fonts/TTF/DejaVuSans.ttf" );
    sansHeight = config.get( "ui.font.sans.height", 16 );

    log.print( "Opening font '%s' %px ...", path, sansHeight );
    sansFont = TTF_OpenFont( path, sansHeight );
    if( sansFont == null ) {
      log.printEnd( " %s", TTF_GetError() );
      TTF_CloseFont( monoFont );
      monoFont = null;
      return false;
    }
    sansHeight = TTF_FontHeight( sansFont );

    log.printEnd( " OK" );
    return true;
  }

  void Font::free()
  {
    log.print( "Closing font ..." );

    if( TTF_WasInit() == 0 ) {
      log.printEnd( " Not initialized" );
      return;
    }
    if( monoFont != null ) {
      TTF_CloseFont( monoFont );
      monoFont = null;
    }
    if( sansFont != null ) {
      TTF_CloseFont( sansFont );
      sansFont = null;
    }
    TTF_Quit();

    log.printEnd( " OK" );
  }

}
}
}
