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

#include "AL.hh"

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
      alSourcei( source.id(), AL_BUFFER, ALint( bufferId ) );
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
  create();

  if( bufferId == 0 ) {
    return false;
  }

  return AL::bufferDataFromFile( bufferId, file );
}

void ALBuffer::destroy()
{
  if( bufferId != 0 ) {
    alDeleteBuffers( 1, &bufferId );
    bufferId = 0;
  }
}

}
