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

#include "AL.hh"

// We don't use those callbacks anywhere and they don't compile on MinGW.
#define OV_EXCLUDE_STATIC_CALLBACKS

#include <cstring>
#include <opus/opusfile.h>
#include <vorbis/vorbisfile.h>

namespace oz
{

namespace
{

/*
 * Vorbis stream reader callbacks.
 */

size_t vorbisRead(void* buffer, size_t size, size_t n, void* handle)
{
  Stream* is = static_cast<Stream*>(handle);

  int blockSize = int(size);
  int nBlocks   = min<int>(int(n), is->available() / blockSize);

  is->read(static_cast<char*>(buffer), nBlocks * blockSize);
  return nBlocks;
}

int vorbisSeek(void* handle, ogg_int64_t offset, int whence)
{
  Stream* is = static_cast<Stream*>(handle);

  int origin = whence == SEEK_CUR ? is->tell() : whence == SEEK_END ? is->capacity() : 0;

  is->seek(origin + int(offset));
  return 0;
}

long vorbisTell(void* handle)
{
  Stream* is = static_cast<Stream*>(handle);

  return is->tell();
}

ov_callbacks VORBIS_CALLBACKS = {vorbisRead, vorbisSeek, nullptr, vorbisTell};

}

/*
 * Class members.
 */

class AL::Decoder::StreamBase
{
protected:

  Stream is_;

public:

  StreamBase() = default;

  StreamBase(const StreamBase&) = delete;
  StreamBase(StreamBase&&) noexcept = default;

  StreamBase& operator=(const StreamBase&) = delete;
  StreamBase& operator=(StreamBase&&) noexcept = default;

  OZ_INTERNAL
  virtual ~StreamBase();

  OZ_INTERNAL
  virtual bool decode(AL::Decoder* decoder) = 0;

};

AL::Decoder::StreamBase::~StreamBase() = default;

class AL::Decoder::WaveStream : public AL::Decoder::StreamBase
{
private:

  int sampleSize_;

public:

  OZ_INTERNAL
  explicit WaveStream(AL::Decoder* decoder, const File& file)
  {
    is_ = Stream(0, Endian::LITTLE);
    if (!file.read(&is_)) {
      OZ_ERROR("oz::AL::Decoder: Failed to open WAVE file `%s'", file.c());
    }

    if (is_.available() < 44) {
      OZ_ERROR("oz::AL::Decoder: Invalid WAVE header in `%s'", file.c());
    }

    is_.seek(22);

    int nChannels = is_.readInt16();
    int rate      = is_.readInt();

    is_.seek(34);

    sampleSize_ = is_.readInt16() / 8;

    while (is_.available() != 0 && !String::beginsWith(is_.readSkip(4), "data")) {
      is_.readSkip(is_.readInt());
    }

    int size = is_.readInt();

    if ((nChannels != 1 && nChannels != 2) || (sampleSize_ != 1 && sampleSize_ != 2)) {
      OZ_ERROR("oz::AL::Decoder: Only mono and stereo, 8-bit and 16-bit WAVE PCM supported `%s'",
               file.c());
    }

    decoder->format_   = nChannels == 2 ? FORMAT_STEREO_FLOAT32 : FORMAT_MONO_FLOAT32;
    decoder->rate_     = rate;
    decoder->capacity_ = size / sampleSize_;
    decoder->samples_  = new float[decoder->capacity_];
  }

  OZ_INTERNAL
  ~WaveStream() override;

  WaveStream(const WaveStream&) = delete;
  WaveStream(WaveStream&&) = delete;

  WaveStream& operator=(const WaveStream&) = delete;
  WaveStream& operator=(WaveStream&&) = delete;

  OZ_INTERNAL
  bool decode(AL::Decoder* decoder) override
  {
    if (decoder->size_ != 0) {
      return false;
    }

    float* begin      = decoder->samples_;
    float* end        = decoder->samples_ + decoder->capacity_;
    float* alignedEnd = begin + Math::alignDown<ptrdiff_t>(end - begin, 4);

    if (sampleSize_ == 1) {
      const ubyte* samples = reinterpret_cast<const ubyte*>(is_.pos());

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
      const int16* samples = reinterpret_cast<const int16*>(is_.pos());

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

    decoder->size_ = decoder->capacity_;
    return true;
  }

};

AL::Decoder::WaveStream::~WaveStream() = default;

class AL::Decoder::OpusStream : public AL::Decoder::StreamBase
{
private:

  OZ_INTERNAL
  static constexpr int FRAME_SIZE = 120 * 48;

  OggOpusFile* opFile_ = nullptr;

public:

  OZ_INTERNAL
  explicit OpusStream(AL::Decoder* decoder, const File& file, bool isStreaming)
  {
    is_ = Stream(0);
    if (!file.read(&is_)) {
      OZ_ERROR("oz::AL::Decoder: Failed to open Opus file `%s'", file.c());
    }

    int error = 0;
    opFile_ = op_open_memory(reinterpret_cast<const ubyte*>(is_.begin()), is_.available(), &error);
    if (error != 0) {
      OZ_ERROR("oz::AL::Decoder: Invalid Opus file `%s'", file.c());
    }

    int nChannels = op_channel_count(opFile_, -1);
    int nSamples  = int(op_pcm_total(opFile_, -1));

    if (nChannels != 1 && nChannels != 2) {
      OZ_ERROR("oz::AL::Decoder: Only mono and stereo Opus supported `%s'", file.c());
    }

    decoder->format_   = nChannels == 2 ? FORMAT_STEREO_FLOAT32 : FORMAT_MONO_FLOAT32;
    decoder->rate_     = 48000;
    decoder->capacity_ = isStreaming ? FRAME_SIZE * nChannels : nSamples * nChannels;
    decoder->samples_  = new float[decoder->capacity_];
  }

  OZ_INTERNAL
  ~OpusStream() override;

  OpusStream(const OpusStream&) = delete;
  OpusStream(OpusStream&&) = delete;

  OpusStream& operator=(const OpusStream&) = delete;
  OpusStream& operator=(OpusStream&&) = delete;

  OZ_INTERNAL
  bool decode(AL::Decoder* decoder) override
  {
    decoder->size_ = 0;

    while (decoder->size_ != decoder->capacity_) {
      int result = 0;

      if (decoder->format_ == FORMAT_STEREO_FLOAT32) {
        result = op_read_float_stereo(opFile_, decoder->samples_ + decoder->size_,
                                      decoder->capacity_ - decoder->size_) * 2;
      }
      else {
        result = op_read_float(opFile_, decoder->samples_ + decoder->size_,
                              decoder->capacity_ - decoder->size_, nullptr);
      }

      if (result <= 0) {
        return decoder->size_ != 0;
      }

      decoder->size_ += result;
    }

    return true;
  }

};

AL::Decoder::OpusStream::~OpusStream()
{
  op_free(opFile_);
}

class AL::Decoder::VorbisStream : public AL::Decoder::StreamBase
{
private:

  OggVorbis_File ovFile_;

public:

  OZ_INTERNAL
  explicit VorbisStream(AL::Decoder* decoder, const File& file, bool isStreaming)
  {
    is_ = Stream(0);
    if (!file.read(&is_)) {
      OZ_ERROR("oz::AL::Decoder: Failed to open Vorbis file `%s'", file.c());
    }

    int error = ov_open_callbacks(&is_, &ovFile_, nullptr, 0, VORBIS_CALLBACKS);
    if (error != 0) {
      OZ_ERROR("oz::AL::Decoder: Invalid Vorbis file `%s'", file.c());
    }

    vorbis_info* ovInfo = ov_info(&ovFile_, -1);

    int nChannels = ovInfo->channels;
    int rate      = int(ovInfo->rate);
    int frameSize = 120 * rate / 1000;
    int nSamples  = int(ov_pcm_total(&ovFile_, -1));

    if (nChannels != 1 && nChannels != 2) {
      OZ_ERROR("oz::AL::Decoder: Only mono and stereo Vorbis supported `%s'", file.c());
    }

    decoder->format_   = nChannels == 2 ? FORMAT_STEREO_FLOAT32 : FORMAT_MONO_FLOAT32;
    decoder->rate_     = rate;
    decoder->capacity_ = isStreaming ? frameSize * nChannels : nSamples * nChannels;
    decoder->samples_  = new float[decoder->capacity_];
  }

  OZ_INTERNAL
  ~VorbisStream() override;

  VorbisStream(const VorbisStream&) = delete;
  VorbisStream(VorbisStream&&) = delete;

  VorbisStream& operator=(const VorbisStream&) = delete;
  VorbisStream& operator=(VorbisStream&&) = delete;

  OZ_INTERNAL
  bool decode(AL::Decoder* decoder) override
  {
    int stereo = decoder->format_ == FORMAT_STEREO_FLOAT32;

    decoder->size_ = 0;

    while (decoder->size_ != decoder->capacity_) {
      float** samples = nullptr;
      int     section = 0;
      long    result  = ov_read_float(&ovFile_, &samples,
                                      (decoder->capacity_ - decoder->size_) >> stereo, &section);
      if (result <= 0) {
        return decoder->size_ != 0;
      }

      float* output = &decoder->samples_[decoder->size_];

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

      decoder->size_ += result;
    }

    return true;
  }

};

AL::Decoder::VorbisStream::~VorbisStream()
{
  ov_clear(&ovFile_);
}

AL::Decoder::Decoder(const File& file, bool isStreaming)
{
  if (file.hasExtension("wav")) {
    stream_ = new WaveStream(this, file);
  }
  else if (file.hasExtension("opus")) {
    stream_ = new OpusStream(this, file, isStreaming);
  }
  else if (file.hasExtension("oga") || file.hasExtension("ogg")) {
    stream_ = new VorbisStream(this, file, isStreaming);
  }
  else {
    OZ_ERROR("oz::AL::Decoder: Unknown file extension `%s'", file.c());
  }
}

AL::Decoder::~Decoder()
{
  delete[] samples_;
  delete stream_;
}

AL::Decoder::Decoder(Decoder&& other) noexcept
  : Decoder()
{
  swap(*this, other);
}

AL::Decoder& AL::Decoder::operator=(AL::Decoder&& other) noexcept
{
  swap(*this, other);
  return *this;
}

bool AL::Decoder::decode()
{
  if (stream_ != nullptr && !stream_->decode(this)) {
    delete stream_;
    stream_ = nullptr;
  }
  return stream_ != nullptr;
}

void AL::Decoder::load(ALuint buffer) const
{
  alBufferData(buffer, format_, samples_, size_ * sizeof(float), rate_);
}

void AL::checkError(const char* function, const char* file, int line)
{
  const char* message = "UNKNOWN";

  switch (alGetError()) {
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
  }

  System::error(function, file, line, 1, "AL error `%s'", message);
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
