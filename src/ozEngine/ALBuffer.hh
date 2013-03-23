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
 * @file ozEngine/ALBuffer.hh
 */

#pragma once

#include "ALSource.hh"

namespace oz
{

/**
 * OpenAL buffer wrapper.
 */
class ALBuffer
{
  private:

    uint bufferId; ///< OpenAL buffer id, 0 if not loaded.

  public:

    /**
     * Create an empty instance (no OpenAL buffer is created).
     */
    explicit ALBuffer();

    /**
     * Create a new buffer from the given file. Same as the default constructor plus `load()`.
     */
    explicit ALBuffer( const File& file );

    /**
     * Destructor, destroys OpenAL buffer if created.
     */
    ~ALBuffer();

    /**
     * Move constructor.
     */
    ALBuffer( ALBuffer&& b ) :
      bufferId( b.bufferId )
    {
      b.bufferId = 0;
    }

    /**
     * Move operator.
     */
    ALBuffer& operator = ( ALBuffer&& b )
    {
      if( &b == this ) {
        return *this;
      }

      bufferId   = b.bufferId;
      b.bufferId = 0;

      return *this;
    }

    /**
     * Get OpenAL buffer id.
     */
    uint id() const
    {
      return bufferId;
    }

    /**
     * True iff loaded.
     */
    bool isLoaded() const
    {
      return bufferId != 0;
    }

    /**
     * Create a new OpenAL source for this buffer.
     */
    ALSource createSource() const;

    /**
     * Create a new uninitialised OpenAL buffer.
     */
    bool create();

    /**
     * Create a new OpenAL buffer from the given WAVE or Ogg Vorbis file.
     */
    bool load( const File& file );

    /**
     * Destroy OpenAL buffer if created.
     */
    void destroy();

};

}
