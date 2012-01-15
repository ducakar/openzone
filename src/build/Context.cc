/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file build/Context.cc
 */

#include "stable.hh"

#include "build/Context.hh"

#include "client/MD2.hh"
#include "client/MD3.hh"

#include "client/SMMImago.hh"
#include "client/SMMVehicleImago.hh"
#include "client/ExplosionImago.hh"
#include "client/MD2Imago.hh"
#include "client/MD2WeaponImago.hh"
#include "client/MD3Imago.hh"

#include "client/BasicAudio.hh"
#include "client/BotAudio.hh"
#include "client/VehicleAudio.hh"

#include <FreeImage.h>

namespace oz
{
namespace build
{

bool Context::useS3TC = false;

uint Context::buildTexture( const void* data, int width, int height, int format, bool wrap,
                            int magFilter, int minFilter )
{
  if( useS3TC && !( Math::isPow2( width ) && Math::isPow2( height ) ) ) {
    throw Exception( "Texture must be of dimensions 2^n x 2^m to use S3 texture compression." );
  }

  bool generateMipmaps = false;
  int internalFormat;

  switch( format ) {
    case GL_BGR:
    case GL_RGB: {
      internalFormat = useS3TC ? GL_COMPRESSED_RGB_S3TC_DXT1_EXT : GL_RGB;
      break;
    }
    case GL_BGRA:
    case GL_RGBA: {
      internalFormat = useS3TC ? GL_COMPRESSED_RGBA_S3TC_DXT5_EXT : GL_RGBA;
      break;
    }
    case GL_LUMINANCE: {
      internalFormat = useS3TC ? GL_COMPRESSED_RGB_S3TC_DXT1_EXT : GL_LUMINANCE;
      break;
    }
    default: {
      throw Exception( "Internal format resolution fall-through" );
    }
  }

  uint texId;
  glGenTextures( 1, &texId );
  glBindTexture( GL_TEXTURE_2D, texId );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );

  switch( magFilter ) {
    case GL_NEAREST:
    case GL_LINEAR: {
      break;
    }
    default: {
      throw Exception( "Invalid texture magnification filter" );
    }
  }

  switch( minFilter ) {
    case GL_NEAREST:
    case GL_LINEAR: {
      break;
    }
    case GL_NEAREST_MIPMAP_NEAREST:
    case GL_NEAREST_MIPMAP_LINEAR:
    case GL_LINEAR_MIPMAP_NEAREST:
    case GL_LINEAR_MIPMAP_LINEAR: {
      generateMipmaps = true;
      break;
    }
    default: {
      throw Exception( "Invalid texture minification filter" );
    }
  }

  if( !wrap ) {
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
  }

  glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width, height, 0, uint( format ),
                GL_UNSIGNED_BYTE, data );

  if( generateMipmaps ) {
#ifdef _WIN32
    client::glGenerateMipmap( GL_TEXTURE_2D );
#else
    glGenerateMipmap( GL_TEXTURE_2D );
#endif
  }

  glBindTexture( GL_TEXTURE_2D, 0 );

  if( glGetError() != GL_NO_ERROR || !glIsTexture( texId ) ) {
    throw Exception( "Texture building failed" );
  }

  return texId;
}

uint Context::loadRawTexture( const char* path, bool wrap, int magFilter, int minFilter )
{
  log.print( "Loading texture '%s' ...", path );

  PhysFile file( path );
  String realPath = file.realPath();

  FREE_IMAGE_FORMAT type = FreeImage_GetFileType( realPath );
  if( type == FIF_UNKNOWN ) {
    throw Exception( "Invalid image file type '%s'", realPath.cstr() );
  }

  FIBITMAP* image = FreeImage_Load( type, realPath );
  if( image == null ) {
    throw Exception( "Texture '%s' loading failed", realPath.cstr() );
  }

  int width  = int( FreeImage_GetWidth( image ) );
  int height = int( FreeImage_GetHeight( image ) );
  int bpp    = int( FreeImage_GetBPP( image ) );

  log.printRaw( " %d x %d %d BPP ...", width, height, bpp );

  if( width % 4 != 0 || height % 4 != 0 ) {
    throw Exception( "Image dimensions must be multiples of 4 to avoid padding between lines." );
  }

  int format = int( FreeImage_GetImageType( image ) );
  if( format != FIT_BITMAP ) {
    throw Exception( "Invalid image colour format" );
  }

  bool isPalettised = FreeImage_GetColorsUsed( image ) != 0;

  if( isPalettised ) {
    bool isOpaque = FreeImage_GetTransparentIndex( image ) == -1;

    if( isOpaque ) {
      format = GL_BGR;

      FIBITMAP* newImage = FreeImage_ConvertTo24Bits( image );
      if( newImage == null ) {
        throw Exception( "Conversion from palettised to RGB image failed" );
      }

      FreeImage_Unload( image );
      image = newImage;
    }
    else {
      format = GL_BGRA;

      FIBITMAP* newImage = FreeImage_ConvertTo32Bits( image );
      if( newImage == null ) {
        throw Exception( "Conversion from palettised to RGBA image failed" );
      }

      FreeImage_Unload( image );
      image = newImage;
    }
  }
  else {
    switch( bpp ) {
      case 8: {
        format = GL_LUMINANCE;
        break;
      }
      case 24: {
        format = GL_BGR;
        break;
      }
      case 32: {
        format = GL_BGRA;
        break;
      }
      default: {
        throw Exception( "Invalid image BPP %d, should be a power of two <= 32", bpp );
      }
    }
  }

  const void* data = FreeImage_GetBits( image );
  if( data == null ) {
    throw Exception( "Failed to access image data" );
  }

  uint texId = buildTexture( data, width, height, format, wrap, magFilter, minFilter );
  FreeImage_Unload( image );

  log.printEnd( " OK" );

  return texId;
}

void Context::writeTexture( uint id, BufferStream* stream )
{
  glBindTexture( GL_TEXTURE_2D, id );

  int wrap, magFilter, minFilter, nMipmaps, internalFormat;

  glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrap );
  glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter );
  glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter );
  glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, &nMipmaps );

  int width;
  for( nMipmaps = 0; nMipmaps < 1000; ++nMipmaps ) {
    glGetTexLevelParameteriv( GL_TEXTURE_2D, nMipmaps, GL_TEXTURE_WIDTH, &width );

    if( width == 0 ) {
      break;
    }
  }

  glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat );
  OZ_GL_CHECK_ERROR();

  stream->writeInt( wrap );
  stream->writeInt( magFilter );
  stream->writeInt( minFilter );
  stream->writeInt( nMipmaps );
  stream->writeInt( internalFormat );

  for( int i = 0; i < nMipmaps; ++i ) {
    int width, height, size;

    glGetTexLevelParameteriv( GL_TEXTURE_2D, i, GL_TEXTURE_WIDTH, &width );
    glGetTexLevelParameteriv( GL_TEXTURE_2D, i, GL_TEXTURE_HEIGHT, &height );

    if( useS3TC ) {
      glGetTexLevelParameteriv( GL_TEXTURE_2D, i, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &size );
    }
    else {
      size = width * height * 4;
    }

    stream->writeInt( width );
    stream->writeInt( height );
    stream->writeInt( size );

    if( useS3TC ) {
#ifdef _WIN32
      client::glGetCompressedTexImage( GL_TEXTURE_2D, i, stream->forward( size ) );
#else
      glGetCompressedTexImage( GL_TEXTURE_2D, i, stream->forward( size ) );
#endif
    }
    else {
      glGetTexImage( GL_TEXTURE_2D, i, uint( internalFormat ),
                     GL_UNSIGNED_BYTE, stream->forward( size ) );
    }
  }

  glDeleteTextures( 1, &id );

  OZ_GL_CHECK_ERROR();
}

}
}
