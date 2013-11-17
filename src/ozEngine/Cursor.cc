/*
 * ozEngine - OpenZone Engine Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file ozEngine/Cursor.cc
 */

#include "Cursor.hh"

#include "GL.hh"

#include <SDL.h>

namespace oz
{

Cursor::Cursor() :
  images{}, nImages( 0 ), frame( 0 ), lastFrame( -1 ), frameTime( 0 ), mode( TEXTURE )
{}

Cursor::Cursor( const File& file, Mode mode, int size ) :
  images{}, nImages( 0 ), frame( 0 ), lastFrame( -1 ), frameTime( 0 ), mode( TEXTURE )
{
  load( file, mode, size );
}

Cursor::~Cursor()
{
  destroy();
}

Cursor::Cursor( Cursor&& c ) :
  nImages( c.nImages ), frame( c.frame ), lastFrame( -1 ), frameTime( c.frameTime ), mode( c.mode )
{
  aCopy<Image>( c.images, MAX_IMAGES, images );

  aFill<Image>( c.images, MAX_IMAGES, Image() );
  c.nImages   = 0;
  c.frame     = 0;
  c.lastFrame = -1;
  c.frameTime = 0;
  c.mode      = TEXTURE;
}

Cursor& Cursor::operator = ( Cursor&& c )
{
  if( &c == this ) {
    return *this;
  }

  aCopy<Image>( c.images, MAX_IMAGES, images );
  nImages   = c.nImages;
  frame     = c.frame;
  lastFrame = c.lastFrame;
  frameTime = c.frameTime;
  mode      = c.mode;

  aFill<Image>( c.images, MAX_IMAGES, Image() );
  c.nImages   = 0;
  c.frame     = 0;
  c.lastFrame = -1;
  c.frameTime = 0;
  c.mode      = TEXTURE;

  return *this;
}

void Cursor::reset()
{
  frame     = 0;
  frameTime = 0;
}

void Cursor::update( int millis )
{
  if( nImages == 0 ) {
    return;
  }

  int delay = images[frame].delay;

  frameTime += millis;
  frame      = ( frame + frameTime / delay ) % nImages;
  frameTime  = frameTime % delay;

  if( mode == SYSTEM && frame != lastFrame && nImages != 0 ) {
    lastFrame = frame;

    SDL_SetCursor( images[frame].sdlCursor );
  }
}

bool Cursor::load( const File& file, Mode mode_, int size )
{
#if SDL_MAJOR_VERSION < 2
  // SDL 1.2 only supports monochromatic cursors.
  if( mode_ == SYSTEM ) {
    mode = mode_;
    return false;
  }
#endif

  InputStream is = file.inputStream( Endian::LITTLE );

  // Implementation is based on specifications from xcursor(3) manual.
  if( !is.isAvailable() || !String::beginsWith( is.begin(), "Xcur" ) ) {
    return false;
  }

  is.seek( 12 );
  int nEntries = is.readInt();

  nImages   = 0;
  frame     = 0;
  frameTime = 0;
  mode      = mode_;

  for( int i = 0; i < nEntries && nImages < MAX_IMAGES; ++i ) {
    uint type     = is.readUInt();
    int  subtype  = is.readInt();
    int  position = is.readInt();

    if( type != 0xfffd0002 ) {
      continue;
    }
    else if( size == -1 ) {
      size = subtype;
    }
    else if( subtype != size ) {
      continue;
    }

    int tablePos = is.tell();
    is.seek( position );

    is.readInt();
    is.readInt();
    is.readInt();
    is.readInt();

    Image& image = images[nImages];
    ++nImages;

    image.width       = is.readInt();
    image.height      = is.readInt();
    image.hotspotLeft = is.readInt();
    image.hotspotTop  = is.readInt();
    image.delay       = is.readInt();
    image.sdlCursor   = nullptr;

    int size = image.width * image.height * 4;

    char* pixels = new char[size];
    is.readChars( pixels, size );

    if( mode == TEXTURE ) {
#ifdef GL_ES_VERSION_2_0
      GLenum srcFormat = GL_RGBA;

      // BGRA -> RGBA
      char* pixel = pixels;
      for( int y = 0; y < image.height; ++y ) {
        for( int x = 0; x < image.width; ++x ) {
          swap( pixel[0], pixel[2] );
          pixel += 4;
        }
      }
#else
      GLenum srcFormat = GL_BGRA;
#endif

      glGenTextures( 1, &image.textureId );
      glBindTexture( GL_TEXTURE_2D, image.textureId );

      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, srcFormat,
                    GL_UNSIGNED_BYTE, pixels );
    }
    else {
#if SDL_MAJOR_VERSION >= 2
      SDL_Surface* surface = SDL_CreateRGBSurfaceFrom( pixels, image.width, image.height, 32,
                                                       image.width * 4, 0x00ff0000, 0x0000ff00,
                                                       0x000000ff, 0xff000000 );

      image.sdlCursor = SDL_CreateColorCursor( surface, image.hotspotLeft, image.hotspotTop );

      SDL_FreeSurface( surface );
#endif
    }

    delete[] pixels;

    is.seek( tablePos );
  }

  return nImages != 0;
}

void Cursor::destroy()
{
  if( nImages == 0 ) {
    return;
  }

  for( int i = 0; i < nImages; ++i ) {
    if( mode == TEXTURE ) {
      glDeleteTextures( 1, &images[i].textureId );
    }
    else {
      SDL_FreeCursor( images[i].sdlCursor );
    }
  }

  aFill<Image>( images, MAX_IMAGES, Image() );
  nImages   = 0;
  frame     = 0;
  lastFrame = -1;
  frameTime = 0;
  mode      = TEXTURE;
}

}
