/*
 * liboz - OpenZone Core Library.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file oz/Mutex.cc
 */

#include "Mutex.hh"

#include "System.hh"

#include <cstdlib>
#include <pthread.h>

namespace oz
{

struct Mutex::Descriptor
{
  pthread_mutex_t mutex;
};

void Mutex::lock() const
{
  hard_assert( descriptor != nullptr );

  pthread_mutex_lock( &descriptor->mutex );
}

bool Mutex::tryLock() const
{
  hard_assert( descriptor != nullptr );

  return pthread_mutex_trylock( &descriptor->mutex ) == 0;
}

void Mutex::unlock() const
{
  hard_assert( descriptor != nullptr );

  pthread_mutex_unlock( &descriptor->mutex );
}

void Mutex::init()
{
  hard_assert( descriptor == nullptr );

  descriptor = static_cast<Descriptor*>( malloc( sizeof( Descriptor ) ) );
  if( descriptor == nullptr ) {
    OZ_ERROR( "Mutex resource allocation failed" );
  }

  if( pthread_mutex_init( &descriptor->mutex, nullptr ) != 0 ) {
    OZ_ERROR( "Mutex initialisation failed" );
  }
}

void Mutex::destroy()
{
  hard_assert( descriptor != nullptr );

  pthread_mutex_destroy( &descriptor->mutex );

  free( descriptor );
  descriptor = nullptr;
}

}
