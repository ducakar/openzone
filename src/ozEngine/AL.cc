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

#include "AL.hh"

#define OZ_OPUS
// We don't use those callbacks anywhere and they don't compile on MinGW.
#define OV_EXCLUDE_STATIC_CALLBACKS

#include <AL/alext.h>
#include <cstring>
#include <vorbis/vorbisfile.h>
#ifdef OZ_OPUS
# include <opus/opus.h>
#endif

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
 * Vorbis decoder helper function.
 */

static bool decodeVorbis(OggVorbis_File* stream, ALenum format, float* output, int size)
{
  float** samples;
  int     nSamples = size / sizeof(float);
  int     section;
  long    result;

  if (format == AL_FORMAT_STEREO_FLOAT32) {
    nSamples /= 2;
  }

  while ((result = ov_read_float(stream, &samples, nSamples, &section)) > 0) {
    nSamples -= result;

    if (format == AL_FORMAT_STEREO_FLOAT32) {
      for (long i = 0; i < result; ++i) {
        *output++ = samples[0][i];
        *output++ = samples[1][i];
      }
    }
    else {
      memcpy(output, samples[0], result * sizeof(float));
      output += result;
    }
  }
  return result == 0;
}

/*
 * Class members.
 */

struct AL::Decoder::StreamBase
{
  AL::Decoder* decoder;
  Stream       is;

  OZ_INTERNAL
  explicit StreamBase(AL::Decoder* decoder_, Stream&& is_) :
    decoder(decoder_), is(static_cast<Stream&&>(is_))
  {}

  virtual ~StreamBase();

  virtual bool decode() = 0;
};

OZ_INTERNAL
AL::Decoder::StreamBase::~StreamBase()
{}

struct AL::Decoder::WaveStream : AL::Decoder::StreamBase
{
  int sampleSize;

  OZ_INTERNAL
  explicit WaveStream(AL::Decoder* decoder, Stream&& is_, int nSamples) :
    StreamBase(decoder, static_cast<Stream&&>(is_))
  {
    if (is.available() < 44) {
      OZ_ERROR("oz::AL::Decoder: Empty/invalid WAVE file");
    }

    is.seek(22);

    int nChannels = is.readShort();
    decoder->rate = is.readInt();

    is.seek(34);

    sampleSize = is.readShort() / 8;

    while (is.available() != 0 && !String::beginsWith(is.readSkip(4), "data")) {
      is.readSkip(is.readInt());
    }

    int size = is.readInt();

    if ((nChannels != 1 && nChannels != 2) || (sampleSize != 1 && sampleSize != 2)) {
      OZ_ERROR("oz::AL::Decoder: Only mono and stereo, 8-bit and 16-bit WAVE PCM supported");
    }

    OZ_ASSERT(size <= is.available());

    decoder->format   = nChannels == 1 ? AL_FORMAT_MONO_FLOAT32 : AL_FORMAT_STEREO_FLOAT32;
    decoder->nSamples = nSamples == 0 ? size / sampleSize : nSamples;
    decoder->samples  = new float[decoder->nSamples];
  }

  bool decode() override;
};

OZ_INTERNAL
bool AL::Decoder::WaveStream::decode()
{
  int nSamples = min<int>(is.available() / sampleSize, decoder->nSamples);

  if (nSamples == 0) {
    return false;
  }

  if (sampleSize == 1) {
    const ubyte* samples = reinterpret_cast<const ubyte*>(is.readSkip(nSamples));

    for (int i = 0; i < nSamples; ++i) {
      decoder->samples[i] = float(*samples++) - 128 / 127.0f;
    }
  }
  else {
    const short* samples = reinterpret_cast<const short*>(is.readSkip(nSamples * 2));

    for (int i = 0; i < nSamples; ++i) {
      decoder->samples[i] = float(*samples++) / 32767.0f;
    }
  }

  return true;
}

struct AL::Decoder::OpusStream : AL::Decoder::StreamBase
{
  const int OGG_BUFFER_SIZE = 16 * 1024;
  const int FRAME_SIZE      = 960 * 6;

  ogg_sync_state   sync;
  ogg_stream_state stream;
  ogg_page         page;
  ogg_packet       packet;
  OpusDecoder*     opus;

  OZ_INTERNAL
  explicit OpusStream(AL::Decoder* decoder, Stream&& is_, int nSamples) :
    StreamBase(decoder, static_cast<Stream&&>(is_))
  {
    if (is.available() == 0) {
      OZ_ERROR("oz::AL::Decoder: Empty Opus stream");
    }

    ogg_sync_init(&sync);
    ogg_stream_init(&stream, 0);

    int   nBytes = min<int>(OGG_BUFFER_SIZE, is.available());
    char* data   = ogg_sync_buffer(&sync, nBytes);

    is.read(data, nBytes);
    ogg_sync_wrote(&sync, nBytes);

    if (ogg_sync_pageout(&sync, &page) == 0) {
      OZ_ERROR("oz::AL::Decoder: Invalid Opus stream");
    }

    ogg_stream_reset_serialno(&stream, ogg_page_serialno(&page));
    ogg_stream_pagein(&stream, &page);

    if (ogg_stream_packetout(&stream, &packet) != 1) {
      OZ_ERROR("oz::AL::Decoder: Invalid Opus stream");
    }

    if (!packet.b_o_s) {
      OZ_ERROR("oz::AL::Decoder: Invalid Opus header");
    }

    int nChannels = opus_packet_get_nb_channels(packet.packet);
    if (nChannels != 1 && nChannels != 2) {
      OZ_ERROR("oz::AL::Decoder: Only mono and stereo Opus streams supported");
    }

    decoder->format = nChannels == 2 ? AL_FORMAT_STEREO_FLOAT32 : AL_FORMAT_MONO_FLOAT32;
    decoder->rate   = 48000;

    opus = opus_decoder_create(decoder->rate, nChannels, nullptr);

    if (nSamples != 0) {
      decoder->samples = new float[FRAME_SIZE];
      decoder->nSamples = FRAME_SIZE;
    }
  }

  ~OpusStream() override;

  bool decode() override;

  void decodeMain(void*);
};

OZ_INTERNAL
AL::Decoder::OpusStream::~OpusStream()
{
  ogg_stream_clear(&stream);
  ogg_sync_clear(&sync);
}

OZ_INTERNAL
bool AL::Decoder::OpusStream::decode()
{
  return false;
}

OZ_INTERNAL
void AL::Decoder::OpusStream::decodeMain(void*)
{
  while (is.available()) {
    int   nBytes = min<int>(OGG_BUFFER_SIZE, is.available());
    char* data   = ogg_sync_buffer(&sync, nBytes);

    is.read(data, nBytes);
    ogg_sync_wrote(&sync, nBytes);

    while (ogg_sync_pageout(&sync, &page) != 0) {
      int serialno = ogg_page_serialno(&page);
      if (stream.serialno != serialno) {
        ogg_stream_reset_serialno(&stream, serialno);
      }

      ogg_stream_pagein(&stream, &page);

      while (ogg_stream_packetout(&stream, &packet) == 1) {
//        opus_packet_get_nb_samples(packet.packet, packet.bytes, 48000);
        if (packet.b_o_s) {
          OZ_ERROR("oz::AL::Decoder: Only one stream per Opus file supported");
        }
        else {
        }

//        int result = opus_decode_float(decoder, packet.packet, int(packet.bytes),
//                                       output.end() - FRAME_SIZE, FRAME_SIZE, 0);

//        output += result * channels;
      }
    }
  }
}

AL::Decoder::Decoder() :
  samples(nullptr), nSamples(0), format(AL_FORMAT_MONO_FLOAT32), rate(48000), stream(nullptr)
{}

AL::Decoder::Decoder(const File& file, int nSamples)
{
  if (file.hasExtension("wav")) {
    stream = new WaveStream(this, file.read(Endian::LITTLE), nSamples);
  }
}

AL::Decoder::~Decoder()
{
  delete samples;
  delete stream;
}

AL::Decoder::Decoder(Decoder&& d) :
  samples(d.samples), nSamples(d.nSamples), format(d.format), rate(d.rate), stream(d.stream)
{
  samples  = nullptr;
  nSamples = 0;
  format   = AL_FORMAT_MONO_FLOAT32;
  rate     = 48000;
  stream   = nullptr;
}

AL::Decoder& AL::Decoder::operator = (AL::Decoder&& d)
{
  if (&d != this) {
    delete samples;
    delete stream;

    samples  = d.samples;
    nSamples = d.nSamples;
    format   = d.format;
    rate     = d.rate;
    stream   = d.stream;

    d.samples  = nullptr;
    d.nSamples = 0;
    d.format   = AL_FORMAT_MONO_FLOAT32;
    d.rate     = 48000;
    d.stream   = nullptr;
  }
  return *this;
}

bool AL::Decoder::decode()
{
  return stream == nullptr ? false : stream->decode();
}

void AL::Decoder::load(ALuint buffer) const
{
  alBufferData(buffer, format, samples, nSamples * sizeof(float), rate);
}

struct AL::Streamer::Data
{
  static const int BUFFER_SIZE = 65536;

  ALuint         buffers[2];
  OggVorbis_File ovFile;
  ALenum         format;
  ALsizei        rate;
  Stream         is;
  float          samples[BUFFER_SIZE];
};

AL::Streamer::Streamer() :
  source(0), data(nullptr)
{}

AL::Streamer::~Streamer()
{
  destroy();
}

AL::Streamer::Streamer(Streamer&& s) :
  source(s.source), data(s.data)
{
  s.source = 0;
  s.data   = nullptr;
}

AL::Streamer& AL::Streamer::operator = (Streamer&& s)
{
  if (&s == this) {
    return *this;
  }

  source   = s.source;
  data     = s.data;

  s.source = 0;
  s.data   = nullptr;

  return *this;
}

void AL::Streamer::attach(ALuint source_)
{
  detach();

  if (source_ != 0 && alIsSource(source_)) {
    source = source_;

    if (data != nullptr) {
      alSourceQueueBuffers(source, 2, data->buffers);
    }
  }
}

void AL::Streamer::detach()
{
  if (source != 0 && alIsSource(source)) {
    alSourceStop(source);
    OZ_AL_CHECK_ERROR();

    if (data != nullptr) {
      ALint nQueued;
      alGetSourcei(source, AL_BUFFERS_QUEUED, &nQueued);
      OZ_AL_CHECK_ERROR();

      Log() << nQueued;
      if (nQueued != 0) {
        ALuint buffers[2];
        alSourceUnqueueBuffers(source, nQueued, buffers);
        OZ_AL_CHECK_ERROR();
      }
    }
  }
  source = 0;
}

bool AL::Streamer::open(const File& file)
{
  close();

  data     = new Data;
  data->is = file.read();

  if (data->is.available() == 0) {
    delete data;
    data = nullptr;
    return false;
  }

  if (ov_open_callbacks(&data->is, &data->ovFile, nullptr, 0, VORBIS_CALLBACKS) != 0) {
    delete data;
    data = nullptr;
    return false;
  }

  vorbis_info* vorbisInfo = ov_info(&data->ovFile, -1);
  if (vorbisInfo == nullptr) {
    ov_clear(&data->ovFile);
    delete data;
    data = nullptr;
    return false;
  }

  int nChannels = vorbisInfo->channels;
  if (nChannels != 1 && nChannels != 2) {
    ov_clear(&data->ovFile);
    delete data;
    data = nullptr;
    return false;
  }

  data->format = nChannels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
  data->rate   = ALsizei(vorbisInfo->rate);

  alGenBuffers(2, data->buffers);

  for (int i = 0; i < 2; ++i) {
    if (!decodeVorbis(&data->ovFile, nChannels, data->samples, Data::BUFFER_SIZE)) {
      close();
      return false;
    }

    alBufferData(data->buffers[i], data->format, &data->samples, Data::BUFFER_SIZE, data->rate);
  }

  if (source != 0 && alIsSource(source)) {
    alSourceQueueBuffers(source, 2, data->buffers);
  }

  OZ_AL_CHECK_ERROR();
  return true;
}

void AL::Streamer::close()
{
  detach();

  if (data != nullptr) {
    OZ_AL_CHECK_ERROR();
    alDeleteBuffers(2, data->buffers);

    OZ_AL_CHECK_ERROR();

    ov_clear(&data->ovFile);

    delete data;
    data = nullptr;
  }
}

bool AL::Streamer::rewind()
{
  if (data == nullptr) {
    return false;
  }

  if (ov_raw_seek(&data->ovFile, 0) != 0) {
    return false;
  }

  if (source != 0 && alIsSource(source)) {
    alSourceStop(source);
    alSourceUnqueueBuffers(source, 2, data->buffers);
  }
  else {
    source = 0;
  }

  for (int i = 0; i < 2; ++i) {
    if (!decodeVorbis(&data->ovFile, data->format, data->samples, Data::BUFFER_SIZE)) {
      return false;
    }

    alBufferData(data->buffers[i], data->format, &data->samples, Data::BUFFER_SIZE, data->rate);
  }

  if (source != 0) {
    alSourceQueueBuffers(source, 2, data->buffers);
  }

  OZ_AL_CHECK_ERROR();
  return true;
}

bool AL::Streamer::update()
{
  if (source == 0 || data == nullptr) {
    return false;
  }
  if (!alIsSource(source)) {
    source = 0;
    return false;
  }

  ALint nProcessed;
  alGetSourcei(source, AL_BUFFERS_PROCESSED, &nProcessed);

  if (nProcessed == 0) {
    return true;
  }

  if (!decodeVorbis(&data->ovFile, data->format, data->samples, Data::BUFFER_SIZE)) {
    return false;
  }

  ALuint buffer;
  alSourceUnqueueBuffers(source, 1, &buffer);
  alBufferData(buffer, data->format, data->samples, Data::BUFFER_SIZE, data->rate);
  alSourceQueueBuffers(source, 1, &buffer);

  OZ_AL_CHECK_ERROR();
  return true;
}

void AL::Streamer::destroy()
{
  detach();
  close();
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

AudioBuffer AL::decodeFromStream(Stream* is)
{
  AudioBuffer buffer;

  if (is->available() == 0) {
    return buffer;
  }

  // WAVE loader is implemented according to specification found in
  // https://ccrma.stanford.edu/courses/422/projects/WaveFormat/.
  if (is->capacity() >= 44 &&
      String::beginsWith(is->begin(), "RIFF") &&
      String::beginsWith(is->begin() + 8, "WAVE"))
  {
    is->seek(16);

    int formatSize = is->readInt();

    is->readShort();

    int nChannels = is->readShort();
    buffer.rate   = is->readInt();

    is->readInt();
    is->readShort();

    int bits = is->readShort();

    is->seek(20 + formatSize);

    while (is->available() != 0 && !String::beginsWith(is->readSkip(4), "data")) {
      is->readSkip(is->readInt());
    }

    int size = is->readInt();

    if ((nChannels != 1 && nChannels != 2) || (bits != 8 && bits != 16)) {
      return buffer;
    }

    OZ_ASSERT(size <= is->available());

    buffer.format = nChannels == 1 ? AL_FORMAT_MONO_FLOAT32 : AL_FORMAT_STEREO_FLOAT32;

    if (bits == 8) {
      const ubyte* samples = reinterpret_cast<const ubyte*>(is->readSkip(size));

      buffer.data.resize(size / sizeof(char));

      for (float& outSample : buffer.data) {
        outSample = float(*samples++ - 128) / 127.0f;
      }
    }
    else {
      const short* samples = reinterpret_cast<const short*>(is->readSkip(size));

      buffer.data.resize(size / sizeof(short));

      for (float& outSample : buffer.data) {
#if OZ_BYTE_ORDER == 4321
        outSample = float(Endian::bswap(*samples++)) / 32768.0f;
#else
        outSample = float(*samples++) / 32767.0f;
#endif
      }
    }

    OZ_AL_CHECK_ERROR();
    return buffer;
  }
  else {
    OggVorbis_File ovStream;
    if (ov_open_callbacks(is, &ovStream, nullptr, 0, VORBIS_CALLBACKS) < 0) {
      return buffer;
    }

    vorbis_info* vorbisInfo = ov_info(&ovStream, -1);
    if (vorbisInfo == nullptr) {
      ov_clear(&ovStream);
      return buffer;
    }

    int nChannels = vorbisInfo->channels;
    if (nChannels != 1 && nChannels != 2) {
      ov_clear(&ovStream);
      return buffer;
    }

    int size = int(ov_pcm_total(&ovStream, -1)) * nChannels * sizeof(float);

    buffer.data.resize(size);
    buffer.format = nChannels == 1 ? AL_FORMAT_MONO_FLOAT32 : AL_FORMAT_STEREO_FLOAT32;
    buffer.rate   = int(vorbisInfo->rate);

    if (!decodeVorbis(&ovStream, buffer.format, buffer.data.begin(), size)) {
      buffer.data.resize(0);
      ov_clear(&ovStream);
      return buffer;
    }

    ov_clear(&ovStream);

    OZ_AL_CHECK_ERROR();
    return buffer;
  }
}

AudioBuffer AL::decodeFromFile(const File& file)
{
  Stream is = file.read(Endian::LITTLE);
  return decodeFromStream(&is);
}

bool AL::bufferDataFromStream(ALuint bufferId, Stream* is)
{
  AudioBuffer buffer = decodeFromStream(is);

  if (buffer.data.isEmpty()) {
    return false;
  }

  alBufferData(bufferId, buffer.format, buffer.data.begin(), buffer.data.length() * sizeof(float),
               buffer.rate);

  OZ_AL_CHECK_ERROR();
  return true;
}

bool AL::bufferDataFromFile(ALuint bufferId, const File& file)
{
  Stream is = file.read(Endian::LITTLE);
  return bufferDataFromStream(bufferId, &is);
}

}
