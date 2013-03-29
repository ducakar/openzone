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

namespace oz
{

#if 0
bool GLTexture::build( const File& file, int options, OutputStream* ostream )
{
  if( file.isVirtual() ) {
    return false;
  }

  int  width      = image->width;
  int  height     = image->height;
  bool genMipmaps = minFilter == GL_NEAREST_MIPMAP_NEAREST ||
                    minFilter == GL_LINEAR_MIPMAP_NEAREST ||
                    minFilter == GL_NEAREST_MIPMAP_LINEAR ||
                    minFilter == GL_LINEAR_MIPMAP_LINEAR;

  if( genMipmaps && ( !Math::isPow2( width ) || !Math::isPow2( height ) ) ) {
    OZ_ERROR( "Image has dimensions %dx%d but both dimensions must be powers of two to generate"
              " mipmaps.", width, height );
  }

  do {
    levels.add();
    Level& level = levels.last();

    level.width  = width;
    level.height = height;

    FIBITMAP* levelDib = image->dib;
    if( levels.length() > 1 ) {
      levelDib = FreeImage_Rescale( image->dib, width, height,
                                    context.isHighQuality ? FILTER_CATMULLROM : FILTER_BOX );
    }

#ifdef OZ_NONFREE
    int squishFlags = context.isHighQuality ?
                      squish::kColourIterativeClusterFit | squish::kWeightColourByAlpha :
                      squish::kColourRangeFit;
#endif

    switch( image->format ) {
      case GL_LUMINANCE: {
        if( context.useS3TC ) {
#ifdef OZ_NONFREE
          // Collapse data (pitch = width * pixelSize) and convert LUMINANCE -> RGBA.
          ubyte* data = new ubyte[height * width * 4];

          for( int y = 0; y < height; ++y ) {
            ubyte* srcLine = FreeImage_GetScanLine( levelDib, y );
            ubyte* dstLine = &data[y * width*4];

            for( int x = 0; x < width; ++x ) {
              dstLine[x*4 + 0] = srcLine[x];
              dstLine[x*4 + 1] = srcLine[x];
              dstLine[x*4 + 2] = srcLine[x];
              dstLine[x*4 + 3] = UCHAR_MAX;
            }
          }

          level.format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
          level.size   = squish::GetStorageRequirements( width, height, squish::kDxt1 );
          level.data   = new ubyte[level.size];

          squish::CompressImage( data, width, height, level.data, squish::kDxt1 | squishFlags );
          delete[] data;
#endif
        }
        else {
          level.format = GL_LUMINANCE;
          level.size   = width * height;
          level.data   = new ubyte[level.size];

          for( int y = 0; y < height; ++y ) {
            mCopy( level.data + y*width, FreeImage_GetScanLine( levelDib, y ), size_t( width ) );
          }
        }
        break;
      }
      case GL_RGB: {
        if( context.useS3TC ) {
#ifdef OZ_NONFREE
          // Collapse data (pitch = width * pixelSize) and convert BGR -> RGBA.
          ubyte* data = new ubyte[height * width * 4];

          for( int y = 0; y < height; ++y ) {
            ubyte* srcLine = FreeImage_GetScanLine( levelDib, y );
            ubyte* dstLine = &data[y * width*4];

            for( int x = 0; x < width; ++x ) {
              dstLine[x*4 + 0] = srcLine[x*3 + 2];
              dstLine[x*4 + 1] = srcLine[x*3 + 1];
              dstLine[x*4 + 2] = srcLine[x*3 + 0];
              dstLine[x*4 + 3] = UCHAR_MAX;
            }
          }

          level.format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
          level.size   = squish::GetStorageRequirements( width, height, squish::kDxt1 );
          level.data   = new ubyte[level.size];

          squish::CompressImage( data, width, height, level.data, squish::kDxt1 | squishFlags );
          delete[] data;
#endif
        }
        else {
          ubyte* data  = FreeImage_GetBits( levelDib );
          int    pitch = int( FreeImage_GetPitch( levelDib ) );

          level.format = GL_RGB;
          level.size   = height * pitch;
          level.data   = new ubyte[level.size];

          for( int y = 0; y < level.height; ++y ) {
            ubyte* srcLine = &data[y * pitch];
            ubyte* dstLine = &level.data[y * pitch];

            for( int x = 0; x + 2 < pitch; x += 3 ) {
              dstLine[x + 0] = srcLine[x + 2];
              dstLine[x + 1] = srcLine[x + 1];
              dstLine[x + 2] = srcLine[x + 0];
            }
          }
        }
        break;
      }
      case GL_RGBA: {
        if( context.useS3TC ) {
#ifdef OZ_NONFREE
          // Collapse data (pitch = width * pixelSize) and convert BGRA -> RGBA.
          ubyte* data = new ubyte[height * width * 4];

          for( int y = 0; y < height; ++y ) {
            ubyte* srcLine = FreeImage_GetScanLine( levelDib, y );
            ubyte* dstLine = &data[y * width*4];

            for( int x = 0; x < width; ++x ) {
              dstLine[x*4 + 0] = srcLine[x*4 + 2];
              dstLine[x*4 + 1] = srcLine[x*4 + 1];
              dstLine[x*4 + 2] = srcLine[x*4 + 0];
              dstLine[x*4 + 3] = srcLine[x*4 + 3];
            }
          }

          level.format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
          level.size   = squish::GetStorageRequirements( width, height, squish::kDxt5 );
          level.data   = new ubyte[level.size];

          squish::CompressImage( data, width, height, level.data, squish::kDxt5 | squishFlags );
          delete[] data;
#endif
        }
        else {
          ubyte* data  = FreeImage_GetBits( levelDib );
          int    pitch = int( FreeImage_GetPitch( levelDib ) );

          level.format = GL_RGBA;
          level.size   = height * pitch;
          level.data   = new ubyte[level.size];

          for( int y = 0; y < level.height; ++y ) {
            ubyte* srcLine = &data[y * pitch];
            ubyte* dstLine = &level.data[y * pitch];

            for( int x = 0; x + 3 < pitch; x += 4 ) {
              dstLine[x + 0] = srcLine[x + 2];
              dstLine[x + 1] = srcLine[x + 1];
              dstLine[x + 2] = srcLine[x + 0];
              dstLine[x + 3] = srcLine[x + 3];
            }
          }
        }
        break;
      }
    }

    if( levelDib != image->dib ) {
      FreeImage_Unload( levelDib );
    }

    width  = max( width / 2, 1 );
    height = max( height / 2, 1 );
  }
  while( genMipmaps && ( levels.last().width > 1 || levels.last().height > 1 ) );

  return true;
}
#endif

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

bool GLTexture::load( const File& file )
{
  destroy();
  create();

  if( textureId == 0 ) {
    return false;
  }

  return GL::textureDataFromFile( textureId, file ) != 0;
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
