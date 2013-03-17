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
 * @file ozEngine/ALSource.cc
 */

#include "ALSource.hh"

#include "OpenAL.hh"

namespace oz
{

ALSource::ALSource() :
  sourceId( 0 )
{}

ALSource::ALSource( const ALBuffer& buffer ) :
  sourceId( 0 )
{
  create( buffer );
}

ALSource::~ALSource()
{
  destroy();
}

bool ALSource::create( const ALBuffer& buffer )
{
  destroy();

  if( !buffer.isLoaded() ) {
    return false;
  }

  alGenSources( 1, &sourceId );
  alSourcei( sourceId, AL_BUFFER, int( buffer.id() ) );

  // This is not necessary by specification but seems it's always the case in openalsoft.
  hard_assert( sourceId != 0 );

  OZ_AL_CHECK_ERROR();
  return true;
}

void ALSource::destroy()
{
  if( sourceId != 0 ) {
    alDeleteSources( 1, &sourceId );
    sourceId = 0;

    OZ_AL_CHECK_ERROR();
  }
}

}
