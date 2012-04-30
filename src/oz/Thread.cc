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
 *
 * Thread class.
 */

#include "Thread.hh"

#include <cstdlib>
#include <pthread.h>

namespace oz
{

struct MutexDesc
{
  pthread_mutex_t mutex;
};

struct BarrierDesc
{
  pthread_mutex_t mutex;
  pthread_cond_t  cond;
  volatile bool   isFinished;
};

struct ThreadDesc
{
  pthread_t thread;
};

void Mutex::init()
{
  if( descriptor == null ) {
    descriptor = static_cast<MutexDesc*>( malloc( sizeof( MutexDesc ) ) );

    pthread_mutex_init( &descriptor->mutex, null );
  }
}

void Mutex::free()
{
  if( descriptor != null ) {
    pthread_mutex_destroy( &descriptor->mutex );

    ::free( descriptor );
    descriptor = null;
  }
}

void Mutex::lock() const
{
  hard_assert( descriptor != null );

  pthread_mutex_lock( &descriptor->mutex );
}

void Mutex::unlock() const
{
  hard_assert( descriptor != null );

  pthread_mutex_unlock( &descriptor->mutex );
}

void Barrier::init()
{
  if( descriptor == null ) {
    descriptor = static_cast<BarrierDesc*>( malloc( sizeof( BarrierDesc ) ) );

    pthread_mutex_init( &descriptor->mutex, null );
    pthread_cond_init( &descriptor->cond, null );
    descriptor->isFinished = true;
  }
}

void Barrier::free()
{
  if( descriptor != null ) {
    pthread_cond_destroy( &descriptor->cond );
    pthread_mutex_destroy( &descriptor->mutex );

    ::free( descriptor );
    descriptor = null;
  }
}

void Barrier::begin() const
{
  hard_assert( descriptor != null );

  descriptor->isFinished = false;
}

void Barrier::finish() const
{
  hard_assert( descriptor != null );

  pthread_mutex_lock( &descriptor->mutex );
  descriptor->isFinished = true;
  pthread_cond_signal( &descriptor->cond );
  pthread_mutex_unlock( &descriptor->mutex );
}

void Barrier::wait() const
{
  hard_assert( descriptor != null );

  pthread_mutex_lock( &descriptor->mutex );
  if( !descriptor->isFinished ) {
    pthread_cond_wait( &descriptor->cond, &descriptor->mutex );
  }
  pthread_mutex_unlock( &descriptor->mutex );
}

Thread::~Thread()
{
  if( descriptor != null ) {
    join();
  }
}

void Thread::start( Main* main, void* param )
{
  hard_assert( descriptor == null );

  descriptor = static_cast<ThreadDesc*>( malloc( sizeof( ThreadDesc ) ) );
  pthread_create( &descriptor->thread, null, main, param );
}

void Thread::join( void** result )
{
  hard_assert( descriptor != null );

  pthread_join( descriptor->thread, result );

  ::free( descriptor );
  descriptor = null;
}

}
