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
 *
 * `ALBuffer` class.
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

    ALuint bufferId; ///< OpenAL buffer id, 0 if not loaded.

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
    ALuint id() const
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
     *
     * If the buffer does not exist, this returns an uninitialised `ALSource`.
     */
    ALSource createSource() const;

    /**
     * Create a new uninitialised OpenAL buffer.
     *
     * This is a NOP if the buffer already exists.
     */
    bool create();

    /**
     * Create a new buffer if necessary and load data from the given WAVE or Ogg Vorbis file.
     *
     * If the buffer already exists, its contents is replaced by the new data. On loading failure,
     * buffer is destroyed.
     *
     * @note
     * OpenAL error is generated if a buffer contents is changed while some source plays it.
     */
    bool load( const File& file );

    /**
     * Destroy OpenAL buffer if created.
     *
     * @note
     * OpenAL error is generated if a buffer is deleted while some source is still bound to it.
     */
    void destroy();

};

}
