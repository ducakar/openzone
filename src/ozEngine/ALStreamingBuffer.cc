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
 * @file ozEngine/ALStreamingBuffer.cc
 */

#include "ALStreamingBuffer.hh"

namespace oz
{

ALStreamingBuffer::ALStreamingBuffer() :
  sourceId( 0 )
{}

ALStreamingBuffer::ALStreamingBuffer( const File& file ) :
  sourceId( 0 )
{
  open( file );
}

ALStreamingBuffer::ALStreamingBuffer( ALStreamingBuffer&& b ) :
  streamer( static_cast<AL::Streamer&&>( b.streamer ) ), sourceId( b.sourceId )
{
  b.sourceId = 0;
}

ALStreamingBuffer& ALStreamingBuffer::operator = ( ALStreamingBuffer&& b )
{
  if( &b == this ) {
    return *this;
  }

  streamer   = static_cast<AL::Streamer&&>( b.streamer );
  sourceId   = b.sourceId;

  b.sourceId = 0;

  return *this;
}

ALStreamingBuffer::~ALStreamingBuffer()
{
  destroy();
}

bool ALStreamingBuffer::update()
{
  return streamer.update();
}

bool ALStreamingBuffer::rewind()
{
  return streamer.rewind();
}

ALSource ALStreamingBuffer::createSource()
{
  ALSource source;

  if( streamer.isStreaming() ) {
    source.create();

    if( source.isCreated() ) {
      sourceId = source.id();
      streamer.attach( sourceId );
    }
  }
  return source;
}

bool ALStreamingBuffer::open( const File& file )
{
  return streamer.open( file );
}

void ALStreamingBuffer::close()
{
  streamer.close();
}

void ALStreamingBuffer::destroy()
{
  streamer.destroy();
}

}
