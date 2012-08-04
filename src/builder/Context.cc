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
 * @file builder/Context.cc
 */

#include "stable.hh"

#include "builder/Context.hh"

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
#ifdef OZ_NONFREE
# include <squish.h>
#endif

namespace oz
{
namespace builder
{

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

Context::Texture::Level::Level() :
  data( null )
{}

Context::Texture::Level::~Level()
{
  delete[] data;
}

Context::Texture::Level::Level( Level&& l ) :
  data( l.data ), width( l.width ), height( l.height ), format( l.format ), size( l.size )
{
  l.data = null;
}

Context::Texture::Level& Context::Texture::Level::operator = ( Level&& l )
{
  if( &l == this ) {
    return *this;
  }

  data   = l.data;
  width  = l.width;
  height = l.height;
  format = l.format;
  size   = l.size;

  l.data = null;

  return *this;
}

Context::Texture::Texture( Image* image, bool wrap_, int magFilter_, int minFilter_ )
{
  hard_assert( image->format == GL_LUMINANCE ||
               image->format == GL_RGB ||
               image->format == GL_RGBA );

  magFilter = magFilter_;
  minFilter = minFilter_;
  wrap      = wrap_ ? GL_REPEAT : GL_CLAMP_TO_EDGE;

  int  width      = image->width;
  int  height     = image->height;
  bool genMipmaps = minFilter == GL_NEAREST_MIPMAP_NEAREST ||
                    minFilter == GL_LINEAR_MIPMAP_NEAREST ||
                    minFilter == GL_NEAREST_MIPMAP_LINEAR ||
                    minFilter == GL_LINEAR_MIPMAP_LINEAR;

  if( genMipmaps && ( !Math::isPow2( width ) || !Math::isPow2( height ) ) ) {
    throw Exception( "Image has dimensions %dx%d but both dimensions must be powers of two to"
                     " generate mipmaps.", width, height );
  }

  do {
    levels.add();
    Level& level = levels.last();

    level.width  = width;
    level.height = height;

    FIBITMAP* levelDib = image->dib;
    if( levels.length() > 1 ) {
      levelDib = FreeImage_Rescale( image->dib, width, height, FILTER_CATMULLROM );
    }

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

          squish::CompressImage( data, width, height, level.data,
                                 squish::kDxt1 | squish::kColourIterativeClusterFit );
          delete[] data;
#endif
        }
        else {
          level.format = GL_LUMINANCE;
          level.size   = width * height;
          level.data   = new ubyte[level.size];

          for( int y = 0; y < height; ++y ) {
            memcpy( level.data + y*width, FreeImage_GetScanLine( levelDib, y ), size_t( width ) );
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

          squish::CompressImage( data, width, height, level.data,
                                 squish::kDxt1 | squish::kColourIterativeClusterFit );
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

          squish::CompressImage( data, width, height, level.data,
                                 squish::kDxt5 | squish::kColourIterativeClusterFit |
                                 squish::kWeightColourByAlpha );
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
}

bool Context::Texture::isEmpty() const
{
  return levels.isEmpty();
}

void Context::Texture::write( BufferStream* os )
{
  os->writeInt( wrap );
  os->writeInt( magFilter );
  os->writeInt( minFilter );

  for( int i = 0; i < levels.length(); ++i ) {
    const Level& level = levels[i];

    os->writeInt( level.width );
    os->writeInt( level.height );
    os->writeInt( level.format );
    os->writeInt( level.size );

    os->writeChars( reinterpret_cast<char*>( level.data ), level.size );
  }

  os->writeInt( 0 );
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
  bool isOpaque     = FreeImage_GetTransparentIndex( dib ) < 0;

  if( forceFormat == 0 ) {
    if( isPalettised ) {
      forceFormat = isOpaque ? GL_RGB : GL_RGBA;
    }
    else {
      forceFormat = bpp == 8 ? GL_LUMINANCE : bpp == 24 ? GL_RGB : GL_RGBA;
    }
  }

  switch( forceFormat ) {
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
    case GL_RGB: {
      bpp    = 24;
      format = GL_RGB;

      FIBITMAP* newImage = FreeImage_ConvertTo24Bits( dib );
      if( newImage == null ) {
        throw Exception( "Conversion from palettised to RGB image failed" );
      }

      FreeImage_Unload( dib );
      dib = newImage;

      break;
    }
    case GL_RGBA: {
      bpp    = 32;
      format = GL_RGBA;

      FIBITMAP* newDIB = FreeImage_ConvertTo32Bits( dib );
      if( newDIB == null ) {
        throw Exception( "Conversion from palettised to RGBA image failed" );
      }

      FreeImage_Unload( dib );
      dib = newDIB;

      break;
    }
    default: {
      hard_assert( false );
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

Context::Texture Context::loadTexture( const char* path, bool wrap, int magFilter, int minFilter )
{
  Image   image = loadImage( path );
  Texture texture( &image, wrap, magFilter, minFilter );
  FreeImage_Unload( image.dib );

  return texture;
}

void Context::loadTextures( Texture* diffuseTex, Texture* masksTex, Texture* normalsTex,
                            const char* basePath_, bool wrap, int magFilter, int minFilter )
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
    if( !specular.stat() ) {
      specular.setPath( specularBasePath + IMAGE_EXTENSIONS[i] );
    }
    if( !specular1.stat() ) {
      specular1.setPath( specular1BasePath + IMAGE_EXTENSIONS[i] );
    }
    if( !emission.stat() ) {
      emission.setPath( emissionBasePath + IMAGE_EXTENSIONS[i] );
    }
    if( !normals.stat() ) {
      normals.setPath( normalsBasePath + IMAGE_EXTENSIONS[i] );
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
    image = loadImage( diffuse.path() );
  }
  else if( diffuse1.stat() ) {
    image = loadImage( diffuse1.path() );
  }
  else {
    throw Exception( "Missing texture '%s' (.png, .jpeg, .jpg and .tga checked)", basePath.cstr() );
  }

  *diffuseTex = Texture( &image, wrap, magFilter, minFilter );
  FreeImage_Unload( image.dib );

  image.dib         = null;
  specImage.dib     = null;
  emissionImage.dib = null;

  if( masks.stat() ) {
    image = loadImage( masks.path(), GL_RGB );
  }
  else if( specular.stat() ) {
    specImage = loadImage( specular.path(), GL_RGB );

    if( emission.stat() ) {
      emissionImage = loadImage( emission.path(), GL_LUMINANCE );
    }
  }
  else if( specular1.stat() ) {
    specImage = loadImage( specular1.path(), GL_RGB );

    if( emission.stat() ) {
      emissionImage = loadImage( emission.path(), GL_LUMINANCE );
    }
  }

  if( image.dib != null ) {
    *masksTex = Texture( &image, wrap, magFilter, minFilter );
    FreeImage_Unload( image.dib );
  }
  else if( specImage.dib != null ) {
    if( emissionImage.dib != null ) {
      if( specImage.width != emissionImage.width || specImage.height != emissionImage.height ) {
        throw Exception( "Specular and emission texture masks must have the same size." );
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

    *masksTex = Texture( &specImage, wrap, magFilter, minFilter );

    FreeImage_Unload( specImage.dib );
    if( emissionImage.dib != null ) {
      FreeImage_Unload( emissionImage.dib );
    }
  }

  if( bumpmap ) {
    image.dib = null;

    if( normals.stat() ) {
      image = loadImage( normals.path() );
    }
    else if( normals1.stat() ) {
      image = loadImage( normals1.path(), GL_RGB );
    }
    else if( normals2.stat() ) {
      image = loadImage( normals2.path(), GL_RGB );
    }
    else if( normals3.stat() ) {
      image = loadImage( normals3.path(), GL_RGB );
    }

    if( image.dib != null ) {
      *normalsTex = Texture( &image, wrap, magFilter, minFilter );
      FreeImage_Unload( image.dib );
    }
  }
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

Context context;

}
}
