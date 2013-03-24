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
 * @file ozEngine/vorbis.cc
 */

#include "vorbis.h"

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

OZ_HIDDEN
ov_callbacks VORBIS_CALLBACKS = { vorbisRead, vorbisSeek, nullptr, vorbisTell };

OZ_HIDDEN
bool decodeVorbis( OggVorbis_File* stream, char* buffer, int size )
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

}
