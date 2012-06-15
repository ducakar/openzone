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

#include "client/SMMImago.hh"
#include "client/SMMVehicleImago.hh"
#include "client/ExplosionImago.hh"
#include "client/MD2Imago.hh"
#include "client/MD2WeaponImago.hh"
#include "client/MD3Imago.hh"

#include "client/BasicAudio.hh"
#include "client/BotAudio.hh"
#include "client/VehicleAudio.hh"

#include <GL/gl.h>
#include <FreeImage.h>

namespace oz
{
namespace build
{

Context context;

const char* const Context::IMAGE_EXTENSIONS[] = {
  ".png",
  ".jpeg",
  ".jpg",
  ".tga"
};

struct Context::Image
{
  FIBITMAP* dib;
  ubyte*    pixels;
  int       width;
  int       height;
  int       bpp;
  int       format;
};

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
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
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

Context::Image Context::loadImage( const char* path, int forceFormat )
{
  Log::print( "Loading image '%s' ...", path );

  PFile file( path );
  String realPath = file.realDir() + "/" + file.path();

  if( !file.stat() ) {
    throw Exception( "File '%s' does not exits", realPath.cstr() );
  }

  FREE_IMAGE_FORMAT type = FreeImage_GetFileType( realPath );
  if( type == FIF_UNKNOWN ) {
    throw Exception( "Invalid image file type '%s'", realPath.cstr() );
  }

  FIBITMAP* dib = FreeImage_Load( type, realPath );
  if( dib == null ) {
    throw Exception( "Texture '%s' loading failed", realPath.cstr() );
  }

  int width  = int( FreeImage_GetWidth( dib ) );
  int height = int( FreeImage_GetHeight( dib ) );
  int bpp    = int( FreeImage_GetBPP( dib ) );

  Log::printRaw( " %d x %d %d BPP ...", width, height, bpp );

  if( height != 1 && ( width * bpp ) % 32 != 0 ) {
    throw Exception( "Image scan line (width * bytesPerPixel) should be a multiple of 4." );
  }

  int format = int( FreeImage_GetImageType( dib ) );

  if( format != FIT_BITMAP ) {
    throw Exception( "Invalid image colour format" );
  }

  bool isPalettised = FreeImage_GetColorsUsed( dib ) != 0;
  bool isOpaque     = FreeImage_GetTransparentIndex( dib ) == -1;

  if( forceFormat == 0 ) {
    if( isPalettised ) {
      forceFormat = isOpaque ? GL_BGR : GL_BGRA;
    }
    else {
      forceFormat = bpp == 8 ? GL_LUMINANCE : bpp == 24 ? GL_BGR : GL_BGRA;
    }
  }

  switch( forceFormat ) {
    default: {
      hard_assert( false );
    }
    case GL_LUMINANCE: {
      bpp    = 8;
      format = GL_LUMINANCE;

      FIBITMAP* newImage = FreeImage_ConvertToGreyscale( dib );
      if( newImage == null ) {
        throw Exception( "Conversion from palettised to grayscale image failed" );
      }

      FreeImage_Unload( dib );
      dib = newImage;

      break;
    }
    case GL_BGR: {
      bpp    = 24;
      format = GL_BGR;

      FIBITMAP* newImage = FreeImage_ConvertTo24Bits( dib );
      if( newImage == null ) {
        throw Exception( "Conversion from palettised to RGB image failed" );
      }

      FreeImage_Unload( dib );
      dib = newImage;

      break;
    }
    case GL_BGRA: {
      bpp    = 32;
      format = GL_BGRA;

      FIBITMAP* newDIB = FreeImage_ConvertTo32Bits( dib );
      if( newDIB == null ) {
        throw Exception( "Conversion from palettised to RGBA image failed" );
      }

      FreeImage_Unload( dib );
      dib = newDIB;

      break;
    }
  }

  ubyte* pixels = FreeImage_GetBits( dib );
  if( pixels == null ) {
    throw Exception( "Failed to access image data" );
  }

  Log::printEnd( " OK" );

  return { dib, pixels, width, height, bpp, format };
}

uint Context::loadLayer( const char* path, bool wrap, int magFilter, int minFilter )
{
  Image image = loadImage( path );
  uint  id    = buildTexture( image.pixels, image.width, image.height, image.format,
                              wrap, magFilter, minFilter );
  FreeImage_Unload( image.dib );

  return id;
}

void Context::loadTexture( uint* diffuseId, uint* masksId, uint* normalsId, const char* basePath_,
                           bool wrap, int magFilter, int minFilter )
{
  String basePath          = basePath_;
  String diffuseBasePath   = basePath;
  String diffuse1BasePath  = basePath + "_d";
  String masksBasePath     = basePath + "_m";
  String specularBasePath  = basePath + "_s";
  String specular1BasePath = basePath + "_spec";
  String emissionBasePath  = basePath + ".blend";
  String normalsBasePath   = basePath + "_n";
  String normals1BasePath  = basePath + "_nm";
  String normals2BasePath  = basePath + "_normal";
  String normals3BasePath  = basePath + "_local";

  PFile diffuse( diffuseBasePath + IMAGE_EXTENSIONS[0] );
  PFile diffuse1( diffuseBasePath + IMAGE_EXTENSIONS[0] );
  PFile masks( masksBasePath + IMAGE_EXTENSIONS[0] );
  PFile specular( specularBasePath + IMAGE_EXTENSIONS[0] );
  PFile specular1( specularBasePath + IMAGE_EXTENSIONS[0] );
  PFile emission( emissionBasePath + IMAGE_EXTENSIONS[0] );
  PFile normals( normalsBasePath + IMAGE_EXTENSIONS[0] );
  PFile normals1( normals1BasePath + IMAGE_EXTENSIONS[0] );
  PFile normals2( normals2BasePath + IMAGE_EXTENSIONS[0] );
  PFile normals3( normals3BasePath + IMAGE_EXTENSIONS[0] );

  for( int i = 1; i < aLength( IMAGE_EXTENSIONS ); ++i ) {
    if( !diffuse.stat() ) {
      diffuse.setPath( diffuseBasePath + IMAGE_EXTENSIONS[i] );
    }
    if( !diffuse1.stat() ) {
      diffuse1.setPath( diffuse1BasePath + IMAGE_EXTENSIONS[i] );
    }
    if( !masks.stat() ) {
      masks.setPath( masksBasePath + IMAGE_EXTENSIONS[i] );
    }
    if( !normals.stat() ) {
      normals.setPath( normalsBasePath + IMAGE_EXTENSIONS[i] );
    }
    if( !specular.stat() ) {
      specular.setPath( specularBasePath + IMAGE_EXTENSIONS[i] );
    }
    if( !specular1.stat() ) {
      specular1.setPath( specular1BasePath + IMAGE_EXTENSIONS[i] );
    }
    if( !emission.stat() ) {
      emission.setPath( emissionBasePath + IMAGE_EXTENSIONS[i] );
    }
    if( !normals1.stat() ) {
      normals1.setPath( normals1BasePath + IMAGE_EXTENSIONS[i] );
    }
    if( !normals2.stat() ) {
      normals2.setPath( normals2BasePath + IMAGE_EXTENSIONS[i] );
    }
    if( !normals3.stat() ) {
      normals3.setPath( normals3BasePath + IMAGE_EXTENSIONS[i] );
    }
  }

  Image image, specImage, emissionImage;
  image.dib = null;

  if( diffuse.stat() ) {
    image = loadImage( diffuse.path(), 0 );
  }
  else if( diffuse1.stat() ) {
    image = loadImage( diffuse1.path(), 0 );
  }
  else {
    throw Exception( "Missing texture '%s' (.png, .jpeg, .jpg and .tga checked)", basePath.cstr() );
  }

  *diffuseId = buildTexture( image.pixels, image.width, image.height, image.format,
                             wrap, magFilter, minFilter );
  FreeImage_Unload( image.dib );

  image.dib         = null;
  specImage.dib     = null;
  emissionImage.dib = null;

  if( masks.stat() ) {
    image = loadImage( masks.path(), GL_BGR );
  }
  else if( specular.stat() ) {
    specImage = loadImage( specular.path(), GL_BGR );

    if( emission.stat() ) {
      emissionImage = loadImage( emission.path(), GL_LUMINANCE );
    }
  }
  else if( specular1.stat() ) {
    specImage = loadImage( specular1.path(), GL_BGR );

    if( emission.stat() ) {
      emissionImage = loadImage( emission.path(), GL_LUMINANCE );
    }
  }

  if( image.dib != null ) {
    *masksId = buildTexture( image.pixels, image.width, image.height, image.format,
                             wrap, magFilter, minFilter );
    FreeImage_Unload( image.dib );
  }
  else if( specImage.dib != null ) {
    if( emissionImage.dib != null ) {
      if( specImage.width != emissionImage.width || specImage.height != emissionImage.height ) {
        throw Exception( "Specular and emission texture masks must have same size." );
      }
    }

    for( int i = 0; i < specImage.width * specImage.height; ++i ) {
      ubyte& b = specImage.pixels[i*3 + 0];
      ubyte& g = specImage.pixels[i*3 + 1];
      ubyte& r = specImage.pixels[i*3 + 2];

      r = ubyte( ( b + g + r ) / 3 );
      g = ubyte( emissionImage.dib == null ? 0 : emissionImage.pixels[i] );
      b = 0;
    }

    *masksId = buildTexture( specImage.pixels, specImage.width, specImage.height, specImage.format,
                             wrap, magFilter, minFilter );

    FreeImage_Unload( specImage.dib );
    if( emissionImage.dib != null ) {
      FreeImage_Unload( emissionImage.dib );
    }
  }
  else {
    *masksId = 0;
  }

  if( !bumpmap ) {
    *normalsId = 0;
  }
  else {
    image.dib = null;

    if( normals.stat() ) {
      image = loadImage( normals.path() );
    }
    else if( normals1.stat() ) {
      image = loadImage( normals1.path(), GL_BGR );
    }
    else if( normals2.stat() ) {
      image = loadImage( normals2.path(), GL_BGR );
    }
    else if( normals3.stat() ) {
      image = loadImage( normals3.path(), GL_BGR );
    }

    if( image.dib != null ) {
      *normalsId = buildTexture( image.pixels, image.width, image.height, GL_BGR,
                                 wrap, magFilter, minFilter );
      FreeImage_Unload( image.dib );
    }
    else {
      *normalsId = 0;
    }
  }
}

void Context::writeLayer( uint id, BufferStream* stream )
{
  glBindTexture( GL_TEXTURE_2D, id );

  int wrap, magFilter, minFilter;
  bool isS3TC = false;

  glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrap );
  glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter );
  glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter );

  stream->writeInt( wrap );
  stream->writeInt( magFilter );
  stream->writeInt( minFilter );

  for( int level = 0; ; ++level ) {
    int width, height, format, size;

    glGetTexLevelParameteriv( GL_TEXTURE_2D, level, GL_TEXTURE_WIDTH, &width );
    glGetTexLevelParameteriv( GL_TEXTURE_2D, level, GL_TEXTURE_HEIGHT, &height );
    glGetTexLevelParameteriv( GL_TEXTURE_2D, level, GL_TEXTURE_INTERNAL_FORMAT, &format );

    if( width == 0 ) {
      break;
    }

    stream->writeInt( width );
    stream->writeInt( height );
    stream->writeInt( format );

    if( format == GL_COMPRESSED_RGB_S3TC_DXT1_EXT || format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT ) {
      glGetTexLevelParameteriv( GL_TEXTURE_2D, level, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &size );

      stream->writeInt( size );
#ifdef _WIN32
      client::glGetCompressedTexImage( GL_TEXTURE_2D, level, stream->forward( size ) );
#else
      glGetCompressedTexImage( GL_TEXTURE_2D, level, stream->forward( size ) );
#endif

      isS3TC = true;
      break;
    }
    else {
      hard_assert( format == GL_RGB || format == GL_RGBA || format == GL_LUMINANCE );

      int sampleSize = format == GL_RGB ? 3 : format == GL_RGBA ? 4 : 1;
      int lineSize = ( ( width * sampleSize - 1 ) / 4 + 1 ) * 4;

      size = height * lineSize;

      stream->writeInt( size );
      glGetTexImage( GL_TEXTURE_2D, level, uint( format ),
                     GL_UNSIGNED_BYTE, stream->forward( size ) );
    }
  }

  if( !isS3TC ) {
    stream->writeInt( 0 );
  }

  OZ_GL_CHECK_ERROR();
}

void Context::init()
{}

void Context::free()
{
  usedTextures.clear();
  usedTextures.dealloc();
  usedSounds.clear();
  usedSounds.dealloc();
  usedModels.clear();
  usedModels.dealloc();
}

}
}
