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

#include "OpenGL.hh"

namespace oz
{

GLTexture::GLTexture() :
  textureId( 0 )
{}

GLTexture::~GLTexture()
{
  destroy();
}

bool GLTexture::load( InputStream* istream )
{
  destroy();

  const char* begin = istream->pos();

  glGenTextures( 1, &textureId );
  glBindTexture( GL_TEXTURE_2D, textureId );

  int wrap      = istream->readInt();
  int magFilter = istream->readInt();
  int minFilter = istream->readInt();

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );

  for( int level = 0; ; ++level ) {
    int width = istream->readInt();
    if( width == 0 ) {
      break;
    }

    int height = istream->readInt();
    int format = istream->readInt();
    int size   = istream->readInt();

    if( format == GL_COMPRESSED_RGB_S3TC_DXT1_EXT || format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT ) {
      glCompressedTexImage2D( GL_TEXTURE_2D, level, uint( format ), width, height, 0,
                              size, istream->forward( size ) );
    }
    else {
      glTexImage2D( GL_TEXTURE_2D, level, format, width, height, 0, uint( format ),
                    GL_UNSIGNED_BYTE, istream->forward( size ) );
    }
  }

  if( glGetError() != GL_NO_ERROR ) {
    glDeleteTextures( 1, &textureId );
    textureId = 0;

    istream->set( begin );
    return false;
  }
  else {
    return true;
  }
}

void GLTexture::destroy()
{
  if( textureId != 0 ) {
    glDeleteTextures( 1, &textureId );
    textureId = 0;

    OZ_GL_CHECK_ERROR();
  }
}

}
