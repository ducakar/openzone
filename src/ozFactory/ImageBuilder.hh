/*
 * ozFactory - OpenZone Assets Builder Library.
 *
 * Copyright © 2002-2016 Davorin Učakar
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

#include "ImageData.hh"

namespace oz
{

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
  static constexpr int CUBE_MAP_BIT = 0x01;

  /// Image is a normal map (set DDPF_NORMAL bit).
  static constexpr int NORMAL_MAP_BIT = 0x02;

  /// Generate mipmaps.
  static constexpr int MIPMAPS_BIT = 0x04;

  /// Enable texture compression (DXT1 or DXT5, depending on presence of alpha channel).
  static constexpr int COMPRESSION_BIT = 0x08;

  /// Flip vertically.
  static constexpr int FLIP_BIT = 0x10;

  /// Flip horizontally.
  static constexpr int FLOP_BIT = 0x20;

  /// Perform RGB(A) -> GGGR swizzle (for DXT5nm normal map compression).
  static constexpr int YYYX_BIT = 0x40;

  /// Perform RGB(A) -> BGBR swizzle (for DXT5nm+z normal map compression).
  static constexpr int ZYZX_BIT = 0x80;

  /// Use fastest but low-quality filters.
  static constexpr int FAST_BIT = 0x100;

  static int   options; ///< Bitfield to control compression, mipmap generation etc.
  static float scale;   ///< Scale to resize images.

public:

  /**
   * Static class.
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
  static bool convertToDDS(const File& file, const File& destPath);

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
