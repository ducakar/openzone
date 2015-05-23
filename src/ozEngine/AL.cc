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
 * @file ozEngine/AL.cc
 */

#include "AL.hh"

#include <cstring>

#include <AL/alc.h>
// We don't use those callbacks anywhere and they don't compile on MinGW.
#define OV_EXCLUDE_STATIC_CALLBACKS
#include <vorbis/vorbisfile.h>

namespace oz
{

static ALCdevice*  soundDevice  = nullptr;
static ALCcontext* soundContext = nullptr;

/*
 * Vorbis stream reader callbacks.
 */

static size_t vorbisRead(void* buffer, size_t size, size_t n, void* handle)
{
  Stream* is = static_cast<Stream*>(handle);

  int blockSize = int(size);
  int nBlocks   = min(int(n), is->available() / blockSize);

  is->readChars(static_cast<char*>(buffer), nBlocks * blockSize);
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

static ov_callbacks VORBIS_CALLBACKS = { vorbisRead, vorbisSeek, nullptr, vorbisTell };

/*
 * Vorbis decoder helper function.
 */

static bool decodeVorbis(OggVorbis_File* stream, char* buffer, int size)
{
  long bytesRead = 0;
  long result;

  do {
    int section;
    result = ov_read(stream, buffer + bytesRead, size - int(bytesRead), 0, 2, 1, &section);

    if (result < 0) {
      return false;
    }

    bytesRead += result;
  }
  while (result > 0 && bytesRead < size);

  return result > 0;
}

/*
 * Class members.
 */

struct AL::Streamer::Data
{
  static const int BUFFER_SIZE = 65536;

  ALuint         buffers[2];
  OggVorbis_File ovFile;
  ALenum         format;
  ALsizei        rate;
  Buffer         fileBuffer;
  Stream         is;
  char           samples[BUFFER_SIZE];
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

  data             = new Data;
  data->fileBuffer = file.read();
  data->is         = Stream(data->fileBuffer);

  if (data->fileBuffer.isEmpty()) {
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
    if (!decodeVorbis(&data->ovFile, data->samples, Data::BUFFER_SIZE)) {
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
    if (!decodeVorbis(&data->ovFile, data->samples, Data::BUFFER_SIZE)) {
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

  if (!decodeVorbis(&data->ovFile, data->samples, Data::BUFFER_SIZE)) {
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

bool AL::bufferDataFromFile(ALuint buffer, const File& file)
{
  Stream is = file.inputStream(Endian::LITTLE);

  if (is.available() == 0) {
    return false;
  }

  // WAVE loader is implemented according to specification found in
  // https://ccrma.stanford.edu/courses/422/projects/WaveFormat/.
  if (is.capacity() >= 44 &&
      String::beginsWith(is.begin(), "RIFF") &&
      String::beginsWith(is.begin() + 8, "WAVE"))
  {
    is.seek(22);
    int nChannels = is.readShort();
    int rate      = is.readInt();

    is.seek(34);
    int bits = is.readShort();

    is.seek(36);

    const char* chunkName = &is[is.tell()];
    is.readInt();

    int size = is.readInt();

    while (!String::beginsWith(chunkName, "data")) {
      is.readSkip(size);

      if (is.available() == 0) {
        return false;
      }

      chunkName = &is[is.tell()];
      is.readInt();

      size = is.readInt();
    }

    if ((nChannels != 1 && nChannels != 2) || (bits != 8 && bits != 16)) {
      return false;
    }

    ALenum format = nChannels == 1 ? bits == 8 ? AL_FORMAT_MONO8   : AL_FORMAT_MONO16 :
                    bits == 8 ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;

    size = min(size, is.available());

    const char* data = is.readSkip(size);

#if OZ_BYTE_ORDER == 4321

    if (nChannels == 2) {
      int    nSamples = size / sizeof(short);
      short* samples  = new short[nSamples];

      memcpy(samples, data, size);

      for (int i = 0; i < nSamples; ++i) {
        samples[i] = Endian::bswap16(samples[i]);
      }

      data = reinterpret_cast<const char*>(samples);
    }

#endif

    alBufferData(buffer, format, data, size, rate);

#if OZ_BYTE_ORDER == 4321

    if (nChannels == 2) {
      delete[] data;
    }

#endif

    OZ_AL_CHECK_ERROR();
    return true;
  }
  else {
    OggVorbis_File ovStream;
    if (ov_open_callbacks(&is, &ovStream, nullptr, 0, VORBIS_CALLBACKS) < 0) {
      return false;
    }

    vorbis_info* vorbisInfo = ov_info(&ovStream, -1);
    if (vorbisInfo == nullptr) {
      ov_clear(&ovStream);
      return false;
    }

    int nChannels = vorbisInfo->channels;
    if (nChannels != 1 && nChannels != 2) {
      ov_clear(&ovStream);
      return false;
    }

    ALenum format = nChannels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    int    rate   = int(vorbisInfo->rate);
    int    size   = int(ov_pcm_total(&ovStream, -1)) * nChannels * sizeof(short);
    char*  data   = new char[size];

    if (!decodeVorbis(&ovStream, data, size)) {
      delete[] data;
      ov_clear(&ovStream);
      return false;
    }

    alBufferData(buffer, format, data, size, rate);

    delete[] data;
    ov_clear(&ovStream);

    OZ_AL_CHECK_ERROR();
    return true;
  }
}

bool AL::init()
{
  destroy();

  Log::print("Opening default OpenAL device and creating context ... ");

  soundDevice = alcOpenDevice(nullptr);
  if (soundDevice == nullptr) {
    Log::printEnd("Failed to open OpenAL device");
    return false;
  }

  soundContext = alcCreateContext(soundDevice, nullptr);
  if (soundContext == nullptr) {
    Log::printEnd("Failed to create OpenAL context");
    return false;
  }

  if (alcMakeContextCurrent(soundContext) != ALC_TRUE) {
    Log::printEnd("Failed to select OpenAL context");
    return false;
  }

  Log::printEnd("OK");
  return true;
}

void AL::destroy()
{
  if (soundContext != nullptr) {
    alcDestroyContext(soundContext);
    soundContext = nullptr;
  }

  if (soundDevice != nullptr) {
    alcCloseDevice(soundDevice);
    soundDevice = nullptr;
  }
}

}
