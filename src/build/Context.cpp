/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file build/Context.cpp
 */

#include "stable.hpp"

#include "build/Context.hpp"

#include "client/MD2.hpp"
#include "client/MD3.hpp"

#include "client/SMMImago.hpp"
#include "client/SMMVehicleImago.hpp"
#include "client/ExplosionImago.hpp"
#include "client/MD2Imago.hpp"
#include "client/MD2WeaponImago.hpp"
#include "client/MD3Imago.hpp"

#include "client/BasicAudio.hpp"
#include "client/BotAudio.hpp"
#include "client/VehicleAudio.hpp"

#include "client/OpenGL.hpp"

#include <SDL/SDL_image.h>

namespace oz
{
namespace build
{

const int Context::DEFAULT_MAG_FILTER = GL_LINEAR;
const int Context::DEFAULT_MIN_FILTER = GL_LINEAR_MIPMAP_LINEAR;

bool Context::useS3TC = false;

uint Context::buildTexture( const void* data, int width, int height, uint format,
                            bool wrap, int magFilter, int minFilter )
{
  if( useS3TC && !( Math::isPow2( width ) && Math::isPow2( height ) ) ) {
    throw Exception( "Texture must be of dimensions 2^n x 2^m to use S3 texture compression." );
  }

  bool generateMipmaps = false;
  int internalFormat;

  switch( format ) {
    case GL_LUMINANCE: {
      internalFormat = useS3TC ? GL_COMPRESSED_RGB_S3TC_DXT1_EXT : GL_LUMINANCE;
      break;
    }
    case GL_RGB:
    case GL_BGR:
    case GL_COLOR_INDEX: {
      internalFormat = useS3TC ? GL_COMPRESSED_RGB_S3TC_DXT1_EXT : GL_RGB;
      break;
    }
    // GL_LUMINANCE_ALPHA is temporarily abused to represent indexed colours with transparency
    case GL_LUMINANCE_ALPHA: {
      format = GL_COLOR_INDEX;
      internalFormat = useS3TC ? GL_COMPRESSED_RGBA_S3TC_DXT1_EXT : GL_RGBA;
      break;
    }
    case GL_RGBA:
    case GL_BGRA: {
      internalFormat = useS3TC ? GL_COMPRESSED_RGBA_S3TC_DXT5_EXT : GL_RGBA;
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

  glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width, height, 0,
                format, GL_UNSIGNED_BYTE, data );

  if( generateMipmaps ) {
    glGenerateMipmap( GL_TEXTURE_2D );
  }

  glBindTexture( GL_TEXTURE_2D, 0 );

  if( glGetError() != GL_NO_ERROR || !glIsTexture( texId ) ) {
    throw Exception( "Texture building failed" );
  }

  return texId;
}

uint Context::createTexture( const void* data, int width, int height, uint format,
                             bool wrap, int magFilter, int minFilter )
{
  uint texId = buildTexture( data, width, height, format, wrap, magFilter, minFilter );

  if( texId == 0 ) {
    log.println( "Error while creating texture from buffer" );
    throw Exception( "Texture loading failed" );
  }
  return texId;
}

uint Context::loadRawTexture( const char* path, bool wrap, int magFilter, int minFilter )
{
  log.print( "Loading raw texture '%s' ...", path );

  const char* ext = String::findLast( path, '.' );
  if( ext == null || ( !String::equals( ext, ".png" ) && !String::equals( ext, ".jpg" ) &&
        !String::equals( ext, ".tga" ) ) )
  {
    throw Exception( "Invalid texture extension. Should be either '.png', '.jpg' or '.tga'" );
  }

  SDL_Surface* image = IMG_Load( path );
  if( image == null ) {
    throw Exception( "Texture loading failed" );
  }

  uint format;
  const char* sFormat;

  if( image->format->BitsPerPixel == 8 ) {
    if( image->format->palette == null ) {
      format = GL_LUMINANCE;
      sFormat = "greyscale";
    }
    else {
      SDL_Palette* palette = image->format->palette;

      if( palette->ncolors > 256 ) {
        throw Exception( "Palette for indexed images should not have more than 256 colours" );
      }

      float red[256];
      float green[256];
      float blue[256];
      float alpha[256];

      for( int i = 0; i < palette->ncolors; ++i ) {
        red[i]   = float( palette->colors[i].r ) / 256.0f;
        green[i] = float( palette->colors[i].g ) / 256.0f;
        blue[i]  = float( palette->colors[i].b ) / 256.0f;
        alpha[i] = i == int( image->format->colorkey ) ? 0.0f : 1.0f;
      }
      for( int i = palette->ncolors; i < 256; ++i ) {
        red[i]   = 1.0f;
        green[i] = 1.0f;
        blue[i]  = 1.0f;
        alpha[i] = 1.0f;
      }

      glPixelMapfv( GL_PIXEL_MAP_I_TO_R, 256, red );
      glPixelMapfv( GL_PIXEL_MAP_I_TO_G, 256, green );
      glPixelMapfv( GL_PIXEL_MAP_I_TO_B, 256, blue );
      glPixelMapfv( GL_PIXEL_MAP_I_TO_A, 256, alpha );

      if( image->flags & SDL_SRCCOLORKEY ) {
        // GL_LUMINANCE_ALPHA is temporarily abused to represent indexed colours with transparency
        format = GL_LUMINANCE_ALPHA;
        sFormat = "indexed(" + String( palette->ncolors ) + ", alpha)";
      }
      else {
        format = GL_COLOR_INDEX;
        sFormat = "indexed(" + String( palette->ncolors ) + ")";
      }
    }
  }
  else if( image->format->BitsPerPixel == 24 ) {
    if( String::equals( ext, ".tga" ) ) {
      format = GL_BGR;
      sFormat = "BGR";
    }
    else {
      format = GL_RGB;
      sFormat = "RGB";
    }
  }
  else if( image->format->BitsPerPixel == 32 ) {
    if( String::equals( ext, ".tga" ) ) {
      format = GL_BGRA;
      sFormat = "BGRA";
    }
    else {
      format = GL_RGBA;
      sFormat = "RBGA";
    }
  }
  else {
    throw Exception( "Wrong texture format. Should be indexed, greyscale, RGB/BGR or RGBA/BGRA." );
  }

  log.printEnd( " %s ... OK", sFormat );

  OZ_GL_CHECK_ERROR();

  int bytesPerPixel = image->format->BitsPerPixel / 8;
  char* bytes = reinterpret_cast<char*>( image->pixels );

  char* pos0 = &bytes[0];
  char* pos1 = &bytes[ ( image->h - 1 ) * image->w * bytesPerPixel ];

  for( int y = 0; y < image->h / 2; ++y ) {
    for( int x = 0; x < image->w; ++x ) {
      for( int i = 0; i < bytesPerPixel; ++i ) {
        swap( pos0[i], pos1[i] );
      }

      pos0 += bytesPerPixel;
      pos1 += bytesPerPixel;
    }

    pos1 -= 2 * image->w * bytesPerPixel;
  }

  uint texId = createTexture( bytes, image->w, image->h, format, wrap,
                              magFilter, minFilter );

  SDL_FreeSurface( image );

  if( texId == 0 || !glIsTexture( texId ) ) {
    throw Exception( "Texture loading failed" );
  }

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
#ifdef OZ_MINGW
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
