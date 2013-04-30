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
 * @file ozEngine/GLTexture.hh
 *
 * `GLTexture` class.
 */

#pragma once

#include "GL.hh"

namespace oz
{

/**
 * OpenGL texture wrapper.
 */
class GLTexture
{
  private:

    GLuint textureId;      ///< OpenGL texture id.
    int    textureMipmaps; ///< True iff mipmaps have been are loaded from file.

  public:

    /**
     * Create an empty instance (no OpenGL texture is created).
     */
    explicit GLTexture();

    /**
     * Load a DDS texture from a file.
     */
    explicit GLTexture( const File& file );

    /**
     * Destructor, unloads texture from GPU if loaded.
     */
    ~GLTexture();

    /**
     * Move constructor.
     */
    GLTexture( GLTexture&& t ) :
      textureId( t.textureId ), textureMipmaps( t.textureMipmaps )
    {
      t.textureId      = 0;
      t.textureMipmaps = 0;
    }

    /**
     * Move operator.
     */
    GLTexture& operator = ( GLTexture&& t )
    {
      if( &t == this ) {
        return *this;
      }

      textureId        = t.textureId;
      textureMipmaps   = t.textureMipmaps;

      t.textureId      = 0;
      t.textureMipmaps = 0;

      return *this;
    }

    /**
     * OpenGL texture id.
     */
    GLuint id() const
    {
      return textureId;
    }

    /**
     * True iff loaded.
     */
    bool isLoaded() const
    {
      return textureId != 0;
    }

    /**
     * True iff mipmaps have been loaded.
     */
    bool hasMipmaps() const
    {
      return textureMipmaps;
    }

    /**
     * Create a new uninitialised OpenGL texture.
     *
     * This is a NOP if the texture already exists.
     */
    bool create();

    /**
     * Create a new texture and load its data from a DDS file.
     *
     * If the texture already exists, it is destroyed and re-created. On loading failure, texture is
     * deleted.
     */
    bool load( const File& file );

    /**
     * Unload texture from GPU if loaded.
     */
    void destroy();

};

}
