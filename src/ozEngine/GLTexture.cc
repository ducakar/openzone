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
 * @file ozEngine/GLTexture.cc
 */

#include "GLTexture.hh"

#include "GL.hh"

namespace oz
{

GLTexture::GLTexture() :
  textureId( 0 ), textureMipmaps( 0 )
{}

GLTexture::GLTexture( const File& file ) :
  textureId( 0 ), textureMipmaps( 0 )
{
  load( file );
}

GLTexture::~GLTexture()
{
  destroy();
}

bool GLTexture::create()
{
  if( textureId == 0 ) {
    glGenTextures( 1, &textureId );
  }
  return textureId != 0;
}

bool GLTexture::load( const File& file, int bias )
{
  create();

  if( textureId == 0 ) {
    return false;
  }

  glBindTexture( GL_TEXTURE_2D, textureId );
  textureMipmaps = GL::textureDataFromFile( file, bias );

  if( textureMipmaps == 0 ) {
    destroy();
    return false;
  }
  return true;
}

bool GLTexture::generateIdenticon( int size, int hash, const Vec4& backgroundColour )
{
  create();

  if( textureId == 0 ) {
    return false;
  }

  textureMipmaps = 1;

  bool grid[5][5];

  // Fill 5 x 5 grid from hash.
  for( int x = 0; x < 3; ++x ) {
    for( int y = 0; y < 5; ++y ) {
      bool value = hash & ( 1 << ( x * 5 + y ) );

      grid[x][y]     = value;
      grid[4 - x][y] = value;
    }
  }

  char bg[3] = {
    char( Math::lround( backgroundColour.x * 255.0f ) ),
    char( Math::lround( backgroundColour.y * 255.0f ) ),
    char( Math::lround( backgroundColour.z * 255.0f ) )
  };

  char rgb[3] = {
    char( 0x60 + ( ( uint( hash ) >> 13 ) & 0x7c ) ),
    char( 0x60 + ( ( uint( hash ) >> 20 ) & 0x7e ) ),
    char( 0x60 + ( ( uint( hash ) >> 25 ) & 0x7c ) )
  };

  int   fieldSize = size / 6;
  int   fieldHalf = fieldSize / 2;
  int   pitch     = ( ( size * 3 + 3 ) / 4 ) * 4;
  char* data      = new char[size * pitch];

  for( int i = 0; i < size; ++i ) {
    char* pixel = data + i * pitch;

    for( int j = 0; j < size; ++j ) {
      int x = j - fieldHalf;
      int y = i - fieldHalf;

      x = x < 0 ? 5 : x / fieldSize;
      y = y < 0 ? 5 : y / fieldSize;

      if( x < 5 && y < 5 && grid[x][y] ) {
        pixel[0] = rgb[0];
        pixel[1] = rgb[1];
        pixel[2] = rgb[2];
      }
      else {
        pixel[0] = bg[0];
        pixel[1] = bg[1];
        pixel[2] = bg[2];
      }

      pixel += 3;
    }
  }

  glBindTexture( GL_TEXTURE_2D, textureId );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data );

  delete[] data;
  return true;
}

void GLTexture::destroy()
{
  if( textureId != 0 ) {
    glDeleteTextures( 1, &textureId );

    textureId      = 0;
    textureMipmaps = 0;

    OZ_GL_CHECK_ERROR();
  }
}

}
