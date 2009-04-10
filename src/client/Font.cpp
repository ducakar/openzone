/*
 *  Font.cpp
 *
 *  Bitmap font loader and print function
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Font.h"

namespace oz
{
namespace client
{

  Font::Font() : texture( null ), baseList( null )
  {}

  Font::~Font()
  {
    free();
  }

  uint Font::loadTexture( const char *fileName )
  {
    SDL_Surface *image = IMG_Load( fileName );
    if( image == null ) {
      logFile.printEnd( "No such file" );
      return 0;
    }

    GLint bytesPerPixel = image->format->BitsPerPixel / 8;
    GLenum format = bytesPerPixel == 4 ? GL_RGBA : GL_RGB;

    uint texNum;
    glGenTextures( 1, &texNum );
    glBindTexture( GL_TEXTURE_2D, texNum );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    glTexImage2D( GL_TEXTURE_2D, 0, bytesPerPixel, image->w, image->h, 0, format,
                  GL_UNSIGNED_BYTE, image->pixels );

    SDL_FreeSurface( image );

    return texNum;
  }

  void Font::init( const char *fileName, float scale )
  {
    logFile.print( "Loading fonts '%s' ... ", fileName );
    texture = loadTexture( fileName );
    if( texture == 0 ) {
      return;
    }

    float width = FONT_WIDTH * scale;
    float height = FONT_HEIGHT * scale;

    baseList = glGenLists( 256 );

    for( int y = 0; y < 16; y++ ) {
      for( int x = 0; x < 16; x++ ) {
        glNewList( baseList + ( 15 - y ) * 16 + x, GL_COMPILE );
        glBindTexture( GL_TEXTURE_2D, texture );
        glBegin( GL_QUADS );
        glTexCoord2f( x / 16.0f         + FONT_BIAS_X,  y / 16.0f        + FONT_BIAS_Y );
        glVertex3f( 0, 0, 0 );
        glTexCoord2f( ( x + 1 ) / 16.0f - FONT_BIAS_X,  y / 16.0f        + FONT_BIAS_Y );
        glVertex3f( width, 0, 0 );
        glTexCoord2f( ( x + 1 ) / 16.0f - FONT_BIAS_X, ( y + 1 ) / 16.0f - FONT_BIAS_Y );
        glVertex3f( width, height, 0 );
        glTexCoord2f( x / 16.0f         + FONT_BIAS_X, ( y + 1 ) / 16.0f - FONT_BIAS_Y );
        glVertex3f( 0, height, 0 );
        glEnd();
        glTranslatef( width, 0, 0 );
        glEndList();
      }
    }
    baseList -= 32;
    logFile.printEnd( "OK" );
  }

  void Font::print( float x, float y, const char *string, ... ) const
  {
    va_list ap;
    char buf[256];

    va_start( ap, string );
    vsprintf( buf, string, ap );
    va_end( ap );

    glBindTexture( GL_TEXTURE_2D, texture );
    glPushMatrix();
    glLoadIdentity();

    glTranslatef( x, y, -50.0f );
    glListBase( baseList );
    glCallLists( String::length( buf ), GL_BYTE, buf );

    glPopMatrix();
  }

  void Font::free()
  {
    if( baseList != 0 ) {
      glDeleteLists( baseList, 256 );
      baseList = 0;
    }

    if( texture != 0 ) {
      glDeleteTextures( 1, &texture );
      texture = 0;
    }
  }

}
}
