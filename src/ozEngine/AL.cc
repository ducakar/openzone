/*
 * ozEngine - OpenZone Engine Library.
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

#include "AL.hh"

// We don't use those callbacks anywhere and they don't compile on MinGW.
#define OV_EXCLUDE_STATIC_CALLBACKS

#include <AL/alext.h>
#include <cstring>
# ifdef OZ_OPUS
#include <opus/opusfile.h>
# endif
#include <vorbis/vorbisfile.h>

namespace oz
{

/*
 * Vorbis stream reader callbacks.
 */

static size_t vorbisRead(void* buffer, size_t size, size_t n, void* handle)
{
  Stream* is = static_cast<Stream*>(handle);

  int blockSize = int(size);
  int nBlocks   = min<int>(int(n), is->available() / blockSize);

  is->read(static_cast<char*>(buffer), nBlocks * blockSize);
  return nBlocks;
}

static int vorbisSeek(void* handle, ogg_int64_t offset, int whence)
{
  Stream* is = static_cast<Stream*>(handle);

  int origin = whence == SEEK_CUR ? is->tell() : whence == SEEK_END ? is->capacity() : 0;

  is->seek(origin + int(offset));
  return 0;
}

static long vorbisTell(void* handle)
{
  Stream* is = static_cast<Stream*>(handle);

  return is->tell();
}

static ov_callbacks VORBIS_CALLBACKS = {vorbisRead, vorbisSeek, nullptr, vorbisTell};

/*
 * Class members.
 */

struct AL::Decoder::StreamBase
{
  Stream is;

  virtual ~StreamBase();
  virtual bool decode(AL::Decoder* decoder) = 0;
};

OZ_INTERNAL
AL::Decoder::StreamBase::~StreamBase()
{}

struct AL::Decoder::WaveStream : AL::Decoder::StreamBase
{
  int sampleSize;

  OZ_INTERNAL
  explicit WaveStream(AL::Decoder* decoder, const File& file)
  {
    is = file.read(Endian::LITTLE);

    if (is.available() < 44) {
      OZ_ERROR("oz::AL::Decoder: Failed to open WAVE file '%s'", file.c());
    }

    is.seek(22);

    int nChannels = is.readShort();
    int rate      = is.readInt();

    is.seek(34);

    sampleSize = is.readShort() / 8;

    while (is.available() != 0 && !String::beginsWith(is.readSkip(4), "data")) {
      is.readSkip(is.readInt());
    }

    int size = is.readInt();

    if ((nChannels != 1 && nChannels != 2) || (sampleSize != 1 && sampleSize != 2)) {
      OZ_ERROR("oz::AL::Decoder: Only mono and stereo, 8-bit and 16-bit WAVE PCM supported '%s'",
               file.c());
    }

    decoder->format   = nChannels == 2 ? AL_FORMAT_STEREO_FLOAT32 : AL_FORMAT_MONO_FLOAT32;
    decoder->rate     = rate;
    decoder->capacity = size / sampleSize;
    decoder->samples  = new float[decoder->capacity];
  }

  bool decode(AL::Decoder* decoder) override;
};

OZ_INTERNAL
bool AL::Decoder::WaveStream::decode(AL::Decoder* decoder)
{
  if (decoder->count != 0) {
    return false;
  }

  float* begin      = decoder->samples;
  float* end        = decoder->samples + decoder->capacity;
  float* alignedEnd = begin + (end - begin & ~3);

  if (sampleSize == 1) {
    const ubyte* samples = reinterpret_cast<const ubyte*>(is.pos());

    while (begin != alignedEnd) {
      begin[0] = float(samples[0] - 128) / 128.0f;
      begin[1] = float(samples[1] - 128) / 128.0f;
      begin[2] = float(samples[2] - 128) / 128.0f;
      begin[3] = float(samples[3] - 128) / 128.0f;

      begin   += 4;
      samples += 4;
    }
    while (begin != end) {
      *begin++ = float(*samples++ - 128) / 128.0f;
    }
  }
  else {
    const short* samples = reinterpret_cast<const short*>(is.pos());

    while (begin != alignedEnd) {
#if OZ_BYTE_ORDER == 4321
      begin[0] = float(Endian::bswap(samples[0])) / 32768.0f;
      begin[1] = float(Endian::bswap(samples[1])) / 32768.0f;
      begin[2] = float(Endian::bswap(samples[2])) / 32768.0f;
      begin[3] = float(Endian::bswap(samples[3])) / 32768.0f;
#else
      begin[0] = float(samples[0]) / 32768.0f;
      begin[1] = float(samples[1]) / 32768.0f;
      begin[2] = float(samples[2]) / 32768.0f;
      begin[3] = float(samples[3]) / 32768.0f;
#endif

      begin   += 4;
      samples += 4;
    }
    while (begin != end) {
#if OZ_BYTE_ORDER == 4321
      *begin++ = float(Endian::bswap(*samples++)) / 32768.0f;
#else
      *begin++ = float(*samples++) / 32768.0f;
#endif
    }
  }

  decoder->count = decoder->capacity;
  return true;
}

#ifdef OZ_OPUS

struct AL::Decoder::OpusStream : AL::Decoder::StreamBase
{
  static const int FRAME_SIZE = 120 * 48;

  OggOpusFile* opFile = nullptr;

  OZ_INTERNAL
  explicit OpusStream(AL::Decoder* decoder, const File& file, bool isStreaming)
  {
    is = file.read();
    if (is.available() == 0) {
      OZ_ERROR("oz::AL::Decoder: Failed to open Opus file '%s'", file.c());
    }

    int error;
    opFile = op_open_memory(reinterpret_cast<const ubyte*>(is.begin()), is.available(), &error);
    if (error != 0) {
      OZ_ERROR("oz::AL::Decoder: Invalid Opus file '%s'", file.c());
    }

    int nChannels = op_channel_count(opFile, -1);
    int nSamples  = int(op_pcm_total(opFile, -1));

    if (nChannels != 1 && nChannels != 2) {
      OZ_ERROR("oz::AL::Decoder: Only mono and stereo Opus supported '%s'", file.c());
    }

    decoder->format   = nChannels == 2 ? AL_FORMAT_STEREO_FLOAT32 : AL_FORMAT_MONO_FLOAT32;
    decoder->rate     = 48000;
    decoder->capacity = isStreaming ? FRAME_SIZE * nChannels : nSamples * nChannels;
    decoder->samples  = new float[decoder->capacity];
  }

  ~OpusStream() override;

  bool decode(AL::Decoder* decoder) override;
};

OZ_INTERNAL
const int AL::Decoder::OpusStream::FRAME_SIZE;

OZ_INTERNAL
AL::Decoder::OpusStream::~OpusStream()
{
  if (opFile != nullptr) {
    op_free(opFile);
  }
}

OZ_INTERNAL
bool AL::Decoder::OpusStream::decode(AL::Decoder* decoder)
{
  decoder->count = 0;

  do {
    int result;

    if (decoder->format == AL_FORMAT_STEREO_FLOAT32) {
      result = op_read_float_stereo(opFile, decoder->samples + decoder->count,
                                    decoder->capacity - decoder->count) * 2;
    }
    else {
      result = op_read_float(opFile, decoder->samples + decoder->count,
                             decoder->capacity - decoder->count, nullptr);
    }

    if (result <= 0) {
      return decoder->count != 0;
    }

    decoder->count += result;
  }
  while (decoder->count != decoder->capacity);

  return true;
}

#endif

struct AL::Decoder::VorbisStream : AL::Decoder::StreamBase
{
  OggVorbis_File ovFile;

  OZ_INTERNAL
  explicit VorbisStream(AL::Decoder* decoder, const File& file, bool isStreaming)
  {
    is = file.read();
    if (is.available() == 0) {
      OZ_ERROR("oz::AL::Decoder: Failed to open Vorbis file '%s'", file.c());
    }

    int error = ov_open_callbacks(&is, &ovFile, nullptr, 0, VORBIS_CALLBACKS);
    if (error != 0) {
      OZ_ERROR("oz::AL::Decoder: Invalid Vorbis file '%s'", file.c());
    }

    vorbis_info* ovInfo = ov_info(&ovFile, -1);

    int nChannels = ovInfo->channels;
    int rate      = int(ovInfo->rate);
    int frameSize = 120 * rate / 1000;
    int nSamples  = int(ov_pcm_total(&ovFile, -1));

    if (nChannels != 1 && nChannels != 2) {
      OZ_ERROR("oz::AL::Decoder: Only mono and stereo Vorbis supported '%s'", file.c());
    }

    decoder->format   = nChannels == 2 ? AL_FORMAT_STEREO_FLOAT32 : AL_FORMAT_MONO_FLOAT32;
    decoder->rate     = rate;
    decoder->capacity = isStreaming ? frameSize * nChannels : nSamples * nChannels;
    decoder->samples  = new float[decoder->capacity];
  }

  ~VorbisStream() override;

  bool decode(AL::Decoder* decoder) override;
};

OZ_INTERNAL
AL::Decoder::VorbisStream::~VorbisStream()
{
  ov_clear(&ovFile);
}

OZ_INTERNAL
bool AL::Decoder::VorbisStream::decode(AL::Decoder* decoder)
{
  int stereo = decoder->format == AL_FORMAT_STEREO_FLOAT32;

  decoder->count = 0;

  do {
    float** samples;
    long    result;
    int     section;

    result = ov_read_float(&ovFile, &samples, (decoder->capacity - decoder->count) >> stereo,
                           &section);

    if (result <= 0) {
      return decoder->count != 0;
    }

    float* output = &decoder->samples[decoder->count];

    if (stereo != 0) {
      for (long i = 0; i < result; ++i) {
        *output++ = samples[0][i];
        *output++ = samples[1][i];
      }
      result *= 2;
    }
    else {
      memcpy(output, samples[0], result * sizeof(float));
    }

    decoder->count += result;
  }
  while (decoder->count != decoder->capacity);

  return true;
}

AL::Decoder::Decoder() :
  samples(nullptr), count(0), capacity(0), format(0), rate(0), stream(nullptr)
{}

AL::Decoder::Decoder(const File& file, bool isStreaming) :
  samples(nullptr), count(0), capacity(0), format(0), rate(0), stream(nullptr)
{
  if (file.hasExtension("wav")) {
    stream = new WaveStream(this, file);
  }
#ifdef OZ_OPUS
  else if (file.hasExtension("opus")) {
    stream = new OpusStream(this, file, isStreaming);
  }
#endif
  else if (file.hasExtension("oga") || file.hasExtension("ogg")) {
    stream = new VorbisStream(this, file, isStreaming);
  }
  else {
    OZ_ERROR("oz::AL::Decoder: Unknown file type '%s'", file.c());
  }
}

AL::Decoder::~Decoder()
{
  delete[] samples;
  delete stream;
}

AL::Decoder::Decoder(Decoder&& d) :
  samples(d.samples), count(d.count), capacity(d.capacity), format(d.format), rate(d.rate),
  stream(d.stream)
{
  d.samples  = nullptr;
  d.count    = 0;
  d.capacity = 0;
  d.format   = 0;
  d.rate     = 0;
  d.stream   = nullptr;
}

AL::Decoder& AL::Decoder::operator = (AL::Decoder&& d)
{
  if (&d != this) {
    delete[] samples;
    delete stream;

    samples  = d.samples;
    count    = d.count;
    capacity = d.capacity;
    format   = d.format;
    rate     = d.rate;
    stream   = d.stream;

    d.samples  = nullptr;
    d.count    = 0;
    d.capacity = 0;
    d.format   = 0;
    d.rate     = 0;
    d.stream   = nullptr;
  }
  return *this;
}

bool AL::Decoder::decode()
{
  if (stream != nullptr && !stream->decode(this)) {
    delete stream;
    stream = nullptr;
  }
  return stream != nullptr;
}

void AL::Decoder::load(ALuint buffer) const
{
  alBufferData(buffer, format, samples, count * sizeof(float), rate);
}

void AL::checkError(const char* function, const char* file, int line)
{
  const char* message;
  ALenum result = alGetError();

  switch (result) {
    case AL_NO_ERROR: {
      return;
    }
    case AL_INVALID_NAME: {
      message = "AL_INVALID_NAME";
      break;
    }
    case AL_INVALID_ENUM: {
      message = "AL_INVALID_ENUM";
      break;
    }
    case AL_INVALID_VALUE: {
      message = "AL_INVALID_VALUE";
      break;
    }
    case AL_INVALID_OPERATION: {
      message = "AL_INVALID_OPERATION";
      break;
    }
    case AL_OUT_OF_MEMORY: {
      message = "AL_OUT_OF_MEMORY";
      break;
    }
    default: {
      message = String::format("UNKNOWN(%d)", int(result));
      break;
    }
  }

  System::error(function, file, line, 1, "AL error '%s'", message);
}

bool AL::bufferDataFromFile(ALuint bufferId, const File& file)
{
  Decoder decoder(file, false);

  if (decoder.decode()) {
    decoder.load(bufferId);
  }

  OZ_AL_CHECK_ERROR();
  return true;
}

}
