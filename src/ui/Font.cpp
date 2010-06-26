/*
 *  Font.cpp
 *
 *  Bitmap font loader and print function
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#include "stable.hpp"

#include "ui/Font.hpp"

#include <cstdarg>

namespace oz
{
namespace client
{
namespace ui
{

  Font font;

  bool Font::init()
  {
    const char* path;

    if( TTF_Init() == -1 ) {
      return false;
    }

    path = config.getSet( "ui.font.mono.file", "fonts/DejaVuSansMono.ttf" );
    monoHeight = config.getSet( "ui.font.mono.height", 12 );

    log.print( "Opening font '%s' %d px ...", path, monoHeight );
    monoFont = TTF_OpenFont( path, monoHeight );
    if( monoFont == null ) {
      log.printEnd( " Error: %s", TTF_GetError() );
      return false;
    }
    log.printEnd( " OK" );

    path = config.getSet( "ui.font.sans.file", "fonts/DejaVuSans.ttf" );
    sansHeight = config.getSet( "ui.font.sans.height", 11 );

    log.print( "Opening font '%s' %d px ...", path, sansHeight );
    sansFont = TTF_OpenFont( path, sansHeight );
    if( sansFont == null ) {
      log.printEnd( " Error: %s", TTF_GetError() );
      TTF_CloseFont( monoFont );
      monoFont = null;
      return false;
    }
    log.printEnd( " OK" );

    path = config.getSet( "ui.font.title.file", "fonts/DejaVuSans.ttf" );
    titleHeight = config.getSet( "ui.font.title.height", 14 );

    log.print( "Opening font '%s' %d px ...", path, titleHeight );
    titleFont = TTF_OpenFont( path, titleHeight );
    if( titleHeight == 0 ) {
      log.printEnd( " Error: %s", TTF_GetError() );
      TTF_CloseFont( monoFont );
      monoFont = null;
      TTF_CloseFont( sansFont );
      sansFont = null;
      return false;
    }
    log.printEnd( " OK" );

    return true;
  }

  void Font::free()
  {
    log.print( "Closing fonts ..." );

    if( TTF_WasInit() == 0 ) {
      log.printEnd( " Not initialised" );
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
    if( titleFont != null ) {
      TTF_CloseFont( titleFont );
      titleFont = null;
    }
    TTF_Quit();

    log.printEnd( " OK" );
  }

}
}
}
