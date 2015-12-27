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
 * @file ozEngine/AL.hh
 *
 * `AL` class and wrapper for OpenAL headers.
 */

#pragma once

#include "common.hh"

#include <AL/al.h>

/**
 * @def OZ_AL_CHECK_ERROR
 * In debug mode, check for OpenAL errors and crash with some diagnostics if there is one.
 */
#ifdef NDEBUG
# define OZ_AL_CHECK_ERROR() void(0)
#else
# define OZ_AL_CHECK_ERROR() oz::AL::checkError(__PRETTY_FUNCTION__, __FILE__, __LINE__)
#endif

namespace oz
{

/**
 * Buffer with decoded audio data.
 */
struct AudioBuffer
{
  List<float> data;   ///< Samples.
  ALenum      format; ///< OpenAL format.
  int         rate;   ///< Sampling rate.
};

/**
 * OpenAL utilities.
 */
class AL
{
public:

  /**
   * Buffer with decoded audio data.
   */
  struct Decoder
  {
  private:

    struct StreamBase;
    struct WaveStream;
    struct OpusStream;
    struct VorbisStream;
    struct MP3Stream;
    struct AACStream;

    List<float> samples;  ///< Samples buffer.
    int         nSamples; ///< Number of contained samples.
    ALenum      format;   ///< OpenAL format (AL_FORMAT_MONO_FLOAT32 or AL_FORMAT_STEREO_FLOAT32).
    int         rate;     ///< Sampling rate.
    StreamBase* stream;   ///< Internal structures used for stream decoding.

  public:

    /**
     * Create empty instance.
     */
    Decoder();

    /**
     * Initialise an instance and open a file for decoding.
     *
     * @param file WAVE, Ogg Vorbis or Ogg Opus file.
     * @param isStreaming whether to decode incrementally or whole file in the first step.
     */
    Decoder(const File& file, bool isStreaming = false);

    /**
     * Destructor.
     */
    ~Decoder();

    /**
     * Move constructor.
     */
    Decoder(Decoder&& d);

    /**
     * Move operator.
     */
    Decoder& operator = (Decoder&& d);

    /**
     * True iff a stream is opened.
     */
    OZ_ALWAYS_INLINE
    bool isValid() const
    {
      return stream != nullptr;
    }

    /**
     * Decode next chunk of data into an internal buffer.
     *
     * The stream is automatically closed on an error or EOF.
     *
     * @return true iff the stream is still opened.
     */
    bool decode();

    /**
     * Copy previously decoded data to an OpenAL buffer.
     */
    void load(ALuint buffer) const;

  };

  /**
   * %Streamer for Ogg Vorbis files.
   */
  class Streamer
  {
  private:

    struct Data;

    ALuint source; ///< Target OpenAL source for which buffers are queued, 0 if none.
    Data*  data;   ///< Internal buffers, Ogg Vorbis stream state, format description ...

  public:

    /**
     * Create an empty instance (no internal OpenAL buffers are created).
     */
    Streamer();

    /**
     * Destructor.
     */
    ~Streamer();

    /**
     * Move constructor.
     */
    Streamer(Streamer&& s);

    /**
     * Move operator.
     */
    Streamer& operator = (Streamer&& s);

    /**
     * True iff streaming.
     */
    bool isStreaming() const
    {
      return data != nullptr;
    }

    /**
     * True iff a source is attached.
     */
    bool hasSource() const
    {
      return source != 0;
    }

    /**
     * Set OpenAL source which buffers with streamed data will be queued for.
     */
    void attach(ALuint source);

    /**
     * Unset OpenAL source for buffer queuing.
     */
    void detach();

    /**
     * Start streaming a given Ogg Vorbis file.
     */
    bool open(const File& file);

    /**
     * Stop streaming and free file buffers and stream state.
     */
    void close();

    /**
     * Stop attached source and rewind the stream to the beginning.
     */
    bool rewind();

    /**
     * Update processed buffers in the queue, decode new data into them.
     *
     * When the end of the stream is reached it invokes `close()`.
     */
    bool update();

    /**
     * Deinitialise, detach source and delete all buffers.
     */
    void destroy();

  };

public:

  /**
   * Static class.
   */
  AL() = delete;

  /**
   * Helper function for `OZ_AL_CHECK_ERROR` macro.
   */
  static void checkError(const char* function, const char* file, int line);

  /**
   * Read WAVE or Ogg Vorbis file stream and decode data into a buffer.
   */
  static AudioBuffer decodeFromStream(Stream* is);

  /**
   * Read WAVE or Ogg Vorbis file and decode data into a buffer.
   */
  static AudioBuffer decodeFromFile(const File& file);

  /**
   * Load OpenAL buffer from a WAVE or Ogg Vorbis file stream.
   */
  static bool bufferDataFromStream(ALuint bufferId, Stream* is);

  /**
   * Load OpenAL buffer from a WAVE or Ogg Vorbis file.
   */
  static bool bufferDataFromFile(ALuint bufferId, const File& file);

};

}
