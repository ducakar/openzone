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

  void Font::print( float x, float y, const char *s, ... )
  {
    char buffer[1024];
    va_list ap;

    va_start( ap, s );
    vsnprintf( buffer, 1024, s, ap );
    va_end( ap );
    buffer[1023] = '\0';

    x = x < 0.0f ? screenX + x : x;
    y = y < 0.0f ? screenY + y : y;

    SDL_Surface *original = TTF_RenderUTF8_Shaded( currentFont, buffer, fgColor, bgColor );
    SDL_Surface *rgba = SDL_CreateRGBSurface( 0, original->w, original->h, 32,
                                              0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 );
    SDL_BlitSurface( original, null, rgba, null );

    GLuint texture;
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    assert( glGetError() == GL_NO_ERROR );

    glTexImage2D( GL_TEXTURE_2D, 0, 4, rgba->w, rgba->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                  rgba->pixels );

    if( glGetError() == GL_NO_ERROR ) {
      glBegin( GL_QUADS );
        glTexCoord2i( 0, 1 );
        glVertex2f( x          , y           );
        glTexCoord2i( 1, 1 );
        glVertex2f( x + rgba->w, y           );
        glTexCoord2i( 1, 0 );
        glVertex2f( x + rgba->w, y + rgba->h );
        glTexCoord2i( 0, 0 );
        glVertex2f( x          , y + rgba->h );
      glEnd();

      glDeleteTextures( 1, &texture );
    }
    else {
      assert( glGetError() == GL_NO_ERROR );
    }

    SDL_FreeSurface( rgba );
    SDL_FreeSurface( original );
  }

  bool Font::init( float screenX_, float screenY_ )
  {
    const char *path;

    screenX = screenX_;
    screenY = screenY_;

    fgColor.r      = 0xff;
    fgColor.g      = 0xff;
    fgColor.b      = 0xff;
    fgColor.unused = 0xff;

    bgColor.r      = 0x00;
    bgColor.g      = 0x00;
    bgColor.b      = 0x00;
    bgColor.unused = 0x00;

    if( TTF_Init() == -1 ) {
      return false;
    }

    path = config.get( "ui.font.mono.file", "/usr/share/fonts/TTF/DejaVuSansMono.ttf" );
    monoHeight = config.get( "ui.font.mono.height", 16 );

    logFile.print( "Opening font '%s' %d px ...", path, monoHeight );
    monoFont = TTF_OpenFont( path, monoHeight );
    if( monoFont == null ) {
      logFile.printEnd( " %s", TTF_GetError() );
      return false;
    }
    monoHeight = TTF_FontHeight( monoFont );

    path = config.get( "ui.font.sans.file", "/usr/share/fonts/TTF/DejaVuSans.ttf" );
    sansHeight = config.get( "ui.font.sans.height", 16 );

    logFile.print( "Opening font '%s' %px ...", path, sansHeight );
    sansFont = TTF_OpenFont( path, sansHeight );
    if( sansFont == null ) {
      logFile.printEnd( " %s", TTF_GetError() );
      TTF_CloseFont( monoFont );
      monoFont = null;
      return false;
    }
    sansHeight = TTF_FontHeight( sansFont );

    currentFont = monoFont;

    logFile.printEnd( " OK" );
    return true;
  }

  void Font::free()
  {
    logFile.print( "Closing font ..." );

    if( TTF_WasInit() == 0 ) {
      logFile.printEnd( " Not initialized" );
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

    logFile.printEnd( " OK" );
  }

}
}
}
