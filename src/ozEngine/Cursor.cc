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

namespace oz
{

Cursor::Cursor() :
  images{}, nImages( 0 ), frame( 0 ), frameTime( 0 )
{}

Cursor::Cursor( const oz::File& file, int size ) :
  images{}, nImages( 0 ), frame( 0 ), frameTime( 0 )
{
  load( file, size );
}

Cursor::~Cursor()
{
  destroy();
}

Cursor::Cursor( Cursor&& c ) :
  nImages( c.nImages ), frame( c.frame ), frameTime( c.frameTime )
{
  aCopy<Image>( images, c.images, MAX_IMAGES );

  aFill<Image>( c.images, Image{}, MAX_IMAGES );
  c.nImages   = 0;
  c.frame     = 0;
  c.frameTime = 0;
}

Cursor& Cursor::operator = ( Cursor&& c )
{
  if( &c == this ) {
    return *this;
  }

  aCopy<Image>( images, c.images, MAX_IMAGES );
  nImages   = c.nImages;
  frame     = c.frame;
  frameTime = c.frameTime;

  aFill<Image>( c.images, Image{}, MAX_IMAGES );
  c.nImages   = 0;
  c.frame     = 0;
  c.frameTime = 0;

  return *this;
}

void Cursor::reset()
{
  frame     = 0;
  frameTime = 0;
}

void Cursor::advance( int millis )
{
  if( nImages == 0 ) {
    return;
  }

  int delay = images[frame].delay;

  frameTime += millis;
  frame      = ( frame + frameTime / delay ) % nImages;
  frameTime  = frameTime % delay;
}

bool Cursor::load( const File& file, int size )
{
  Buffer      buffer;
  InputStream istream;

  if( file.isMapped() ) {
    istream = file.inputStream();
  }
  else {
    buffer  = file.read();
    istream = buffer.inputStream();
  }

  // Implementation is based on specifications from xcursor(3) manual.
  if( !istream.isAvailable() || !String::beginsWith( istream.begin(), "Xcur" ) ) {
    return false;
  }

  istream.seek( 12 );
  int nEntries = istream.readInt();

  nImages   = 0;
  frame     = 0;
  frameTime = 0;

  for( int i = 0; i < nEntries && nImages < MAX_IMAGES; ++i ) {
    uint type     = istream.readUInt();
    int  subtype  = istream.readInt();
    int  position = istream.readInt();

    if( type != 0xfffd0002 ) {
      continue;
    }
    else if( size == -1 ) {
      size = subtype;
    }
    else if( subtype != size ) {
      continue;
    }

    int tablePos = istream.tell();
    istream.seek( position );

    istream.readInt();
    istream.readInt();
    istream.readInt();
    istream.readInt();

    Image& image = images[nImages];
    ++nImages;

    image.width       = istream.readInt();
    image.height      = istream.readInt();
    image.hotspotLeft = istream.readInt();
    image.hotspotTop  = istream.readInt();
    image.delay       = istream.readInt();

    int size = image.width*image.height * 4;

    char* pixels = new char[size];
    istream.readChars( pixels, size );

    // BGRA -> RGBA
    char* pixel = pixels;
    for( int y = 0; y < image.height; ++y ) {
      for( int x = 0; x < image.width; ++x ) {
        swap( pixel[0], pixel[2] );
        pixel += 4;
      }
    }

    glGenTextures( 1, &image.textureId );
    glBindTexture( GL_TEXTURE_2D, image.textureId );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA,
                  GL_UNSIGNED_BYTE, pixels );

    delete[] pixels;

    istream.seek( tablePos );
  }

  return nImages != 0;
}

void Cursor::destroy()
{
  if( nImages == 0 ) {
    return;
  }

  for( int i = 0; i < nImages; ++i ) {
    glDeleteTextures( 1, &images[i].textureId );
  }

  aFill<Image>( images, Image{}, MAX_IMAGES );
  nImages   = 0;
  frame     = 0;
  frameTime = 0;
}

}
