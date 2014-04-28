/*
 * ozFactory - OpenZone Assets Builder Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file ozFactory/ImageBuilder.cc
 */

#include "ImageBuilder.hh"

#include <cstdio>
#include <FreeImage.h>
#ifdef OZ_NONFREE
# include <squish.h>
#endif

namespace oz
{

static const int DDSD_CAPS                          = 0x00000001;
static const int DDSD_HEIGHT                        = 0x00000002;
static const int DDSD_WIDTH                         = 0x00000004;
static const int DDSD_PITCH                         = 0x00000008;
static const int DDSD_PIXELFORMAT                   = 0x00001000;
static const int DDSD_MIPMAPCOUNT                   = 0x00020000;
static const int DDSD_LINEARSIZE                    = 0x00080000;

static const int DDSCAPS_COMPLEX                    = 0x00000008;
static const int DDSCAPS_MIPMAP                     = 0x00400000;
static const int DDSCAPS_TEXTURE                    = 0x00001000;

static const int DDSCAPS2_CUBEMAP                   = 0x00000200;
static const int DDSCAPS2_CUBEMAP_POSITIVEX         = 0x00000400;
static const int DDSCAPS2_CUBEMAP_NEGITIVEX         = 0x00000800;
static const int DDSCAPS2_CUBEMAP_POSITIVEY         = 0x00001000;
static const int DDSCAPS2_CUBEMAP_NEGITIVEY         = 0x00002000;
static const int DDSCAPS2_CUBEMAP_POSITIVEZ         = 0x00004000;
static const int DDSCAPS2_CUBEMAP_NEGITIVEZ         = 0x00008000;

static const int DDPF_ALPHAPIXELS                   = 0x00000001;
static const int DDPF_FOURCC                        = 0x00000004;
static const int DDPF_RGB                           = 0x00000040;

static const int DXGI_FORMAT_R8G8B8A8_TYPELESS      = 27;
#ifdef OZ_NONFREE
static const int DXGI_FORMAT_BC1_TYPELESS           = 70;
static const int DXGI_FORMAT_BC3_TYPELESS           = 76;
#endif

static const int D3D10_RESOURCE_DIMENSION_TEXTURE2D = 3;

static const int ERROR_LENGTH                       = 1024;

static char errorBuffer[ERROR_LENGTH]               = {};

static FIBITMAP* createBitmap( const ImageData& image )
{
  FIBITMAP* dib = FreeImage_ConvertFromRawBits( reinterpret_cast<ubyte*>( image.pixels ),
                                                image.width, image.height, image.width * 4, 32,
                                                FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK,
                                                FI_RGBA_BLUE_MASK );

  // Convert RGBA -> BGRA.
  int    size   = image.width * image.height * 4;
  ubyte* pixels = FreeImage_GetBits( dib );

  for( int i = 0; i < size; i += 4 ) {
    swap( pixels[i + 0], pixels[i + 2] );
  }

  if( dib == nullptr ) {
    snprintf( errorBuffer, ERROR_LENGTH, "FreeImage_ConvertFromRawBits failed to build image." );
  }

  FreeImage_SetTransparent( dib, image.flags & ImageData::ALPHA_BIT );
  return dib;
}

static FIBITMAP* loadBitmap( const File& file )
{
  InputStream       is        = file.inputStream();
  ubyte*            dataBegin = reinterpret_cast<ubyte*>( const_cast<char*>( is.begin() ) );
  FIMEMORY*         memoryIO  = FreeImage_OpenMemory( dataBegin, uint( is.capacity() ) );
  FREE_IMAGE_FORMAT format    = FreeImage_GetFileTypeFromMemory( memoryIO, is.capacity() );
  FIBITMAP*         dib       = FreeImage_LoadFromMemory( format, memoryIO );

  FreeImage_CloseMemory( memoryIO );

  if( dib == nullptr ) {
    snprintf( errorBuffer, ERROR_LENGTH, "Failed to read '%s'.", file.path().cstr() );
    return nullptr;
  }

  FIBITMAP* oldDib = dib;
  dib = FreeImage_ConvertTo32Bits( dib );
  FreeImage_Unload( oldDib );

  // Remove alpha if unused.
  int    width    = int( FreeImage_GetWidth( dib ) );
  int    height   = int( FreeImage_GetHeight( dib ) );
  int    size     = width * height * 4;
  bool   hasAlpha = false;
  ubyte* pixels   = FreeImage_GetBits( dib );

  for( int i = 0; i < size; i += 4 ) {
    if( pixels[i + 3] != 255 ) {
      hasAlpha = true;
      break;
    }
  }
  if( !hasAlpha ) {
    FreeImage_SetTransparent( dib, false );
  }

  FreeImage_FlipVertical( dib );
  return dib;
}

static bool buildDDS( FIBITMAP** faces, int nFaces, int options, const File& destFile )
{
  hard_assert( nFaces > 0 );

  int width      = int( FreeImage_GetWidth( faces[0] ) );
  int height     = int( FreeImage_GetHeight( faces[0] ) );

  bool hasAlpha  = FreeImage_IsTransparent( faces[0] );
  bool doMipmaps = options & ImageBuilder::MIPMAPS_BIT;
  bool compress  = options & ImageBuilder::COMPRESSION_BIT;
  bool isCubeMap = options & ImageBuilder::CUBE_MAP_BIT;
  bool isArray   = !isCubeMap && nFaces > 1;

  for( int i = 1; i < nFaces; ++i ) {
    if( int( FreeImage_GetWidth( faces[i] ) ) != width ||
        int( FreeImage_GetHeight( faces[i] ) ) != height )
    {
      snprintf( errorBuffer, ERROR_LENGTH, "All faces must have same dimensions." );
      return false;
    }
  }

  if( compress ) {
#ifndef OZ_NONFREE
    snprintf( errorBuffer, ERROR_LENGTH, "Texture compression requested but compiled without"
              " libsquish (OZ_NONFREE is disabled)." );
    return false;
#else
    if( !Math::isPow2( width ) || !Math::isPow2( height ) ) {
      snprintf( errorBuffer, ERROR_LENGTH, "Compressed texture dimensions must be powers of 2." );
      return false;
    }
#endif
  }

  if( isCubeMap && nFaces != 6 ) {
    snprintf( errorBuffer, ERROR_LENGTH, "Cube map requires exactly 6 faces." );
    return false;
  }

  int targetBPP      = hasAlpha || compress || isArray ? 32 : 24;
  int pitchOrLinSize = ( ( width * targetBPP / 8 + 3 ) / 4 ) * 4;
  int nMipmaps       = doMipmaps ? Math::index1( max( width, height ) ) + 1 : 1;

  int flags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
  flags |= doMipmaps ? DDSD_MIPMAPCOUNT : 0;
  flags |= compress  ? DDSD_LINEARSIZE  : DDSD_PITCH;

  int caps = DDSCAPS_TEXTURE;
  caps |= doMipmaps ? DDSCAPS_COMPLEX | DDSCAPS_MIPMAP : 0;
  caps |= isCubeMap ? DDSCAPS_COMPLEX : 0;

  int caps2 = isCubeMap ? DDSCAPS2_CUBEMAP : 0;
  caps2 |= isCubeMap ? DDSCAPS2_CUBEMAP_POSITIVEX | DDSCAPS2_CUBEMAP_NEGITIVEX : 0;
  caps2 |= isCubeMap ? DDSCAPS2_CUBEMAP_POSITIVEY | DDSCAPS2_CUBEMAP_NEGITIVEY : 0;
  caps2 |= isCubeMap ? DDSCAPS2_CUBEMAP_POSITIVEZ | DDSCAPS2_CUBEMAP_NEGITIVEZ : 0;

  int pixelFlags = 0;
  pixelFlags |= hasAlpha ? DDPF_ALPHAPIXELS : 0;
  pixelFlags |= compress ? DDPF_FOURCC : DDPF_RGB;

  const char* fourCC = isArray ? "DX10" : "\0\0\0\0";
  int dx10Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;

#ifdef OZ_NONFREE
  int squishFlags = squish::kColourIterativeClusterFit | squish::kWeightColourByAlpha;
  squishFlags    |= hasAlpha ? squish::kDxt5 : squish::kDxt1;

  if( compress ) {
    pitchOrLinSize = squish::GetStorageRequirements( width, height, squishFlags );
    dx10Format     = hasAlpha ? DXGI_FORMAT_BC3_TYPELESS : DXGI_FORMAT_BC1_TYPELESS;
    fourCC         = isArray ? "DX10" : hasAlpha ? "DXT5" : "DXT1";
  }
#endif

  OutputStream os( 0, Endian::LITTLE );

  // Header beginning.
  os.writeChars( "DDS ", 4 );
  os.writeInt( 124 );
  os.writeInt( flags );
  os.writeInt( height );
  os.writeInt( width );
  os.writeInt( pitchOrLinSize );
  os.writeInt( 0 );
  os.writeInt( nMipmaps );

  // Reserved int[11].
  os.writeInt( 0 );
  os.writeInt( 0 );
  os.writeInt( 0 );
  os.writeInt( 0 );
  os.writeInt( 0 );
  os.writeInt( 0 );
  os.writeInt( 0 );
  os.writeInt( 0 );
  os.writeInt( 0 );
  os.writeInt( 0 );
  os.writeInt( 0 );

  // Pixel format.
  os.writeInt( 32 );
  os.writeInt( pixelFlags );
  os.writeChars( fourCC, 4 );
  os.writeInt( targetBPP );
  os.writeUInt( 0x00ff0000 );
  os.writeUInt( 0x0000ff00 );
  os.writeUInt( 0x000000ff );
  os.writeUInt( 0xff000000 );

  os.writeInt( caps );
  os.writeInt( caps2 );
  os.writeInt( 0 );
  os.writeInt( 0 );
  os.writeInt( 0 );

  if( isArray ) {
    os.writeInt( dx10Format );
    os.writeInt( D3D10_RESOURCE_DIMENSION_TEXTURE2D );
    os.writeInt( 0 );
    os.writeInt( nFaces );
    os.writeInt( 0 );
  }

  for( int i = 0; i < nFaces; ++i ) {
    FIBITMAP* face = faces[i];

    if( compress ) {
      ubyte* pixels = FreeImage_GetBits( face );
      int    size   = width * height * 4;

      for( int j = 0; j < size; j += 4 ) {
        swap( pixels[j], pixels[j + 2] );
      }
    }

    if( targetBPP == 24 ) {
      face = FreeImage_ConvertTo24Bits( faces[i] );
    }

    int levelWidth  = width;
    int levelHeight = height;

    for( int j = 0; j < nMipmaps; ++j ) {
      FIBITMAP* level = face;
      if( j != 0 ) {
        level = FreeImage_Rescale( face, levelWidth, levelHeight, FILTER_CATMULLROM );
      }

      if( compress ) {
#ifdef OZ_NONFREE
        ubyte* pixels = FreeImage_GetBits( level );
        int    s3Size = squish::GetStorageRequirements( levelWidth, levelHeight, squishFlags );

        squish::CompressImage( pixels, levelWidth, levelHeight, os.forward( s3Size ), squishFlags );
#endif
      }
      else {
        const char* pixels = reinterpret_cast<const char*>( FreeImage_GetBits( level ) );
        int         pitch  = int( FreeImage_GetPitch( level ) );

        for( int k = 0; k < levelHeight; ++k ) {
          os.writeChars( pixels, levelWidth * targetBPP / 8 );
          pixels += pitch;
        }
      }

      levelWidth  = max( 1, levelWidth / 2 );
      levelHeight = max( 1, levelHeight / 2 );

      if( j != 0 ) {
        FreeImage_Unload( level );
      }
    }

    if( face != faces[i] ) {
      FreeImage_Unload( face );
    }
  }

  bool success = destFile.write( os.begin(), os.tell() );
  if( !success ) {
    snprintf( errorBuffer, ERROR_LENGTH, "Failed to write '%s'.", destFile.path().cstr() );
    return false;
  }
  return true;
}

ImageData::ImageData() :
  width( 0 ), height( 0 ), flags( 0 ), pixels( nullptr )
{}

ImageData::ImageData( int width_, int height_ ) :
  width( width_ ), height( height_ ), flags( 0 ), pixels( new char[width * height * 4] )
{}

ImageData::~ImageData()
{
  delete[] pixels;
}

ImageData::ImageData( ImageData&& i ) :
  width( i.width ), height( i.height ), flags( i.flags ), pixels( i.pixels )
{
  i.width  = 0;
  i.height = 0;
  i.flags  = 0;
  i.pixels = nullptr;
}

ImageData& ImageData::operator = ( ImageData&& i )
{
  if( &i == this ) {
    return *this;
  }

  width  = i.width;
  height = i.height;
  flags  = i.flags;
  pixels = i.pixels;

  i.width  = 0;
  i.height = 0;
  i.flags  = 0;
  i.pixels = nullptr;

  return *this;
}

void ImageData::determineAlpha()
{
  if( pixels == nullptr ) {
    return;
  }

  int size = width * height * 4;

  flags &= ~ALPHA_BIT;

  for( int i = 0; i < size; i += 4 ) {
    if( pixels[i * 4 + 3] != char( 255 ) ) {
      flags |= ALPHA_BIT;
      return;
    }
  }
}

const char* ImageBuilder::getError()
{
  return errorBuffer;
}

bool ImageBuilder::isImage( const File& file )
{
  errorBuffer[0] = '\0';

  InputStream is        = file.inputStream();
  ubyte*      dataBegin = reinterpret_cast<ubyte*>( const_cast<char*>( is.begin() ) );

  FIMEMORY*         memoryIO = FreeImage_OpenMemory( dataBegin, uint( is.capacity() ) );
  FREE_IMAGE_FORMAT format   = FreeImage_GetFileTypeFromMemory( memoryIO, is.capacity() );

  FreeImage_CloseMemory( memoryIO );
  return format != FIF_UNKNOWN;
}

ImageData ImageBuilder::loadImage( const File& file )
{
  errorBuffer[0] = '\0';

  ImageData image;

  FIBITMAP* dib = loadBitmap( file );
  if( dib == nullptr ) {
    return image;
  }

  image = ImageData( int( FreeImage_GetWidth( dib ) ), int( FreeImage_GetHeight( dib ) ) );

  // Copy and convert BGRA -> RGBA.
  int    size   = image.width * image.height * 4;
  ubyte* pixels = FreeImage_GetBits( dib );

  for( int i = 0; i < size; i += 4 ) {
    image.pixels[i + 0] = char( pixels[i + 2] );
    image.pixels[i + 1] = char( pixels[i + 1] );
    image.pixels[i + 2] = char( pixels[i + 0] );
    image.pixels[i + 3] = char( pixels[i + 3] );
  }

  return image;
}

bool ImageBuilder::createDDS( const ImageData* faces, int nFaces, int options,
                              const File& destFile )
{
  errorBuffer[0] = '\0';

  if( nFaces < 1 ) {
    snprintf( errorBuffer, ERROR_LENGTH, "At least one face must be given." );
    return false;
  }

  FIBITMAP** dibs = new FIBITMAP*[nFaces];

  for( int i = 0; i < nFaces; ++i ) {
    dibs[i] = createBitmap( faces[i] );

    if( dibs[i] == nullptr ) {
      for( int j = 0; j <= i; ++j ) {
        FreeImage_Unload( dibs[j] );
      }
    }
  }

  bool success = buildDDS( dibs, nFaces, options, destFile );

  for( int i = 0; i < nFaces; ++i ) {
    FreeImage_Unload( dibs[i] );
  }
  return success;
}

bool ImageBuilder::convertToDDS( const File& file, int options, const char* destPath )
{
  errorBuffer[0] = '\0';

  if( file.hasExtension( "dds" ) ) {
    return File::cp( file, destPath );
  }

  File destFile = destPath;
  if( destFile.type() == File::DIRECTORY ) {
    destFile = String::str( "%s/%s.dds", destPath, file.baseName().cstr() );
  }

  FIBITMAP* dib = loadBitmap( file );
  if( dib == nullptr ) {
    return false;
  }
  bool success = buildDDS( &dib, 1, options, destFile );

  FreeImage_Unload( dib );
  return success;
}

}
