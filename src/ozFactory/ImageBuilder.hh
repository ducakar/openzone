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
 * @file ozFactory/ImageBuilder.hh
 *
 * `ImageBuilder` class.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * %Image pixel data with basic metadata (dimensions and transparency).
 */
struct ImageData
{
  /// Alpha flag.
  static const int ALPHA_BIT = 0x01;

  int   width;  ///< Width.
  int   height; ///< Height.
  int   flags;  ///< Flags.
  char* pixels; ///< Pixels data in RGBA format.

  /**
   * Create empty instance, no allocation is performed.
   */
  ImageData();

  /**
   * Create an image an allocate memory for pixel data.
   */
  explicit ImageData(int width, int height);

  /**
   * Destructor.
   */
  ~ImageData();

  /**
   * Move constructor, moves pixel data.
   */
  ImageData(ImageData&& i);

  /**
   * Move operator, moves pixel data.
   */
  ImageData& operator = (ImageData&& i);

  /**
   * True iff it holds no image data.
   */
  OZ_ALWAYS_INLINE
  bool isEmpty() const
  {
    return pixels == nullptr;
  }

  /**
   * Check if any non-opaque pixel is present and update alpha flag accordingly.
   */
  void determineAlpha();

  /**
   * Guess if the image is a normal map.
   *
   * The guess is based on average colour being close to #8080ff and whether pixel lengths roughly
   * one (\f$ (R - 0.5)^2 + (G - 0.5)^2 + (B - 0.5)^2 = 1 \f$).
   */
  bool isNormalMap() const;
};

/**
 * %ImageBuilder class converts generic image formats to DDS (DirectDraw Surface).
 *
 * FreeImage library is used to read source images and apply transformations to them (e.g. resizing
 * for mipmaps) and libsquish to apply S3 texture compression.
 */
class ImageBuilder
{
public:

  /// Image array is a cube map.
  static const int CUBE_MAP_BIT = 0x01;

  /// Image is a normal map (set DDPF_NORMAL bit).
  static const int NORMAL_MAP_BIT = 0x02;

  /// Generate mipmaps.
  static const int MIPMAPS_BIT = 0x04;

  /// Enable texture compression (DXT1 or DXT5, depending on presence of alpha channel).
  static const int COMPRESSION_BIT = 0x08;

  /// Flip vertically.
  static const int FLIP_BIT = 0x10;

  /// Flip horizontally.
  static const int FLOP_BIT = 0x20;

  /// Perform RGB(A) -> GGGR swizzle (for DXT5nm normal map compression).
  static const int YYYX_BIT = 0x40;

  /// Perform RGB(A) -> BGBR swizzle (for DXT5nm+z normal map compression).
  static const int ZYZX_BIT = 0x80;

  /// Use fastest but low-quality filters.
  static const int FAST_BIT = 0x100;

  static int   options; ///< Bitfield to control compression, mipmap generation etc.
  static float scale;   ///< Scale to resize images.

public:

  /**
   * Forbid instances.
   */
  ImageBuilder() = delete;

  /**
   * Get the last error string from `buildDDS()`.
   */
  static const char* getError();

  /**
   * True iff the given file contains an image according to FreeImage.
   */
  static bool isImage(const File& file);

  /**
   * Load an image.
   */
  static ImageData loadImage(const File& file);

  /**
   * Generate a DDS form a given image and optionally compress it and create mipmaps.
   *
   * An array texture is created if more than one image face is given. If an array of exactly 6
   * faces is given and `CUBE_MAP_BIT` option is set a cube map is generated. Cube map faces must
   * be given in the following order: +x, -x, +y, -y, +z, -z.
   *
   * @note
   * The highest possible quality settings are used for compression and mipmap scaling, so this
   * might take a long time for a large image.
   *
   * @param faces array of input images.
   * @param nFaces number of input images.
   * @param destFile output file.
   */
  static bool createDDS(const ImageData* faces, int nFaces, const File& destFile);

  /**
   * Convert a given image to DDS format, similar to `buildDDS()`.
   *
   * Freeimage library is used for reading a file, so most of image file formats are supported.
   * If the input file is a valid DDS, it is only copied. Arrays and cube map textures cannot be
   * built by this function.
   *
   * @param file input image file.
   * @param destPath output file or directory (in the latter case output file has the same base name
   * as the input one but "dds" extension).
   *
   * @sa `buildDDS()`
   */
  static bool convertToDDS(const File& file, const char* destPath);

  /**
   * Initialise underlaying FreeImage library.
   *
   * This function should be called before `ImageBuilder` class is used.
   */
  static void init();

  /**
   * Deinitialise underlaying FreeImage library.
   *
   * This function should be called after you finish using `ImageBuilder`.
   */
  static void destroy();

};

}
