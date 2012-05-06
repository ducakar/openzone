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
 * @file oz/Thread.cc
 */

#include "Thread.hh"

#include "Exception.hh"

#ifdef _WIN32
# include "windefs.h"
# include <windows.h>
# include <climits>
# include <cstdlib>
#else
# include <cstdlib>
# include <pthread.h>
#endif

namespace oz
{

#ifdef _WIN32

struct MutexDesc
{
  CRITICAL_SECTION mutex;
};

struct SemaphoreDesc
{
  HANDLE        semaphore;
  volatile long counter;
};

struct ThreadDesc
{
  HANDLE thread;
};

static DWORD WINAPI winMain( void* data )
{
  Thread::Main* main = *reinterpret_cast<Thread::Main**>( &data );
  main();
  return 0;
}

#else

struct MutexDesc
{
  pthread_mutex_t mutex;
};

struct SemaphoreDesc
{
  pthread_mutex_t mutex;
  pthread_cond_t  cond;
  volatile int    counter;
};

struct ThreadDesc
{
  pthread_t thread;
};

static void* pthreadMain( void* data )
{
  Thread::Main* main = *reinterpret_cast<Thread::Main**>( &data );
  main();
  return null;
}

#endif

void Mutex::init()
{
  hard_assert( descriptor == null );

  descriptor = static_cast<MutexDesc*>( malloc( sizeof( MutexDesc ) ) );
  if( descriptor == null ) {
    throw Exception( "Mutex resource allocation failed" );
  }

#ifdef _WIN32
  InitializeCriticalSectionAndSpinCount( &descriptor->mutex, 2000 );
#else
  if( pthread_mutex_init( &descriptor->mutex, null ) != 0 ) {
    free( descriptor );
    throw Exception( "Mutex initialisation failed" );
  }
#endif
}

void Mutex::destroy()
{
  hard_assert( descriptor != null );

#ifdef _WIN32
  DeleteCriticalSection( &descriptor->mutex );
#else
  pthread_mutex_destroy( &descriptor->mutex );
#endif

  free( descriptor );
  descriptor = null;
}

void Mutex::lock() const
{
  hard_assert( descriptor != null );

#ifdef _WIN32
  EnterCriticalSection( &descriptor->mutex );
#else
  pthread_mutex_lock( &descriptor->mutex );
#endif
}

bool Mutex::tryLock() const
{
  hard_assert( descriptor != null );

#ifdef _WIN32
  EnterCriticalSection( &descriptor->mutex );
#else
  return pthread_mutex_trylock( &descriptor->mutex ) == 0;
#endif
}

void Mutex::unlock() const
{
  hard_assert( descriptor != null );

#ifdef _WIN32
  LeaveCriticalSection( &descriptor->mutex );
#else
  pthread_mutex_unlock( &descriptor->mutex );
#endif
}

void Semaphore::init( int counterValue )
{
  hard_assert( descriptor == null && counterValue >= 0 );

  descriptor = static_cast<SemaphoreDesc*>( malloc( sizeof( SemaphoreDesc ) ) );
  if( descriptor == null ) {
    throw Exception( "Semaphore resource allocation failed" );
  }

#ifdef _WIN32

  descriptor->semaphore = CreateSemaphore( null, counter, 32 * 1024, null );
  if( descriptor->semaphore == null ) {
    free( descriptor );
    throw Exception( "Semaphore semaphore creation failed" );
  }

#else

  if( pthread_mutex_init( &descriptor->mutex, null ) != 0 ) {
    free( descriptor );
    throw Exception( "Semaphore mutex creation failed" );
  }
  if( pthread_cond_init( &descriptor->cond, null ) != 0 ) {
    pthread_mutex_destroy( &descriptor->mutex );
    free( descriptor );
    throw Exception( "Semaphore condition variable creation failed" );
  }

#endif

  descriptor->counter = counterValue;
}

void Semaphore::destroy()
{
  hard_assert( descriptor != null );

#ifdef _WIN32
  CloseHandle( &descriptor->semaphore );
#else
  pthread_cond_destroy( &descriptor->cond );
  pthread_mutex_destroy( &descriptor->mutex );
#endif

  free( descriptor );
  descriptor = null;
}

void Semaphore::post() const
{
  hard_assert( descriptor != null );

#ifdef _WIN32

  InterlockedIncrement( &descriptor->counter );
  ReleaseSemaphore( &descriptor->semaphore, 1, null );

#else

  pthread_mutex_lock( &descriptor->mutex );
  ++descriptor->counter;
  pthread_mutex_unlock( &descriptor->mutex );
  pthread_cond_signal( &descriptor->cond );

#endif
}

void Semaphore::wait() const
{
  hard_assert( descriptor != null );

#ifdef _WIN32

  if( WaitForSingleObject( descriptor->semaphore, INFINITE ) != WAIT_OBJECT_0 ) {
    throw Exception( "Semaphore wait failed" );
  }
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
  hard_assert( descriptor != null );

#ifdef _WIN32

  int ret = WaitForSingleObject( descriptor->semaphore, 0 );
  if( ret == WAIT_TIMEOUT ) {
    return false;
  }
  else if( ret != WAIT_OBJECT_0 ) {
    throw Exception( "Barrier semaphore wait failed" );
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

void Thread::start( Main* main )
{
  hard_assert( descriptor == null );

  descriptor = static_cast<ThreadDesc*>( malloc( sizeof( ThreadDesc ) ) );
  if( descriptor == null ) {
    throw Exception( "Thread resource allocation failed" );
  }

#ifdef _WIN32
  descriptor->thread = CreateThread( null, 0, winMain, *reinterpret_cast<void**>( &main ), 0,
                                     null );
  if( descriptor->thread == null ) {
    throw Exception( "Thread creation failed" );
  }
#else
  if( pthread_create( &descriptor->thread, null, pthreadMain,
                      *reinterpret_cast<void**>( &main ) ) != 0 )
  {
    throw Exception( "Thread creation failed" );
  }
#endif
}

void Thread::join()
{
  hard_assert( descriptor != null );

#ifdef _WIN32
  WaitForSingleObject( descriptor->thread, INFINITE );
  CloseHandle( descriptor->thread );
#else
  if( pthread_join( descriptor->thread, null ) != 0 ) {
    free( descriptor );
    descriptor = null;
    throw Exception( "Thread join failed" );
  }
#endif

  free( descriptor );
  descriptor = null;
}

}
