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
 * @file oz/Semaphore.cc
 */

#include "Semaphore.hh"

#include "System.hh"

#include <cstdlib>

#ifdef _WIN32
# include <windows.h>
#else
# include <pthread.h>
#endif

namespace oz
{

#ifdef _WIN32

struct Semaphore::Descriptor
{
  HANDLE        semaphore;
  volatile long counter;
};

#else

struct Semaphore::Descriptor
{
  pthread_mutex_t mutex;
  pthread_cond_t  cond;
  volatile int    counter;
};

#endif

int Semaphore::counter() const
{
  hard_assert( descriptor != nullptr );

  return descriptor->counter;
}

void Semaphore::post() const
{
  hard_assert( descriptor != nullptr );

#ifdef _WIN32

  InterlockedIncrement( &descriptor->counter );
  ReleaseSemaphore( descriptor->semaphore, 1, nullptr );

#else

  pthread_mutex_lock( &descriptor->mutex );
  ++descriptor->counter;
  pthread_mutex_unlock( &descriptor->mutex );
  pthread_cond_signal( &descriptor->cond );

#endif
}

void Semaphore::wait() const
{
  hard_assert( descriptor != nullptr );

#ifdef _WIN32

  WaitForSingleObject( descriptor->semaphore, INFINITE );
  InterlockedDecrement( &descriptor->counter );

#else

  pthread_mutex_lock( &descriptor->mutex );
  while( descriptor->counter == 0 ) {
    pthread_cond_wait( &descriptor->cond, &descriptor->mutex );
  }
  --descriptor->counter;
  pthread_mutex_unlock( &descriptor->mutex );

#endif
}

bool Semaphore::tryWait() const
{
  hard_assert( descriptor != nullptr );

#ifdef _WIN32

  int ret = WaitForSingleObject( descriptor->semaphore, 0 );
  if( ret == WAIT_TIMEOUT ) {
    return false;
  }

  InterlockedDecrement( &descriptor->counter );
  return true;

#else

  bool hasSucceeded = false;

  pthread_mutex_lock( &descriptor->mutex );
  if( descriptor->counter != 0 ) {
    --descriptor->counter;
    hasSucceeded = true;
  }
  pthread_mutex_unlock( &descriptor->mutex );

  return hasSucceeded;

#endif
}

void Semaphore::init( int counter )
{
  hard_assert( descriptor == nullptr && counter >= 0 );

  descriptor = static_cast<Descriptor*>( malloc( sizeof( Descriptor ) ) );
  if( descriptor == nullptr ) {
    OZ_ERROR( "Semaphore resource allocation failed" );
  }

  descriptor->counter = counter;

#ifdef _WIN32

  descriptor->semaphore = CreateSemaphore( nullptr, counter, 0x7fffffff, nullptr );
  if( descriptor->semaphore == nullptr ) {
    OZ_ERROR( "Semaphore semaphore creation failed" );
  }

#else

  if( pthread_mutex_init( &descriptor->mutex, nullptr ) != 0 ) {
    OZ_ERROR( "Semaphore mutex creation failed" );
  }
  if( pthread_cond_init( &descriptor->cond, nullptr ) != 0 ) {
    OZ_ERROR( "Semaphore condition variable creation failed" );
  }

#endif
}

void Semaphore::destroy()
{
  hard_assert( descriptor != nullptr );

#ifdef _WIN32
  CloseHandle( &descriptor->semaphore );
#else
  pthread_cond_destroy( &descriptor->cond );
  pthread_mutex_destroy( &descriptor->mutex );
#endif

  free( descriptor );
  descriptor = nullptr;
}

}
