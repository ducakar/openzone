/*
 * ozFactory - OpenZone Assets Builder Library.
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

static const int DDSD_CAPS            = 0x00000001;
static const int DDSD_HEIGHT          = 0x00000002;
static const int DDSD_WIDTH           = 0x00000004;
static const int DDSD_PITCH           = 0x00000008;
static const int DDSD_PIXELFORMAT     = 0x00001000;
static const int DDSD_MIPMAPCOUNT     = 0x00020000;
static const int DDSD_LINEARSIZE      = 0x00080000;

static const int DDSDCAPS_COMPLEX     = 0x00000008;
static const int DDSDCAPS_TEXTURE     = 0x00001000;
static const int DDSDCAPS_MIPMAP      = 0x00400000;

static const int DDPF_ALPHAPIXELS     = 0x00000001;
static const int DDPF_FOURCC          = 0x00000004;
static const int DDPF_RGB             = 0x00000040;

static const int ERROR_LENGTH         = 1024;

static char errorBuffer[ERROR_LENGTH] = {};

static bool buildDDS( FIBITMAP* dib, bool doMipmaps, bool compress, const File& destFile )
{
  int width  = int( FreeImage_GetWidth( dib ) );
  int height = int( FreeImage_GetHeight( dib ) );
  int bpp    = int( FreeImage_GetBPP( dib ) );

  if( bpp != 24 && bpp != 32 ) {
    snprintf( errorBuffer, ERROR_LENGTH, "Image should be either 24-bit RGB or 32-bit RGBA." );
    return false;
  }

#ifndef OZ_NONFREE
  if( compress ) {
    snprintf( errorBuffer, ERROR_LENGTH, "Texture compression requested, but compiled without"
                                         "libsquish (enable OZ_NONFREE)." );
    return false;
  }
#endif

  if( compress && ( !Math::isPow2( width ) || !Math::isPow2( height ) ) ) {
    snprintf( errorBuffer, ERROR_LENGTH, "Compressed texture dimensions must be powers of 2." );
    return false;
  }

  int pitchOrLinSize = ( ( width * bpp / 8 + 3 ) / 4 ) * 4;
  int nMipmaps       = doMipmaps ? Math::index1( max( width, height ) ) + 1 : 1;

  int flags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
  flags |= doMipmaps ? DDSD_MIPMAPCOUNT : 0;
  flags |= compress ? DDSD_LINEARSIZE : DDSD_PITCH;

  int caps = DDSDCAPS_TEXTURE;
  caps |= doMipmaps ? DDSDCAPS_COMPLEX | DDSDCAPS_MIPMAP : 0;

  int pixelFlags = 0;
  pixelFlags |= bpp == 32 ? DDPF_ALPHAPIXELS : 0;
  pixelFlags |= compress ? DDPF_FOURCC : DDPF_RGB;

  const char* compression = "\0\0\0\0";

#ifdef OZ_NONFREE
  int squishFlags = bpp == 32 ? squish::kDxt5 : squish::kDxt1;
  squishFlags    |= squish::kColourIterativeClusterFit | squish::kWeightColourByAlpha;

  if( compress ) {
    pitchOrLinSize = squish::GetStorageRequirements( width, height, squishFlags );
    compression    = bpp == 32 ? "DXT5" : "DXT1";
  }
#endif

  OutputStream ostream( 0, Endian::LITTLE );

  // Header beginning.
  ostream.writeChars( "DDS ", 4 );
  ostream.writeInt( 124 );
  ostream.writeInt( flags );
  ostream.writeInt( height );
  ostream.writeInt( width );
  ostream.writeInt( pitchOrLinSize );
  ostream.writeInt( 0 );
  ostream.writeInt( nMipmaps );

  // Reserved int[11].
  ostream.writeInt( 0 );
  ostream.writeInt( 0 );
  ostream.writeInt( 0 );
  ostream.writeInt( 0 );
  ostream.writeInt( 0 );
  ostream.writeInt( 0 );
  ostream.writeInt( 0 );
  ostream.writeInt( 0 );
  ostream.writeInt( 0 );
  ostream.writeInt( 0 );
  ostream.writeInt( 0 );

  // Pixel format.
  ostream.writeInt( 32 );
  ostream.writeInt( pixelFlags );
  ostream.writeChars( compression, 4 );
  ostream.writeInt( bpp );
  ostream.writeUInt( 0x00ff0000 );
  ostream.writeUInt( 0x0000ff00 );
  ostream.writeUInt( 0x000000ff );
  ostream.writeUInt( 0xff000000 );

  ostream.writeInt( caps );
  ostream.writeInt( 0 );
  ostream.writeInt( 0 );
  ostream.writeInt( 0 );
  ostream.writeInt( 0 );

  for( int i = 0; i < nMipmaps; ++i ) {
    FIBITMAP* level = dib;

    if( i != 0 ) {
      width  = max( 1, width / 2 );
      height = max( 1, height / 2 );
      level  = FreeImage_Rescale( dib, width, height, FILTER_CATMULLROM );
    }

    if( compress ) {
#ifdef OZ_NONFREE
      ubyte* pixels = FreeImage_GetBits( level );
      int    size   = width * height * 4;
      int    s3Size = squish::GetStorageRequirements( width, height, squishFlags );

      for( int i = 0; i < size; i += 4 ) {
        swap( pixels[i], pixels[i + 2] );
      }

      squish::CompressImage( pixels, width, height, ostream.forward( s3Size ), squishFlags );
#endif
    }
    else {
      const char* pixels = reinterpret_cast<const char*>( FreeImage_GetBits( level ) );
      int         pitch  = int( FreeImage_GetPitch( level ) );

      for( int i = 0; i < height; ++i ) {
        ostream.writeChars( pixels, width * bpp / 8 );
        pixels += pitch;
      }
    }

    if( level != dib ) {
      FreeImage_Unload( level );
    }
  }

  bool success = destFile.write( ostream.begin(), ostream.tell() );
  if( !success ) {
    snprintf( errorBuffer, ERROR_LENGTH, "Failed to write '%s'", destFile.path().cstr() );
    return false;
  }
  return true;
}

const char* ImageBuilder::getError()
{
  return errorBuffer;
}

bool ImageBuilder::isImage( const File& file )
{
  errorBuffer[0] = '\0';

  InputStream istream = file.inputStream();

  ubyte* dataBegin = reinterpret_cast<ubyte*>( const_cast<char*>( istream.begin() ) );

  FIMEMORY*         memoryIO = FreeImage_OpenMemory( dataBegin, uint( istream.capacity() ) );
  FREE_IMAGE_FORMAT format   = FreeImage_GetFileTypeFromMemory( memoryIO, istream.capacity() );

  FreeImage_CloseMemory( memoryIO );
  return format != FIF_UNKNOWN;
}

bool ImageBuilder::createDDS( const void* data, int width, int height, int bpp, int options,
                              const File& destFile )
{
  errorBuffer[0] = '\0';

  int    pixelSize = bpp / 8;
  int    pitch     = ( ( width * pixelSize + 3 ) / 4 ) * 4;
  int    size      = height * pitch;
  ubyte* image     = new ubyte[size];

  mCopy( image, data, size_t( size ) );

  // RGB(A) -> BGR(A)
  for( int y = 0; y < height; ++y ) {
    ubyte* pixels = &image[y * pitch];

    for( int x = 0; x < width; ++x ) {
      swap( pixels[0], pixels[2] );
      pixels += pixelSize;
    }
  }

  FIBITMAP* dib = FreeImage_ConvertFromRawBits( image, width, height, pitch, uint( bpp ),
                                                FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK,
                                                FI_RGBA_BLUE_MASK );
  if( dib == nullptr ) {
    snprintf( errorBuffer, ERROR_LENGTH, "FreeImage_ConvertFromRawBits failed to build image." );
    return false;
  }

  bool success = buildDDS( dib, options & MIPMAPS_BIT, options & COMPRESSION_BIT, destFile );

  FreeImage_Unload( dib );
  return success;
}

bool ImageBuilder::convertToDDS( const File& file, int options, const char* destPath )
{
  errorBuffer[0] = '\0';

  if( file.hasExtension( "dds" ) ) {
    return File::cp( file, destPath );
  }

  InputStream       istream   = file.inputStream();
  ubyte*            dataBegin = reinterpret_cast<ubyte*>( const_cast<char*>( istream.begin() ) );
  FIMEMORY*         memoryIO  = FreeImage_OpenMemory( dataBegin, uint( istream.capacity() ) );
  FREE_IMAGE_FORMAT format    = FreeImage_GetFileTypeFromMemory( memoryIO, istream.capacity() );
  FIBITMAP*         dib       = FreeImage_LoadFromMemory( format, memoryIO );

  FreeImage_CloseMemory( memoryIO );

  if( dib == nullptr ) {
    snprintf( errorBuffer, ERROR_LENGTH, "Failed to read '%s'", file.path().cstr() );
    return false;
  }

  bool isTransparent = FreeImage_IsTransparent( dib );
  FIBITMAP* newDib = isTransparent ? FreeImage_ConvertTo32Bits( dib ) :
                                     FreeImage_ConvertTo24Bits( dib );
  FreeImage_Unload( dib );
  FreeImage_FlipVertical( newDib );
  dib = newDib;

  File destFile = destPath;
  if( destFile.type() == File::DIRECTORY ) {
    destFile = String::str( "%s/%s.dds", destPath, file.baseName().cstr() );
  }

  bool success = buildDDS( dib, options & MIPMAPS_BIT, options & COMPRESSION_BIT, destFile );

  FreeImage_Unload( dib );
  return success;
}

}
