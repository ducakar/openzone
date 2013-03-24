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
 * @file ozEngine/ALStreamingBuffer.cc
 */

#include "ALStreamingBuffer.hh"

#include "OpenAL.hh"

#include "vorbis.h"

namespace oz
{

struct ALStreamingBuffer::Stream
{
  static const int BUFFER_SIZE = 65536 / 2;

  Buffer         buffer;
  InputStream    istream;
  OggVorbis_File ovStream;
  ALenum         format;
  int            rate;
  char           samples[BUFFER_SIZE];
};

ALStreamingBuffer::ALStreamingBuffer()
{
  bufferIds[0] = 0;
  bufferIds[1] = 0;
  sourceId     = 0;
  stream       = nullptr;
}

ALStreamingBuffer::ALStreamingBuffer( const File& file )
{
  bufferIds[0] = 0;
  bufferIds[1] = 0;
  sourceId     = 0;
  stream       = nullptr;

  load( file );
}

ALStreamingBuffer::~ALStreamingBuffer()
{
  destroy();
}

bool ALStreamingBuffer::update()
{
  if( sourceId == 0 || stream == nullptr ) {
    return false;
  }
  if( !alIsSource( sourceId ) ) {
    sourceId = 0;
    return false;
  }

  int nProcessed;
  alGetSourcei( sourceId, AL_BUFFERS_PROCESSED, &nProcessed );

  if( nProcessed == 0 ) {
    return true;
  }

  uint bufferId;
  alSourceUnqueueBuffers( sourceId, 1, &bufferId );

  if( !decodeVorbis( &stream->ovStream, stream->samples, Stream::BUFFER_SIZE ) ) {
    ov_clear( &stream->ovStream );
    delete stream;
    stream = nullptr;
    return false;
  }

  alBufferData( bufferId, stream->format, stream->samples, Stream::BUFFER_SIZE, stream->rate );
  alSourceQueueBuffers( sourceId, 1, &bufferId );
  return true;
}

ALSource ALStreamingBuffer::createSource()
{
  ALSource source;

  if( bufferIds[0] != 0 ) {
    source.create();

    if( source.isCreated() ) {
      sourceId = source.id();
      alSourceQueueBuffers( source.id(), 2, bufferIds );
    }
  }
  return source;
}

bool ALStreamingBuffer::create()
{
  if( bufferIds[0] == 0 ) {
    alGenBuffers( 2, bufferIds );
  }
  return bufferIds[0] != 0;
}

bool ALStreamingBuffer::load( const File& file )
{
  if( sourceId != 0 ) {
    if( alIsSource( sourceId ) ) {
      alSourceStop( sourceId );
      alSourceUnqueueBuffers( sourceId, 2, bufferIds );
    }
    else {
      sourceId = 0;
    }
  }

  stream = stream == nullptr ? new Stream() : stream;
  stream->buffer = file.read();
  stream->istream = stream->buffer.inputStream();

  if( stream->buffer.isEmpty() ) {
    delete stream;
    stream = nullptr;
    return false;
  }

  if( ov_open_callbacks( &stream->istream, &stream->ovStream, nullptr, 0, VORBIS_CALLBACKS ) != 0 )
  {
    delete stream;
    stream = nullptr;
    return false;
  }

  vorbis_info* vorbisInfo = ov_info( &stream->ovStream, -1 );
  if( vorbisInfo == nullptr ) {
    ov_clear( &stream->ovStream );
    delete stream;
    stream = nullptr;
    return false;
  }

  int nChannels = vorbisInfo->channels;
  if( nChannels != 1 && nChannels != 2 ) {
    ov_clear( &stream->ovStream );
    delete stream;
    stream = nullptr;
    return false;
  }

  stream->format = nChannels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
  stream->rate   = int( vorbisInfo->rate );

  create();

  if( bufferIds[0] != 0 ) {
    if( !decodeVorbis( &stream->ovStream, stream->samples, Stream::BUFFER_SIZE ) ) {
      ov_clear( &stream->ovStream );
      delete stream;
      stream = nullptr;
    }

    alBufferData( bufferIds[0], stream->format, &stream->samples, Stream::BUFFER_SIZE,
                  stream->rate );

    if( !decodeVorbis( &stream->ovStream, stream->samples, Stream::BUFFER_SIZE ) ) {
      ov_clear( &stream->ovStream );
      delete stream;
      stream = nullptr;
    }

    alBufferData( bufferIds[1], stream->format, &stream->samples, Stream::BUFFER_SIZE,
                  stream->rate );
  }

  if( sourceId != 0 ) {
    alSourceQueueBuffers( sourceId, 2, bufferIds );
  }
  OZ_AL_CHECK_ERROR();
  return true;
}

void ALStreamingBuffer::destroy()
{
  if( bufferIds[0] != 0 ) {
    if( sourceId != 0 && alIsSource( sourceId ) ) {
      alSourceStop( sourceId );
    }

    delete stream;

    alDeleteBuffers( 2, bufferIds );
    bufferIds[0] = 0;
    bufferIds[1] = 0;
    sourceId     = 0;
    stream       = nullptr;
  }
}

}
