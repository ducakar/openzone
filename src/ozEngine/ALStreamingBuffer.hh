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
 * @file ozEngine/ALStreamingBuffer.hh
 */

#pragma once

#include "ALSource.hh"

namespace oz
{

/**
 * Wrapper for a queue of two OpenAL buffers for streaming an Ogg Vorbis file.
 */
class ALStreamingBuffer
{
  private:

    struct Stream;

    uint    bufferIds[2]; ///< OpenAL buffer ids, both 0 if not loaded.
    uint    sourceId;     ///< Target OpenAL source for which buffers are queued.
    Stream* stream;       ///< Internal file buffer and decoder state.

  public:

    /**
     * Create an empty instance (no OpenAL buffers are created).
     */
    explicit ALStreamingBuffer();

    /**
     * Create a new buffer from the given file. Same as the default constructor plus `load()`.
     */
    explicit ALStreamingBuffer( const File& file );

    /**
     * Destructor, invokes `destroy()`.
     */
    ~ALStreamingBuffer();

    /**
     * Move constructor.
     */
    ALStreamingBuffer( ALStreamingBuffer&& b )
    {
      bufferIds[0]   = b.bufferIds[0];
      bufferIds[1]   = b.bufferIds[1];
      sourceId       = b.sourceId;
      stream         = b.stream;

      b.bufferIds[0] = 0;
      b.bufferIds[1] = 0;
      b.sourceId     = 0;
      b.stream       = nullptr;
    }

    /**
     * Move operator.
     */
    ALStreamingBuffer& operator = ( ALStreamingBuffer&& b )
    {
      if( &b == this ) {
        return *this;
      }

      bufferIds[0]   = b.bufferIds[0];
      bufferIds[1]   = b.bufferIds[1];
      sourceId       = b.sourceId;
      stream         = b.stream;

      b.bufferIds[0] = 0;
      b.bufferIds[1] = 0;
      b.sourceId     = 0;
      stream         = nullptr;

      return *this;
    }

    /**
     * Get OpenAL buffer id.
     */
    uint id( int i ) const
    {
      hard_assert( 0 <= i && i < 2 );

      return bufferIds[i];
    }

    /**
     * Get attached OpenAL source id.
     */
    uint attachedSourceId() const
    {
      return sourceId;
    }

    /**
     * True iff loaded.
     */
    bool isLoaded() const
    {
      return bufferIds[0] != 0;
    }

    /**
     * True iff a source is attached.
     */
    bool isSourceAttached() const
    {
      return sourceId != 0;
    }

    /**
     * Update buffers and queue for the attached source.
     *
     * If a buffer has been processed, unqueues it, fills it with new data and puts it back into
     * the queue. This is a NOP if no source is attached.
     */
    bool update();

    /**
     * Create a new source and attach it to this buffer queue.
     *
     * The buffers must be created to attach a source. If there already is a source attached it is
     * forgotten.
     */
    ALSource createSource();

    /**
     * Create a pair of uninitialised OpenAL buffers for streaming.
     */
    bool create();

    /**
     * Create a new buffer queue from the given Ogg Vorbis file.
     *
     * If the source is already attached, it is stopped and newly filled buffers are queued for it.
     */
    bool load( const File& file );

    /**
     * Destroy OpenAL buffers if created and forget the attached source.
     */
    void destroy();

};

}
