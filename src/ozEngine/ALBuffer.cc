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
#include <vorbis/vorbisfile.h>

namespace oz
{

static size_t vorbisRead( void* buffer, size_t size, size_t n, void* handle )
{
  InputStream* istream = static_cast<InputStream*>( handle );

  int blockSize = int( size );
  int nBlocks   = min( int( n ), istream->available() / blockSize );

  istream->readChars( static_cast<char*>( buffer ), nBlocks * blockSize );
  return size_t( nBlocks );
}

static int vorbisSeek( void* handle, long offset, int whence )
{
  InputStream* istream = static_cast<InputStream*>( handle );

  const char* origin = whence == SEEK_CUR ? istream->pos() :
                       whence == SEEK_END ? istream->end() : istream->begin();

  istream->set( origin + offset );
  return 0;
}

static long vorbisTell( void* handle )
{
  InputStream* istream = static_cast<InputStream*>( handle );

  return long( istream->tell() );
}

static ov_callbacks VORBIS_CALLBACKS = { vorbisRead, vorbisSeek, nullptr, vorbisTell };

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

bool ALBuffer::load( const File& file )
{
  destroy();

  Buffer      buffer  = file.read();
  InputStream istream = buffer.inputStream();

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

    alGenBuffers( 1, &bufferId );
    alBufferData( bufferId, format, istream.forward( size ), size, rate );

    OZ_AL_CHECK_ERROR();
    return true;
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

    int rate      = int( vorbisInfo->rate );
    int nChannels = vorbisInfo->channels;
    int size      = int( ov_pcm_total( &ovStream, -1 ) ) * nChannels * 2;

    if( nChannels != 1 && nChannels != 2 ) {
      ov_clear( &ovStream );
      return false;
    }

    char* data      = new char[size];
    long  bytesRead = 0;
    long  result;

    do {
      int section;
      result = ov_read( &ovStream, data + bytesRead, size - int( bytesRead ), 0, 2, 1, &section );

      if( result < 0 ) {
        delete[] data;
        ov_clear( &ovStream );
        return false;
      }

      bytesRead += result;
    }
    while( result > 0 && bytesRead < size );

    ALenum format = nChannels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

    alGenBuffers( 1, &bufferId );
    alBufferData( bufferId, format, data, size, rate );

    delete[] data;
    ov_clear( &ovStream );

    OZ_AL_CHECK_ERROR();
    return true;
  }
}

void ALBuffer::destroy()
{
  if( bufferId != 0 ) {
    alDeleteBuffers( 1, &bufferId );
    bufferId = 0;

    OZ_AL_CHECK_ERROR();
  }
}

}
