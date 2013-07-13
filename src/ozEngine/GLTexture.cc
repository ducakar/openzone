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
  destroy();
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
