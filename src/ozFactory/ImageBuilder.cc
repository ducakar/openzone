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

static const uint DDSD_CAPS                          = 0x00000001;
static const uint DDSD_HEIGHT                        = 0x00000002;
static const uint DDSD_WIDTH                         = 0x00000004;
static const uint DDSD_PITCH                         = 0x00000008;
static const uint DDSD_PIXELFORMAT                   = 0x00001000;
static const uint DDSD_MIPMAPCOUNT                   = 0x00020000;
static const uint DDSD_LINEARSIZE                    = 0x00080000;

static const uint DDSCAPS_COMPLEX                    = 0x00000008;
static const uint DDSCAPS_MIPMAP                     = 0x00400000;
static const uint DDSCAPS_TEXTURE                    = 0x00001000;

static const uint DDSCAPS2_CUBEMAP                   = 0x00000200;
static const uint DDSCAPS2_CUBEMAP_POSITIVEX         = 0x00000400;
static const uint DDSCAPS2_CUBEMAP_NEGITIVEX         = 0x00000800;
static const uint DDSCAPS2_CUBEMAP_POSITIVEY         = 0x00001000;
static const uint DDSCAPS2_CUBEMAP_NEGITIVEY         = 0x00002000;
static const uint DDSCAPS2_CUBEMAP_POSITIVEZ         = 0x00004000;
static const uint DDSCAPS2_CUBEMAP_NEGITIVEZ         = 0x00008000;

static const uint DDPF_ALPHAPIXELS                   = 0x00000001;
static const uint DDPF_FOURCC                        = 0x00000004;
static const uint DDPF_RGB                           = 0x00000040;
static const uint DDPF_NORMAL                        = 0x80000000;

static const uint DXGI_FORMAT_R8G8B8A8_UNORM         = 28;
#ifdef OZ_NONFREE
static const uint DXGI_FORMAT_BC1_UNORM              = 71;
static const uint DXGI_FORMAT_BC3_UNORM              = 77;
#endif

static const int D3D10_RESOURCE_DIMENSION_TEXTURE2D = 3;

static const int ERROR_LENGTH                       = 1024;

static char errorBuffer[ERROR_LENGTH]               = {};

static void printError(FREE_IMAGE_FORMAT fif, const char* message)
{
  Log::println("FreeImage(%s): %s", FreeImage_GetFormatFromFIF(fif), message);
}

static FIBITMAP* createBitmap(const ImageData& image)
{
  FIBITMAP* dib = FreeImage_ConvertFromRawBits(reinterpret_cast<ubyte*>(image.pixels),
                                               image.width, image.height, image.width * 4, 32,
                                               FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK,
                                               FI_RGBA_BLUE_MASK);

  // Convert RGBA -> BGRA.
  int    size   = image.width * image.height * 4;
  ubyte* pixels = FreeImage_GetBits(dib);

  for (int i = 0; i < size; i += 4) {
    swap(pixels[i + 0], pixels[i + 2]);
  }

  if (dib == nullptr) {
    snprintf(errorBuffer, ERROR_LENGTH, "FreeImage_ConvertFromRawBits failed to build image.");
  }

  FreeImage_SetTransparent(dib, image.flags & ImageData::ALPHA_BIT);
  return dib;
}

static FIBITMAP* loadBitmap(const File& file)
{
  Stream            is        = file.inputStream();
  ubyte*            dataBegin = reinterpret_cast<ubyte*>(const_cast<char*>(is.begin()));
  FIMEMORY*         memoryIO  = FreeImage_OpenMemory(dataBegin, is.capacity());
  FREE_IMAGE_FORMAT format    = FreeImage_GetFileTypeFromMemory(memoryIO, is.capacity());
  FIBITMAP*         dib       = FreeImage_LoadFromMemory(format < 0 ? FIF_TARGA : format, memoryIO);

  FreeImage_CloseMemory(memoryIO);

  if (dib == nullptr) {
    snprintf(errorBuffer, ERROR_LENGTH, "Failed to read '%s'.", file.c());
    return nullptr;
  }

  FIBITMAP* oldDib = dib;
  dib = FreeImage_ConvertTo32Bits(dib);
  FreeImage_Unload(oldDib);

  // Remove alpha if unused.
  int    width    = FreeImage_GetWidth(dib);
  int    height   = FreeImage_GetHeight(dib);
  int    size     = width * height * 4;
  bool   hasAlpha = false;
  ubyte* pixels   = FreeImage_GetBits(dib);

  for (int i = 0; i < size; i += 4) {
    if (pixels[i + 3] != 255) {
      hasAlpha = true;
      break;
    }
  }
  if (!hasAlpha) {
    FreeImage_SetTransparent(dib, false);
  }

  FreeImage_FlipVertical(dib);
  return dib;
}

static bool buildDDS(const ImageData* faces, int nFaces, const File& destFile)
{
  hard_assert(nFaces > 0);

  int width      = faces[0].width;
  int height     = faces[0].height;

  bool isCubeMap = ImageBuilder::options & ImageBuilder::CUBE_MAP_BIT;
  bool isNormal  = ImageBuilder::options & ImageBuilder::NORMAL_MAP_BIT;
  bool doMipmaps = ImageBuilder::options & ImageBuilder::MIPMAPS_BIT;
  bool compress  = ImageBuilder::options & ImageBuilder::COMPRESSION_BIT;
  bool doFlip    = ImageBuilder::options & ImageBuilder::FLIP_BIT;
  bool doFlop    = ImageBuilder::options & ImageBuilder::FLOP_BIT;
  bool doYYYX    = ImageBuilder::options & ImageBuilder::YYYX_BIT;
  bool doZYZX    = ImageBuilder::options & ImageBuilder::ZYZX_BIT;
  bool hasAlpha  = (faces[0].flags & ImageData::ALPHA_BIT) || doYYYX || doZYZX;
  bool isArray   = !isCubeMap && nFaces > 1;

  for (int i = 1; i < nFaces; ++i) {
    if (faces[i].width != width || faces[i].height != height) {
      snprintf(errorBuffer, ERROR_LENGTH, "All faces must have the same dimensions.");
      return false;
    }
  }

  if (compress) {
#ifndef OZ_NONFREE
    snprintf(errorBuffer, ERROR_LENGTH, "Texture compression requested but compiled without"
             " libsquish (OZ_NONFREE is disabled).");
    return false;
#else
    compress = Math::isPow2(width) && Math::isPow2(height);
#endif
  }

  if (isCubeMap && nFaces != 6) {
    snprintf(errorBuffer, ERROR_LENGTH, "Cube map requires exactly 6 faces.");
    return false;
  }

  int targetWidth    = max(Math::lround(width * ImageBuilder::scale), 1);
  int targetHeight   = max(Math::lround(height * ImageBuilder::scale), 1);
  int targetBPP      = hasAlpha || compress || isArray ? 32 : 24;
  int pitchOrLinSize = ((targetWidth * targetBPP / 8 + 3) / 4) * 4;
  int nMipmaps       = doMipmaps ? Math::index1(max(targetWidth, targetHeight)) + 1 : 1;

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
  pixelFlags |= isNormal ? DDPF_NORMAL : 0;

  const char* fourCC = isArray ? "DX10" : "\0\0\0\0";
  int dx10Format = DXGI_FORMAT_R8G8B8A8_UNORM;

#ifdef OZ_NONFREE
  bool isFast     = ImageBuilder::options & ImageBuilder::FAST_BIT;
  int squishFlags = 0;

  squishFlags |= isFast   ? squish::kColourRangeFit : squish::kColourIterativeClusterFit;
  squishFlags |= hasAlpha ? squish::kDxt5 | squish::kWeightColourByAlpha : squish::kDxt1;

  if (compress) {
    pitchOrLinSize = squish::GetStorageRequirements(targetWidth, targetHeight, squishFlags);
    dx10Format     = hasAlpha ? DXGI_FORMAT_BC3_UNORM : DXGI_FORMAT_BC1_UNORM;
    fourCC         = isArray ? "DX10" : hasAlpha ? "DXT5" : "DXT1";
  }
#endif

  Stream os(0, Endian::LITTLE);

  // Header beginning.
  os.writeChars("DDS ", 4);
  os.writeInt(124);
  os.writeInt(flags);
  os.writeInt(targetHeight);
  os.writeInt(targetWidth);
  os.writeInt(pitchOrLinSize);
  os.writeInt(0);
  os.writeInt(nMipmaps);

  // Reserved int[11].
  os.writeInt(0);
  os.writeInt(0);
  os.writeInt(0);
  os.writeInt(0);
  os.writeInt(0);
  os.writeInt(0);
  os.writeInt(0);
  os.writeInt(0);
  os.writeInt(0);
  os.writeInt(0);
  os.writeInt(0);

  // Pixel format.
  os.writeInt(32);
  os.writeInt(pixelFlags);
  os.writeChars(fourCC, 4);

  if (compress) {
    os.writeInt(0);
    os.writeUInt(0);
    os.writeUInt(0);
    os.writeUInt(0);
    os.writeUInt(0);
  }
  else {
    os.writeInt(targetBPP);
    os.writeUInt(0x00ff0000);
    os.writeUInt(0x0000ff00);
    os.writeUInt(0x000000ff);
    os.writeUInt(0xff000000);
  }

  os.writeInt(caps);
  os.writeInt(caps2);
  os.writeInt(0);
  os.writeInt(0);
  os.writeInt(0);

  if (isArray) {
    os.writeInt(dx10Format);
    os.writeInt(D3D10_RESOURCE_DIMENSION_TEXTURE2D);
    os.writeInt(0);
    os.writeInt(nFaces);
    os.writeInt(0);
  }

  for (int i = 0; i < nFaces; ++i) {
    FIBITMAP* face = createBitmap(faces[i]);

    if (doFlip) {
      FreeImage_FlipVertical(face);
    }
    if (doFlop) {
      FreeImage_FlipHorizontal(face);
    }

    if (doYYYX) {
      FreeImage_SetTransparent(face, true);

      ubyte* pixels = FreeImage_GetBits(face);
      int    size   = width * height * 4;

      for (int j = 0; j < size; j += 4) {
        pixels[j + 3] = pixels[j + 2];
        pixels[j + 0] = pixels[j + 1];
        pixels[j + 2] = pixels[j + 1];
      }
    }
    else if (doZYZX) {
      FreeImage_SetTransparent(face, true);

      ubyte* pixels = FreeImage_GetBits(face);
      int    size   = width * height * 4;

      for (int j = 0; j < size; j += 4) {
        pixels[j + 3] = pixels[j + 2];
        pixels[j + 2] = pixels[j + 0];
      }
    }
    else if (compress) {
      ubyte* pixels = FreeImage_GetBits(face);
      int    size   = width * height * 4;

      for (int j = 0; j < size; j += 4) {
        swap(pixels[j], pixels[j + 2]);
      }
    }

    if (targetBPP == 24) {
      face = FreeImage_ConvertTo24Bits(face);
    }

    int levelWidth  = targetWidth;
    int levelHeight = targetHeight;

    for (int j = 0; j < nMipmaps; ++j) {
      FIBITMAP* level = face;

      if (levelWidth != width || levelHeight != height) {
        level = FreeImage_Rescale(face, levelWidth, levelHeight, FILTER_CATMULLROM);
      }

      if (compress) {
#ifdef OZ_NONFREE
        ubyte* pixels = FreeImage_GetBits(level);
        int    s3Size = squish::GetStorageRequirements(levelWidth, levelHeight, squishFlags);

        squish::CompressImage(pixels, levelWidth, levelHeight, os.writeSkip(s3Size), squishFlags);
#endif
      }
      else {
        const char* pixels = reinterpret_cast<const char*>(FreeImage_GetBits(level));
        int         pitch  = FreeImage_GetPitch(level);

        for (int k = 0; k < levelHeight; ++k) {
          os.writeChars(pixels, levelWidth * targetBPP / 8);
          pixels += pitch;
        }
      }

      levelWidth  = max(1, levelWidth / 2);
      levelHeight = max(1, levelHeight / 2);

      if (level != face) {
        FreeImage_Unload(level);
      }
    }

    FreeImage_Unload(face);
  }

  bool success = destFile.write(os.begin(), os.tell());
  if (!success) {
    snprintf(errorBuffer, ERROR_LENGTH, "Failed to write '%s'.", destFile.c());
    return false;
  }
  return true;
}

int   ImageBuilder::options = 0;
float ImageBuilder::scale   = 1.0f;

ImageData::ImageData() :
  width(0), height(0), flags(0), pixels(nullptr)
{}

ImageData::ImageData(int width_, int height_) :
  width(width_), height(height_), flags(0), pixels(new char[width* height * 4])
{}

ImageData::~ImageData()
{
  delete[] pixels;
}

ImageData::ImageData(ImageData&& i) :
  width(i.width), height(i.height), flags(i.flags), pixels(i.pixels)
{
  i.width  = 0;
  i.height = 0;
  i.flags  = 0;
  i.pixels = nullptr;
}

ImageData& ImageData::operator = (ImageData&& i)
{
  if (&i != this) {
    width  = i.width;
    height = i.height;
    flags  = i.flags;
    pixels = i.pixels;

    i.width  = 0;
    i.height = 0;
    i.flags  = 0;
    i.pixels = nullptr;
  }
  return *this;
}

void ImageData::determineAlpha()
{
  if (pixels == nullptr) {
    return;
  }

  int size = width * height * 4;

  flags &= ~ALPHA_BIT;

  for (int i = 0; i < size; i += 4) {
    if (pixels[i + 3] != char(255)) {
      flags |= ALPHA_BIT;
      return;
    }
  }
}

bool ImageData::isNormalMap() const
{
  if (pixels == nullptr) {
    return false;
  }

  int  size    = width * height * 4;
  Vec4 average = Vec4::ZERO;

  for (int i = 0; i < size; i += 4) {
    Vec4 c = Vec4(ubyte(pixels[i + 0]) / 255.0f - 0.5f,
                  ubyte(pixels[i + 1]) / 255.0f - 0.5f,
                  ubyte(pixels[i + 2]) / 255.0f - 0.5f,
                  ubyte(pixels[i + 3]) / 255.0f);

    if (abs(1.0f - c.sqN()) > 1.0f || c.w < 0.9f) {
      return false;
    }

    average += c;
  }

  average /= float(width * height);
  return (average - Vec4(0.0f, 0.0f, 0.5f, 1.0f)).sqN() < 0.1f;
}

const char* ImageBuilder::getError()
{
  return errorBuffer;
}

bool ImageBuilder::isImage(const File& file)
{
  errorBuffer[0] = '\0';

  Stream            is        = file.inputStream();
  ubyte*            dataBegin = reinterpret_cast<ubyte*>(const_cast<char*>(is.begin()));
  FIMEMORY*         memoryIO  = FreeImage_OpenMemory(dataBegin, is.capacity());
  FREE_IMAGE_FORMAT format    = FreeImage_GetFileTypeFromMemory(memoryIO, is.capacity());

  FreeImage_CloseMemory(memoryIO);
  return format != FIF_UNKNOWN;
}

ImageData ImageBuilder::loadImage(const File& file)
{
  errorBuffer[0] = '\0';

  ImageData image;

  FIBITMAP* dib = loadBitmap(file);
  if (dib == nullptr) {
    return image;
  }

  image = ImageData(FreeImage_GetWidth(dib), FreeImage_GetHeight(dib));

  // Copy and convert BGRA -> RGBA.
  int    size   = image.width * image.height * 4;
  ubyte* pixels = FreeImage_GetBits(dib);

  for (int i = 0; i < size; i += 4) {
    image.pixels[i + 0] = char(pixels[i + 2]);
    image.pixels[i + 1] = char(pixels[i + 1]);
    image.pixels[i + 2] = char(pixels[i + 0]);
    image.pixels[i + 3] = char(pixels[i + 3]);
  }

  if (FreeImage_IsTransparent(dib)) {
    image.flags |= ImageData::ALPHA_BIT;
  }

  FreeImage_Unload(dib);
  return image;
}

bool ImageBuilder::createDDS(const ImageData* faces, int nFaces, const File& destFile)
{
  errorBuffer[0] = '\0';

  if (nFaces < 1) {
    snprintf(errorBuffer, ERROR_LENGTH, "At least one face must be given.");
    return false;
  }

  return buildDDS(faces, nFaces, destFile);
}

bool ImageBuilder::convertToDDS(const File& file, const File& destPath)
{
  errorBuffer[0] = '\0';

  if (file.hasExtension("dds")) {
    return file.copyTo(destPath);
  }

  File destFile = destPath;
  if (destFile.isDirectory()) {
    destFile = String::format("%s/%s.dds", destPath.c(), file.baseName().c());
  }

  ImageData image = loadImage(file);
  if (image.isEmpty()) {
    return false;
  }
  return buildDDS(&image, 1, destFile);
}

void ImageBuilder::init()
{
  FreeImage_Initialise();
  FreeImage_SetOutputMessage(printError);
}

void ImageBuilder::destroy()
{
  FreeImage_DeInitialise();
}

}
