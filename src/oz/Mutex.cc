/*
 * liboz - OpenZone core library.
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

#ifdef _WIN32
# include "windefs.h"
# include <cstdlib>
# include <windows.h>
#else
# include <cstdlib>
# include <pthread.h>
#endif

namespace oz
{

#ifdef _WIN32

struct Mutex::Descriptor
{
  HANDLE mutex;
};

#else

struct Mutex::Descriptor
{
  pthread_mutex_t mutex;
};

#endif

void Mutex::lock() const
{
  hard_assert( descriptor != null );

#ifdef _WIN32
  WaitForSingleObject( descriptor->mutex, INFINITE );
#else
  pthread_mutex_lock( &descriptor->mutex );
#endif
}

bool Mutex::tryLock() const
{
  hard_assert( descriptor != null );

#ifdef _WIN32
  return WaitForSingleObject( descriptor->mutex, 0 ) == WAIT_OBJECT_0;
#else
  return pthread_mutex_trylock( &descriptor->mutex ) == 0;
#endif
}

void Mutex::unlock() const
{
  hard_assert( descriptor != null );

#ifdef _WIN32
  ReleaseMutex( descriptor->mutex );
#else
  pthread_mutex_unlock( &descriptor->mutex );
#endif
}

void Mutex::init()
{
  hard_assert( descriptor == null );

  void* descriptorPtr = malloc( sizeof( Descriptor ) );
  if( descriptorPtr == null ) {
    System::error( 0, "Mutex resource allocation failed" );
  }

  descriptor = new( descriptorPtr ) Descriptor();

#ifdef _WIN32

  descriptor->mutex = CreateMutex( null, false, null );
  if( descriptor->mutex == null ) {
    System::error( 0, "Mutex initialisation failed" );
  }

#else

  if( pthread_mutex_init( &descriptor->mutex, null ) != 0 ) {
    System::error( 0, "Mutex initialisation failed" );
  }

#endif
}

void Mutex::destroy()
{
  hard_assert( descriptor != null );

#ifdef _WIN32
  CloseHandle( descriptor->mutex );
#else
  pthread_mutex_destroy( &descriptor->mutex );
#endif

  descriptor->~Descriptor();
  free( descriptor );
  descriptor = null;
}

}
