/*
 * ozEngine - OpenZone Engine Library.
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
 * @file ozEngine/ALStreamingBuffer.hh
 *
 * `ALStreamingBuffer` class.
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

  AL::Streamer streamer; ///< Internal streamer.
  ALuint       sourceId; ///< Attached source id, 0 if none.

public:

  /**
   * Create an empty instance (no OpenAL buffers are created).
   */
  explicit ALStreamingBuffer();

  /**
   * Create a new buffer from a given file. Same as the default constructor plus `open()`.
   */
  explicit ALStreamingBuffer( const File& file );

  /**
   * Destructor, invokes `destroy()`.
   */
  ~ALStreamingBuffer();

  /**
   * Move constructor.
   */
  ALStreamingBuffer( ALStreamingBuffer&& b );

  /**
   * Move operator.
   */
  ALStreamingBuffer& operator = ( ALStreamingBuffer&& b );

  /**
   * True iff streaming.
   */
  bool isStreaming() const
  {
    return streamer.isStreaming();
  }

  /**
   * Get attached OpenAL source id.
   */
  ALuint attachedSource() const
  {
    return sourceId;
  }

  /**
   * True iff a source is attached.
   */
  bool isSourceAttached() const
  {
    return sourceId != 0;
  }

  /**
   * Update processed buffers in the queue, decode new data into them.
   *
   * When the end of the stream is reached it invokes `close()`.
   */
  bool update();

  /**
   * Stop source and streaming and rewind to the beginning of the stream.
   */
  bool rewind();

  /**
   * Create a new source and attach it to this buffer queue.
   *
   * The buffers must be created to attach a source. If there already is a source attached it is
   * forgotten.
   */
  ALSource createSource();

  /**
   * Start streaming a given Ogg Vorbis file.
   */
  bool open( const File& file );

  /**
   * Stop streaming and free file buffers and stream state.
   */
  void close();

  /**
   * Deinitialise, detach source and delete all buffers.
   */
  void destroy();

};

}
