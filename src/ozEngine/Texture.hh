/*
 * ozEngine - OpenZone Engine Library.
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
 * @file ozEngine/Texture.hh
 */

#pragma once

#include "common.hh"

namespace oz
{

class Texture
{
  public:

    /**
     * Flag bit telling the texture is uploaded to the graphic card.
     */
    static const int UPLOADED_BIT  = 0x01;

    /**
     * Build option bit enabling highest quality scaling and compression.
     */
    static const int QUALITY_BIT = 0x01;

    /**
     * Build option bit enabling texture compression (must be compiled with libsquish).
     */
    static const int COMPRESSION_BIT = 0x02;

  public:

    uint id;    ///< OpenGL texture id.
    int  flags; ///< Texture flags.

  public:

    /**
     * Load texture components from files and convert it to OpenZone format.
     *
     * Texture is loaded from different files representing texture diffuse colour (none or "_d"
     * suffix), normal map ("_n", "_nm", "_normal" or "_local" suffix), specular ("_spec" suffix or
     * red component in "_m" suffixed image) and emission masks (".blend" suffix or green component
     * in "_m" suffixed image. png, jpg, jpeg, tga and bmp file extensions are recognised (must be
     * lower-case).
     *
     * Quality of texture compression and texture scaling can be controlled via `options` parameter.
     * `QUALITY_BIT` enables highest quality but slow methods for scaling texture to smaller
     * dimensions used for mipmaps and for texture compression if enabled. `COMPRESSION_BIT` enables
     * S3TC texture compression, DXT1 and DXT5 for textures without and with alpha channel
     * respectively.
     *
     * @param fs file system type.
     * @param path path to texture files, without suffixes and extensions.
     * @param options bit-mask to control quality and compression.
     * @param ostream stream to write texture to.
     */
    static bool build( File::FileSystem fs, const char* path, int options, OutputStream* ostream );

    /**
     * Create new empty instance.
     */
    explicit Texture();

    /**
     * Destructor, unloads texture from graphics card if loaded.
     */
    ~Texture();

    /**
     * Upload texture in OpenZone format to graphics card.
     */
    bool upload( InputStream* istream );

    /**
     * Unload texture from graphics card if uploaded.
     */
    void destroy();

};

}
