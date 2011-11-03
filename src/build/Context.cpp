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
 * Davorin Učakar <davorin.ucakar@gmail.com>
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
  OZ_GL_CHECK_ERROR();

  if( useS3TC && !( Math::isPow2( width ) && Math::isPow2( height ) ) ) {
    throw Exception( "Texture must be of dimensions 2^n x 2^m to use S3 texture compression." );
  }

  bool generateMipmaps = false;
  int  internalFormat = format == GL_RGBA || format == GL_BGRA ?
       ( useS3TC ? GL_COMPRESSED_RGBA_S3TC_DXT5_EXT : GL_RGBA ) :
       ( useS3TC ? GL_COMPRESSED_RGB_S3TC_DXT1_EXT : GL_RGB );

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

  uint        format;
  const char* sFormat;

  if( image->format->BitsPerPixel == 24 ) {
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
    throw Exception( "Wrong texture format. Should be 24 bpp RGB/BGR or 32 bpp RGBA/BGRA" );
  }

  log.printEnd( " %s ... OK", sFormat );

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
    glDeleteTextures( 1, &texId );

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
      glGetTexImage( GL_TEXTURE_2D, i, GL_RGBA, GL_UNSIGNED_BYTE, stream->forward( size ) );
    }
  }

  OZ_GL_CHECK_ERROR();
}

}
}
