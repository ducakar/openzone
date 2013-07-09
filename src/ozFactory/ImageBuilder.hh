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
 * @file ozFactory/ImageBuilder.hh
 *
 * `ImageBuilder` class.
 */

#pragma once

#include "common.hh"

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

    /**
     * Enable generation of mipmaps for a texture.
     */
    static const int MIPMAPS_BIT = 0x01;

    /**
     * Enable texture compression.
     */
    static const int COMPRESSION_BIT = 0x02;

  public:

    /**
     * Forbid instances.
     */
    explicit ImageBuilder() = delete;

    /**
     * True iff the given file contains an image according to FreeImage.
     */
    static bool isImage( const File& file );

    /**
     * Get the last error string from `buildDDS()`.
     */
    static const char* getError();

    /**
     * Convert given image to DDS format and optionally compress it and create mipmaps.
     *
     * Mipmap generation and S3 texture compression can be controlled via `options` parameter.
     * - `MIPMAPS_BIT` enables generation of mipmaps.
     * - `COMPRESSION_BIT` enables S3 texture compression; DXT1 is used for images without an
     *   alpha channel and DXT5 for images with an alpha channel. Texture compression is enabled
     *   only if `OZ_NONFREE` is enabled on ozEngine build.
     *
     * Freeimage library is used for reading a file, so most of image file formats are supported.
     *
     * @note
     * The highest possible quality settings are used for compression and mipmap scaling, so this
     * might take a long time for a large image.
     *
     * @param file image file.
     * @param options bit-mask to control mipmap generation and compression.
     * @param destPath output file or directory (in the latter case output file has the same base
     *        name as input one but "dds" extension).
     */
    static bool buildDDS( const File& file, int options, const char* destPath );

};

}
