/*
 * ozEngine - OpenZone Engine Library.
 *
 * Copyright © 2002-2019 Davorin Učakar
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
 * OpenAL utilities.
 */
class AL
{
public:

  static constexpr ALenum FORMAT_MONO_FLOAT32   = 0x10010;
  static constexpr ALenum FORMAT_STEREO_FLOAT32 = 0x10011;

  /**
   * Buffer with decoded audio data.
   */
  struct Decoder
  {
  private:

    class StreamBase;
    class WaveStream;
    class OpusStream;
    class VorbisStream;

    float*      samples_  = nullptr; ///< Samples buffer.
    int         size_     = 0;       ///< Sum of all samples per all channels written in buffer.
    int         capacity_ = 0;       ///< Buffer capacity (total sum per all channels).
    ALenum      format_   = 0;       ///< AL_FORMAT_MONO_FLOAT32 or AL_FORMAT_STEREO_FLOAT32.
    int         rate_     = 0;       ///< Sampling rate.
    StreamBase* stream_   = nullptr; ///< Internal structures used for stream decoding.

  public:

    /**
     * Create empty instance.
     */
    Decoder() = default;

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
     * No copying.
     */
    Decoder(const Decoder&) = delete;

    /**
     * Move constructor.
     */
    Decoder(Decoder&& other) noexcept;

    /**
     * No copying.
     */
    Decoder& operator=(const Decoder&) = delete;

    /**
     * Move operator.
     */
    Decoder& operator=(Decoder&& other) noexcept;

    /**
     * True iff a stream is opened.
     */
    OZ_ALWAYS_INLINE
    bool isValid() const noexcept
    {
      return stream_ != nullptr;
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
   * Load OpenAL buffer from a WAVE or Ogg Vorbis file.
   */
  static bool bufferDataFromFile(ALuint bufferId, const File& file);

};

}
