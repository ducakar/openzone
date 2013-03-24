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
 * @file ozEngine/ALBuffer.cc
 */

#include "ALBuffer.hh"

#include "OpenAL.hh"
#include "vorbis.h"

namespace oz
{

ALBuffer::ALBuffer() :
  bufferId( 0 )
{}

ALBuffer::ALBuffer( const File& file ) :
  bufferId( 0 )
{
  load( file );
}

ALBuffer::~ALBuffer()
{
  destroy();
}

ALSource ALBuffer::createSource() const
{
  ALSource source;

  if( bufferId != 0 ) {
    source.create();

    if( source.isCreated() ) {
      alSourcei( source.id(), AL_BUFFER, int( bufferId ) );
    }
  }
  return source;
}

bool ALBuffer::create()
{
  if( bufferId == 0 ) {
    alGenBuffers( 1, &bufferId );
  }
  return bufferId != 0;
}

bool ALBuffer::load( const File& file )
{
  Buffer      buffer;
  InputStream istream;

  if( file.isMapped() ) {
    istream = file.inputStream();
  }
  else {
    buffer  = file.read();
    istream = buffer.inputStream();
  }

  if( !istream.isAvailable() ) {
    return false;
  }

  // WAVE loader is implemented according to specification found in
  // https://ccrma.stanford.edu/courses/422/projects/WaveFormat/.
  if( istream.capacity() >= 44 &&
      String::beginsWith( istream.begin(), "RIFF" ) &&
      String::beginsWith( istream.begin() + 8, "WAVE" ) )
  {
    istream.seek( 22 );
    int nChannels = int( istream.readShort() );
    int rate      = istream.readInt();

    istream.seek( 34 );
    int bits = int( istream.readShort() );

    istream.seek( 40 );
    int size = istream.readInt();

    if( ( nChannels != 1 && nChannels != 2 ) || ( bits != 8 && bits != 16 ) ) {
      return false;
    }

    ALenum format = nChannels == 1 ? bits == 8 ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16 :
                                     bits == 8 ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;

#ifdef OZ_BIG_ENDIAN

    int    nSamples = size / int( sizeof( short ) );
    short* data     = new short[nSamples];

    for( int i = 0; i < nSamples; ++i ) {
      data[i] = Endian::bswap16( istream->readShort() );
    }

#else

    const char* data = istream.forward( size );

#endif

    create();

    if( bufferId != 0 ) {
      alBufferData( bufferId, format, data, size, rate );
    }

    OZ_AL_CHECK_ERROR();
    return bufferId != 0;
  }
  else {
    OggVorbis_File ovStream;
    if( ov_open_callbacks( &istream, &ovStream, nullptr, 0, VORBIS_CALLBACKS ) < 0 ) {
      return false;
    }

    vorbis_info* vorbisInfo = ov_info( &ovStream, -1 );
    if( vorbisInfo == nullptr ) {
      ov_clear( &ovStream );
      return false;
    }

    int nChannels = vorbisInfo->channels;
    if( nChannels != 1 && nChannels != 2 ) {
      ov_clear( &ovStream );
      return false;
    }

    ALenum format = nChannels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    int    rate   = int( vorbisInfo->rate );
    int    size   = int( ov_pcm_total( &ovStream, -1 ) ) * nChannels * int( sizeof( short ) );
    char*  data   = new char[size];

    if( !decodeVorbis( &ovStream, data, size ) ) {
      delete[] data;
      ov_clear( &ovStream );
      return false;
    }

    create();

    if( bufferId != 0 ) {
      alBufferData( bufferId, format, data, size, rate );
    }

    delete[] data;
    ov_clear( &ovStream );

    OZ_AL_CHECK_ERROR();
    return bufferId != 0;
  }
}

void ALBuffer::destroy()
{
  if( bufferId != 0 ) {
    alDeleteBuffers( 1, &bufferId );
    bufferId = 0;
  }
}

}
