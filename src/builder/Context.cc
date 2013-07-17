/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#include <builder/Context.hh>

#include <client/SMMImago.hh>
#include <client/SMMVehicleImago.hh>
#include <client/ExplosionImago.hh>
#include <client/MD2Imago.hh>
#include <client/MD2WeaponImago.hh>
#include <client/BasicAudio.hh>
#include <client/BotAudio.hh>
#include <client/VehicleAudio.hh>

#include <FreeImage.h>
#ifdef OZ_NONFREE
# include <squish.h>
#endif

namespace oz
{
namespace builder
{

static const int DDSD_CAPS        = 0x00000001;
static const int DDSD_HEIGHT      = 0x00000002;
static const int DDSD_WIDTH       = 0x00000004;
static const int DDSD_PITCH       = 0x00000008;
static const int DDSD_PIXELFORMAT = 0x00001000;
static const int DDSD_MIPMAPCOUNT = 0x00020000;
static const int DDSD_LINEARSIZE  = 0x00080000;

static const int DDSDCAPS_COMPLEX = 0x00000008;
static const int DDSDCAPS_TEXTURE = 0x00001000;
static const int DDSDCAPS_MIPMAP  = 0x00400000;

static const int DDPF_ALPHAPIXELS = 0x00000001;
static const int DDPF_FOURCC      = 0x00000004;
static const int DDPF_RGB         = 0x00000040;
static const int DDPF_LUMINANCE   = 0x00020000;

const char* const IMAGE_EXTENSIONS[] = {
  ".png",
  ".jpeg",
  ".jpg",
  ".bmp",
  ".tga",
  ".tiff"
};

static FIBITMAP* loadImage( const File& file, bool force24Bits )
{
  InputStream       istream   = file.inputStream();
  ubyte*            dataBegin = reinterpret_cast<ubyte*>( const_cast<char*>( istream.begin() ) );
  FIMEMORY*         memoryIO  = FreeImage_OpenMemory( dataBegin, uint( istream.capacity() ) );
  FREE_IMAGE_FORMAT format    = FreeImage_GetFileTypeFromMemory( memoryIO, istream.capacity() );
  FIBITMAP*         image     = FreeImage_LoadFromMemory( format, memoryIO );

  FreeImage_CloseMemory( memoryIO );

  if( image == nullptr ) {
    OZ_ERROR( "Failed to read '%s'", file.path().cstr() );
  }

  bool isTransparent = FreeImage_IsTransparent( image );
  FIBITMAP* newImage = isTransparent && !force24Bits ? FreeImage_ConvertTo32Bits( image ) :
                                                       FreeImage_ConvertTo24Bits( image );
  FreeImage_Unload( image );
  FreeImage_FlipVertical( newImage );
  return newImage;
}

static void writeDDS( FIBITMAP* image, const char* filePath )
{
  int width  = int( FreeImage_GetWidth( image ) );
  int height = int( FreeImage_GetHeight( image ) );
  int bpp    = int( FreeImage_GetBPP( image ) );

  if( context.useS3TC && ( !Math::isPow2( width ) || !Math::isPow2( height ) ) ) {
    OZ_ERROR( "Dimensions of compressed textures must be powers of 2" );
  }

  bool isTransparent  = bpp == 32;
  int  pitchOrLinSize = width * ( bpp / 8 );
  int  nMipmaps       = Math::index1( max( width, height ) ) + 1;

  int flags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_MIPMAPCOUNT;
  flags |= context.useS3TC ? DDSD_LINEARSIZE : DDSD_PITCH;

  int caps = DDSDCAPS_TEXTURE | DDSDCAPS_COMPLEX | DDSDCAPS_MIPMAP;

  int pixelFlags = 0;
  pixelFlags |= isTransparent ? DDPF_ALPHAPIXELS : 0;
  pixelFlags |= context.useS3TC ? DDPF_FOURCC :
                bpp == 8 ? DDPF_LUMINANCE : DDPF_RGB;

  const char* compression = "\0\0\0\0";

#ifdef OZ_NONFREE
  int squishFlags = isTransparent ? squish::kDxt5 : squish::kDxt1;
  squishFlags    |= squish::kColourIterativeClusterFit | squish::kWeightColourByAlpha;

  if( context.useS3TC ) {
    pitchOrLinSize = squish::GetStorageRequirements( width, height, squishFlags );
    compression    = isTransparent ? "DXT5" : "DXT1";
  }
#endif

  OutputStream ostream( 0 );

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
    FIBITMAP* level = image;

    if( i != 0 ) {
      width  = max( 1, width / 2 );
      height = max( 1, height / 2 );
      level  = FreeImage_Rescale( image, width, height, FILTER_CATMULLROM );
    }

    if( context.useS3TC ) {
#ifdef OZ_NONFREE
      FIBITMAP* level32 = FreeImage_ConvertTo32Bits( level );
      ubyte*    pixels  = FreeImage_GetBits( level32 );
      int       size    = width * height * 4;
      int       s3Size  = squish::GetStorageRequirements( width, height, squishFlags );

      for( int i = 0; i < size; i += 4 ) {
        swap( pixels[i], pixels[i + 2] );
      }

      squish::CompressImage( pixels, width, height, ostream.forward( s3Size ), squishFlags );

      FreeImage_Unload( level32 );
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

    if( level != image ) {
      FreeImage_Unload( level );
    }
  }

  File destFile( filePath );
  if( !destFile.write( ostream.begin(), ostream.tell() ) ) {
    OZ_ERROR( "Failed to write '%s'", filePath );
  }
}

bool Context::isBaseTexture( const char* path_ )
{
  String path = path_;
  return !path.endsWith( "_d" ) && !path.endsWith( "_m" ) && !path.endsWith( "_s" ) &&
         !path.endsWith( "_spec" ) && !path.endsWith( ".blend" ) && !path.endsWith( "_n" ) &&
         !path.endsWith( "_nm" ) && !path.endsWith( "_normal" ) && !path.endsWith( "_local" );
}

void Context::buildTexture( const char* basePath_, const char* destBasePath_ )
{
  Log::print( "Building texture(s) '%s' -> '%s' ...", basePath_, destBasePath_ );

  String destBasePath      = destBasePath_;
  String basePath          = basePath_;
  String diffuseBasePath   = basePath;
  String masksBasePath     = basePath + "_m";
  String specularBasePath  = basePath + "_s";
  String specular1BasePath = basePath + "_spec";
  String emissionBasePath  = basePath + ".blend";
  String normalsBasePath   = basePath + "_n";
  String normals1BasePath  = basePath + "_nm";
  String normals2BasePath  = basePath + "_normal";
  String normals3BasePath  = basePath + "_local";

  File diffuse, masks, specular, emission, normals;

  for( int i = 0; i < aLength( IMAGE_EXTENSIONS ); ++i ) {
    if( diffuse.path().isEmpty() || diffuse.type() == File::MISSING ) {
      diffuse = File( diffuseBasePath + IMAGE_EXTENSIONS[i] );
    }

    if( masks.path().isEmpty() || masks.type() == File::MISSING ) {
      masks = File( masksBasePath + IMAGE_EXTENSIONS[i] );
    }

    if( specular.path().isEmpty() || specular.type() == File::MISSING ) {
      specular = File( specularBasePath + IMAGE_EXTENSIONS[i] );
    }
    if( specular.type() == File::MISSING ) {
      specular = File( specular1BasePath + IMAGE_EXTENSIONS[i] );
    }

    if( emission.path().isEmpty() || emission.type() == File::MISSING ) {
      emission = File( emissionBasePath + IMAGE_EXTENSIONS[i] );
    }

    if( normals.path().isEmpty() || normals.type() == File::MISSING ) {
      normals = File( normalsBasePath + IMAGE_EXTENSIONS[i] );
    }
    if( normals.type() == File::MISSING ) {
      normals = File( normals1BasePath + IMAGE_EXTENSIONS[i] );
    }
    if( normals.type() == File::MISSING ) {
      normals = File( normals2BasePath + IMAGE_EXTENSIONS[i] );
    }
    if( normals.type() == File::MISSING ) {
      normals = File( normals3BasePath + IMAGE_EXTENSIONS[i] );
    }
  }

  if( diffuse.type() != File::MISSING ) {
    FIBITMAP* image = loadImage( diffuse, false );

    writeDDS( image, destBasePath + ".dds" );
    FreeImage_Unload( image );
  }
  else {
    OZ_ERROR( "Missing texture '%s' (.png, .jpeg, .jpg and .tga checked)", basePath.cstr() );
  }

  if( masks.type() != File::MISSING ) {
    FIBITMAP* image = loadImage( masks, true );

    writeDDS( image, destBasePath + "_m.dds" );
    FreeImage_Unload( image );
  }
  else {
    FIBITMAP* specularImage  = nullptr;
    ubyte*    specularPixels = nullptr;
    int       specularWidth  = 0;
    int       specularHeight = 0;

    FIBITMAP* emissionImage  = nullptr;
    ubyte*    emissionPixels = nullptr;
    int       emissionWidth  = 0;
    int       emissionHeight = 0;

    if( specular.type() != File::MISSING ) {
      specularImage  = loadImage( specular, true );
      specularPixels = FreeImage_GetBits( specularImage );
      specularWidth  = int( FreeImage_GetWidth( specularImage ) );
      specularHeight = int( FreeImage_GetHeight( specularImage ) );
    }
    if( emission.type() != File::MISSING ) {
      emissionImage  = loadImage( emission, true );
      emissionPixels = FreeImage_GetBits( emissionImage );
      emissionWidth  = int( FreeImage_GetWidth( emissionImage ) );
      emissionHeight = int( FreeImage_GetHeight( emissionImage ) );
    }

    if( specularImage == nullptr && emissionImage == nullptr ) {
      // Drop through.
    }
    else if( emissionImage == nullptr ) {
      for( int i = 0; i < specularWidth * specularHeight; ++i ) {
        ubyte& b = specularPixels[i*3 + 0];
        ubyte& g = specularPixels[i*3 + 1];
        ubyte& r = specularPixels[i*3 + 2];

        r = ubyte( ( b + g + r ) / 3 );
        g = 0;
        b = 0;
      }

      writeDDS( specularImage, destBasePath + "_m.dds" );
      FreeImage_Unload( specularImage );
    }
    else if( specularImage == nullptr ) {
      for( int i = 0; i < emissionWidth * emissionHeight; ++i ) {
        ubyte& b = emissionPixels[i*3 + 0];
        ubyte& g = emissionPixels[i*3 + 1];
        ubyte& r = emissionPixels[i*3 + 2];

        r = 0;
        g = ubyte( ( b + g + r ) / 3 );
        b = 0;
      }

      writeDDS( emissionImage, destBasePath + "_m.dds" );
      FreeImage_Unload( emissionImage );
    }
    else {
      if( specularWidth != emissionWidth || specularHeight != emissionHeight ) {
        OZ_ERROR( "Specular and emission texture masks must have the same size." );
      }

      for( int i = 0; i < specularWidth * specularHeight; ++i ) {
        ubyte& b = specularPixels[i*3 + 0];
        ubyte& g = specularPixels[i*3 + 1];
        ubyte& r = specularPixels[i*3 + 2];

        ubyte& eb = emissionPixels[i*3 + 0];
        ubyte& eg = emissionPixels[i*3 + 1];
        ubyte& er = emissionPixels[i*3 + 2];

        r = ubyte( ( b + g + r ) / 3 );
        g = ubyte( ( eb + eg + er ) / 3 );
        b = 0;
      }

      writeDDS( specularImage, destBasePath + "_m.dds" );
      FreeImage_Unload( specularImage );
      FreeImage_Unload( emissionImage );
    }
  }

  if( bumpmap && normals.type() != File::MISSING ) {
    FIBITMAP* image = loadImage( normals, true );

    writeDDS( image, destBasePath + "_n.dds" );
    FreeImage_Unload( image );
  }

  Log::printEnd( " OK" );
}

void Context::init()
{}

void Context::destroy()
{
  usedTextures.clear();
  usedTextures.deallocate();
  usedSounds.clear();
  usedSounds.deallocate();
  usedModels.clear();
  usedModels.deallocate();
}

Context context;

}
}
