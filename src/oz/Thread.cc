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
 * @file oz/Thread.cc
 */

#include "Thread.hh"

#include "System.hh"

#include <cstdlib>
#include <cstring>
#include <pthread.h>

namespace oz
{

static const int NAME_LENGTH = 15;

static pthread_key_t  nameKey;
static pthread_once_t nameOnce = PTHREAD_ONCE_INIT;

static void initName()
{
  if( pthread_key_create( &nameKey, nullptr ) != 0 ) {
    OZ_ERROR( "Thread name key creation failed" );
  }
}

// Set main thread name to "main" during static initialisation.
static struct MainThreadNameInitialiser
{
  OZ_HIDDEN
  explicit MainThreadNameInitialiser()
  {
    pthread_once( &nameOnce, &initName );
    pthread_setspecific( nameKey, "main" );
  }
}
mainThreadNameInitialiser;

struct Thread::Descriptor
{
  pthread_t     thread;
  Thread::Main* main;
  void*         data;
  char          name[NAME_LENGTH + 1];

  static void* threadMain( void* data );
};

OZ_HIDDEN
void* Thread::Descriptor::threadMain( void* data )
{
  Descriptor* descriptor = static_cast<Descriptor*>( data );

  pthread_once( &nameOnce, &initName );
  pthread_setspecific( nameKey, descriptor->name );

  System::threadInit();

  descriptor->main( descriptor->data );
  return nullptr;
}

const char* Thread::name()
{
  void* data = pthread_getspecific( nameKey );
  return static_cast<const char*>( data );
}

void Thread::start( const char* name, Main* main, void* data )
{
  hard_assert( descriptor == nullptr );

  descriptor = static_cast<Descriptor*>( malloc( sizeof( Descriptor ) ) );
  if( descriptor == nullptr ) {
    OZ_ERROR( "Thread resource allocation failed" );
  }

  descriptor->main = main;
  descriptor->data = data;

  strncpy( descriptor->name, name, NAME_LENGTH );
  descriptor->name[NAME_LENGTH] = '\0';

  if( pthread_create( &descriptor->thread, nullptr, Descriptor::threadMain, descriptor ) != 0 ) {
    OZ_ERROR( "Thread creation failed" );
  }
}

void Thread::detach()
{
  hard_assert( descriptor != nullptr );

  if( pthread_detach( descriptor->thread ) != 0 ) {
    OZ_ERROR( "Thread detach failed" );
  }

  free( descriptor );
  descriptor = nullptr;
}

void Thread::join()
{
  hard_assert( descriptor != nullptr );

  if( pthread_join( descriptor->thread, nullptr ) != 0 ) {
    OZ_ERROR( "Thread join failed" );
  }

  free( descriptor );
  descriptor = nullptr;
}

}
