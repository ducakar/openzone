/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2011 Davorin Učakar
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

#include "client/OpenGL.hh"

#include <SDL/SDL_image.h>
#include <IL/il.h>

namespace oz
{
namespace build
{

const int Context::DEFAULT_MAG_FILTER = GL_LINEAR;
const int Context::DEFAULT_MIN_FILTER = GL_LINEAR_MIPMAP_LINEAR;

bool Context::useS3TC = false;

uint Context::buildTexture( const void* data, int width, int height, int format,
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
    case GL_RGB: {
      internalFormat = useS3TC ? GL_COMPRESSED_RGB_S3TC_DXT1_EXT : GL_RGB;
      break;
    }
    case GL_RGBA: {
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

  glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width, height, 0, uint( format ), GL_UNSIGNED_BYTE,
                data );

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

uint Context::createTexture( const void* data, int width, int height, int format,
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
  log.print( "Loading texture '%s' ...", path );

  uint image = ilGenImage();
  ilBindImage( image );

  if( !ilLoadImage( path ) ) {
    throw Exception( "Texture '%s' loading failed", path );
  }

  log.printRaw( " converting ..." );

  ubyte* data   = null;
  int    width  = ilGetInteger( IL_IMAGE_WIDTH );
  int    height = ilGetInteger( IL_IMAGE_HEIGHT );
  int    format = ilGetInteger( IL_IMAGE_FORMAT );

  switch( format ) {
    case IL_LUMINANCE: {
      format = GL_LUMINANCE;
      data   = new ubyte[width * height];

      ilCopyPixels( 0, 0, 0, uint( width ), uint( height ), 1,
                    IL_LUMINANCE, IL_UNSIGNED_BYTE, data );

      ubyte* top    = data;
      ubyte* bottom = data + ( height - 1 ) * width;

      for( int y = 0; y < height / 2; ++y ) {
        for( int x = 0; x < width; ++x ) {
          swap( top[0], bottom[0] );

          top    += 1;
          bottom += 1;
        }

        bottom -= 2 * width;
      }
      break;
    }
    case IL_RGB:
    case IL_BGR:
    case IL_COLOUR_INDEX: {
      format = GL_RGB;
      data   = new ubyte[width * height * 3];

      ilCopyPixels( 0, 0, 0, uint( width ), uint( height ), 1, IL_RGB, IL_UNSIGNED_BYTE, data );

      ubyte* top    = data;
      ubyte* bottom = data + ( height - 1 ) * width * 3;

      for( int y = 0; y < height / 2; ++y ) {
        for( int x = 0; x < width; ++x ) {
          swap( top[0], bottom[0] );
          swap( top[1], bottom[1] );
          swap( top[2], bottom[2] );

          top    += 3;
          bottom += 3;
        }

        bottom -= 2 * width * 3;
      }
      break;
    }
    case IL_RGBA:
    case IL_BGRA: {
      format = GL_RGBA;
      data   = new ubyte[width * height * 4];

      ilCopyPixels( 0, 0, 0, uint( width ), uint( height ), 1, IL_RGBA, IL_UNSIGNED_BYTE, data );

      ubyte* top    = data;
      ubyte* bottom = data + ( height - 1 ) * width * 4;

      for( int y = 0; y < height / 2; ++y ) {
        for( int x = 0; x < width; ++x ) {
          swap( top[0], bottom[0] );
          swap( top[1], bottom[1] );
          swap( top[2], bottom[2] );
          swap( top[3], bottom[3] );

          top    += 4;
          bottom += 4;
        }

        bottom -= 2 * width * 4;
      }
      break;
    }
  }

  ilDeleteImage( image );

  log.printEnd( " OK" );

  uint texId = createTexture( data, width, height, format, wrap, magFilter, minFilter );

  delete[] data;

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
