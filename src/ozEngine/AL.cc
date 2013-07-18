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
 * @file ozEngine/AL.cc
 */

#include "AL.hh"

// We don't use those callbacks anywhere and they don't compile on MinGW.
#define OV_EXCLUDE_STATIC_CALLBACKS
#include <vorbis/vorbisfile.h>

namespace oz
{

/*
 * Vorbis stream reader callbacks.
 */

static size_t vorbisRead( void* buffer, size_t size, size_t n, void* handle )
{
  InputStream* istream = static_cast<InputStream*>( handle );

  int blockSize = int( size );
  int nBlocks   = min( int( n ), istream->available() / blockSize );

  istream->readChars( static_cast<char*>( buffer ), nBlocks * blockSize );
  return size_t( nBlocks );
}

static int vorbisSeek( void* handle, ogg_int64_t offset, int whence )
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

/*
 * Vorbis decoder helper function.
 */

static bool decodeVorbis( OggVorbis_File* stream, char* buffer, int size )
{
  long bytesRead = 0;
  long result;

  do {
    int section;
    result = ov_read( stream, buffer + bytesRead, size - int( bytesRead ), 0, 2, 1, &section );

    if( result < 0 ) {
      return false;
    }

    bytesRead += result;
  }
  while( result > 0 && bytesRead < long( size ) );

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
  InputStream    istream;
  char           samples[BUFFER_SIZE];

  explicit Data() :
    buffers{ 0, 0 }
  {}
};

AL::Streamer::Streamer() :
  source( 0 ), data( nullptr )
{}

AL::Streamer::Streamer( const File& file ) :
  source( 0 ), data( nullptr )
{
  open( file );
}

AL::Streamer::Streamer( Streamer&& s ) :
  source( s.source ), data( s.data )
{
  s.source = 0;
  s.data   = nullptr;
}

AL::Streamer& AL::Streamer::operator = ( Streamer&& s )
{
  if( &s == this ) {
    return *this;
  }

  source   = s.source;
  data     = s.data;

  s.source = 0;
  s.data   = nullptr;

  return *this;
}

void AL::Streamer::attach( ALuint source_ )
{
  detach();

  if( source_ != 0 && alIsSource( source_ ) ) {
    source = source_;

    if( data != nullptr  ) {
      alSourceQueueBuffers( source, 2, data->buffers );
    }
  }
}

void AL::Streamer::detach()
{
  if( source != 0 && data != nullptr && alIsSource( source ) ) {
    alSourceStop( source );

    ALint nQueued;
    alGetSourcei( source, AL_BUFFERS_PROCESSED, &nQueued );

    if( nQueued != 0 ) {
      alSourceUnqueueBuffers( source, 2, data->buffers );
    }
  }
  source = 0;
}

bool AL::Streamer::rewind()
{
  if( data == nullptr ) {
    return false;
  }

  if( ov_raw_seek( &data->ovFile, 0 ) != 0 ) {
    close();
    return false;
  }

  if( source != 0 && alIsSource( source ) ) {
    alSourceStop( source );
    alSourceUnqueueBuffers( source, 2, data->buffers );
  }
  else {
    source = 0;
  }

  for( int i = 0; i < 2; ++i ) {
    if( !decodeVorbis( &data->ovFile, data->samples, Data::BUFFER_SIZE ) ) {
      close();
      return false;
    }

    alBufferData( data->buffers[i], data->format, &data->samples, Data::BUFFER_SIZE, data->rate );
  }

  if( source != 0 ) {
    alSourceQueueBuffers( source, 2, data->buffers );
  }

  OZ_AL_CHECK_ERROR();
  return true;
}

bool AL::Streamer::update()
{
  if( source == 0 || data == nullptr ) {
    return false;
  }
  if( !alIsSource( source ) ) {
    source = 0;
    return false;
  }

  ALint nProcessed;
  alGetSourcei( source, AL_BUFFERS_PROCESSED, &nProcessed );

  if( nProcessed == 0 ) {
    return true;
  }

  if( !decodeVorbis( &data->ovFile, data->samples, Data::BUFFER_SIZE ) ) {
    close();
    return false;
  }

  ALuint buffer;
  alSourceUnqueueBuffers( source, 1, &buffer );
  alBufferData( buffer, data->format, data->samples, Data::BUFFER_SIZE, data->rate );
  alSourceQueueBuffers( source, 1, &buffer );

  OZ_AL_CHECK_ERROR();
  return true;
}

bool AL::Streamer::open( const File& file )
{
  close();

  data             = new Data();
  data->fileBuffer = file.read();
  data->istream    = data->fileBuffer.inputStream();

  if( data->fileBuffer.isEmpty() ) {
    delete data;
    data = nullptr;
    return false;
  }

  if( ov_open_callbacks( &data->istream, &data->ovFile, nullptr, 0, VORBIS_CALLBACKS ) != 0 ) {
    delete data;
    data = nullptr;
    return false;
  }

  vorbis_info* vorbisInfo = ov_info( &data->ovFile, -1 );
  if( vorbisInfo == nullptr ) {
    close();
    return false;
  }

  int nChannels = vorbisInfo->channels;
  if( nChannels != 1 && nChannels != 2 ) {
    close();
    return false;
  }

  data->format = nChannels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
  data->rate   = ALsizei( vorbisInfo->rate );

  hard_assert( data->buffers[0] == 0 && data->buffers[1] == 0 );
  alGenBuffers( 2, data->buffers );

  for( int i = 0; i < 2; ++i ) {
    if( !decodeVorbis( &data->ovFile, data->samples, Data::BUFFER_SIZE ) ) {
      close();
      return false;
    }

    alBufferData( data->buffers[i], data->format, &data->samples, Data::BUFFER_SIZE, data->rate );
  }

  if( source != 0 && alIsSource( source ) ) {
    alSourceQueueBuffers( source, 2, data->buffers );
  }

  OZ_AL_CHECK_ERROR();
  return true;
}

void AL::Streamer::close()
{
  if( data != nullptr ) {
    if( source != 0 && alIsSource( source ) ) {
      alSourceStop( source );

      ALint nQueued;
      alGetSourcei( source, AL_BUFFERS_PROCESSED, &nQueued );

      if( nQueued != 0 ) {
        alSourceUnqueueBuffers( source, 2, data->buffers );
      }
    }

    alDeleteBuffers( 2, data->buffers );
    ov_clear( &data->ovFile );

    delete data;
    data = nullptr;

    OZ_AL_CHECK_ERROR();
  }
}

void AL::Streamer::destroy()
{
  detach();
  close();
}

void AL::checkError( const char* function, const char* file, int line )
{
  const char* message;
  ALenum result = alGetError();

  switch( result ) {
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
      message = String::str( "UNKNOWN(%d)", int( result ) );
      break;
    }
  }

  System::error( function, file, line, 1, "AL error '%s'", message );
}

bool AL::bufferDataFromFile( ALuint buffer, const File &file )
{
  InputStream istream = file.inputStream();

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

    istream.seek( 36 );

    const char* chunkName = istream.pos();
    istream.readInt();

    int size = istream.readInt();

    while( !String::beginsWith( chunkName, "data" ) ) {
      istream.forward( size );

      if( !istream.isAvailable() ) {
        return false;
      }

      chunkName = istream.pos();
      istream.readInt();

      size = istream.readInt();
    }

    if( ( nChannels != 1 && nChannels != 2 ) || ( bits != 8 && bits != 16 ) ) {
      return false;
    }

    ALenum format = nChannels == 1 ? bits == 8 ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16 :
                                     bits == 8 ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;

#if OZ_BYTE_ORDER == 4321

    int    nSamples = size / int( sizeof( short ) );
    short* data     = new short[nSamples];

    mCopy( data, istream.forward( size ), size_t( size ) );

    for( int i = 0; i < nSamples; ++i ) {
      data[i] = Endian::bswap16( data[i] );
    }

#else

    const char* data = istream.forward( size );

#endif

    alBufferData( buffer, format, data, size, rate );
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

    alBufferData( buffer, format, data, size, rate );

    delete[] data;
    ov_clear( &ovStream );

    return true;
  }
}

}
