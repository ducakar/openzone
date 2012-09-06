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

struct Thread::Descriptor
{
  HANDLE        thread;
  Thread::Main* main;
  void*         data;

  static DWORD WINAPI threadMain( void* data );
};

OZ_HIDDEN
DWORD WINAPI Thread::Descriptor::threadMain( void* data )
{
  Descriptor* descriptor = static_cast<Descriptor*>( data );

  descriptor->main( descriptor->data );
  return 0;
}

#else

struct Thread::Descriptor
{
  pthread_t     thread;
  Thread::Main* main;
  void*         data;

  static void* threadMain( void* data );
};

OZ_HIDDEN
void* Thread::Descriptor::threadMain( void* data )
{
  Descriptor* descriptor = static_cast<Descriptor*>( data );

  descriptor->main( descriptor->data );
  return nullptr;
}

#endif

void Thread::start( Main* main, void* data )
{
  hard_assert( descriptor == nullptr );

  descriptor = static_cast<Descriptor*>( malloc( sizeof( Descriptor ) ) );
  if( descriptor == nullptr ) {
    OZ_ERROR( "Thread resource allocation failed" );
  }

  descriptor->main = main;
  descriptor->data = data;

#ifdef _WIN32

  descriptor->thread = CreateThread( nullptr, 0, Descriptor::threadMain, descriptor, 0, nullptr );
  if( descriptor->thread == nullptr ) {
    OZ_ERROR( "Thread creation failed" );
  }

#else

  if( pthread_create( &descriptor->thread, nullptr, Descriptor::threadMain, descriptor ) != 0 ) {
    OZ_ERROR( "Thread creation failed" );
  }

#endif
}

void Thread::join()
{
  hard_assert( descriptor != nullptr );

#ifdef _WIN32

  WaitForSingleObject( descriptor->thread, INFINITE );
  CloseHandle( descriptor->thread );

#else

  if( pthread_join( descriptor->thread, nullptr ) != 0 ) {
    OZ_ERROR( "Thread join failed" );
  }

#endif

  free( descriptor );
  descriptor = nullptr;
}

}
