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
#include <cstring>

#ifdef _WIN32
# include <windows.h>
#else
# include <pthread.h>
#endif

namespace oz
{

static const int NAME_LENGTH = 15;

#ifdef _WIN32
static DWORD         nameKey;
#else
static pthread_key_t nameKey;
#endif
static bool          isNameKeyInitialised = false;

static void initNameKey()
{
#ifdef _WIN32
  nameKey = TlsAlloc();
  if( nameKey == TLS_OUT_OF_INDEXES ) {
    OZ_ERROR( "Thread name key creation failed" );
  }

  // This method is always invoked in static initialisation and hence in the main thread.
  TlsSetValue( nameKey, const_cast<char*>( "main" ) );
#else
  if( pthread_key_create( &nameKey, nullptr ) != 0 ) {
    OZ_ERROR( "Thread name key creation failed" );
  }

  // This method is always invoked in static initialisation and hence in the main thread.
  pthread_setspecific( nameKey, "main" );
#endif

  isNameKeyInitialised = true;
}

// Ensure that initNameKey is invoked during static initialisation.
static struct NameKeyInitialiser
{
  OZ_HIDDEN
  explicit NameKeyInitialiser()
  {
    initNameKey();
  }
}
nameKeyInitialiser;

struct Thread::Descriptor
{
#ifdef _WIN32
  HANDLE        thread;
#else
  pthread_t     thread;
#endif
  Thread::Main* main;
  void*         data;
  char          name[NAME_LENGTH + 1];

#ifdef _WIN32
  static DWORD WINAPI threadMain( void* data );
#else
  static void* threadMain( void* data );
#endif
};

OZ_HIDDEN
#ifdef _WIN32
DWORD WINAPI Thread::Descriptor::threadMain( void* data )
#else
void* Thread::Descriptor::threadMain( void* data )
#endif
{
  Descriptor* descriptor = static_cast<Descriptor*>( data );

#ifdef _WIN32
  TlsSetValue( nameKey, descriptor->name );
#else
  pthread_setspecific( nameKey, descriptor->name );
#endif

  // Set up signal handlers (if enabled).
  System::threadInit();

  descriptor->main( descriptor->data );

#ifdef _WIN32
  return 0;
#else
  return nullptr;
#endif
}

const char* Thread::name()
{
#ifdef _WIN32
  void* data = TlsGetValue( nameKey );
#else
  void* data = pthread_getspecific( nameKey );
#endif
  return static_cast<const char*>( data );
}

void Thread::start( const char* name, Main* main, void* data )
{
  hard_assert( descriptor == nullptr );

  if( !isNameKeyInitialised ) {
    initNameKey();
  }

  descriptor = static_cast<Descriptor*>( malloc( sizeof( Descriptor ) ) );
  if( descriptor == nullptr ) {
    OZ_ERROR( "Thread resource allocation failed" );
  }

  descriptor->main = main;
  descriptor->data = data;

  strncpy( descriptor->name, name, NAME_LENGTH );
  descriptor->name[NAME_LENGTH] = '\0';

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

void Thread::detach()
{
  hard_assert( descriptor != nullptr );

#ifdef _WIN32
#else

  if( pthread_detach( descriptor->thread ) != 0 ) {
    OZ_ERROR( "Thread detach failed" );
  }

#endif

  free( descriptor );
  descriptor = nullptr;
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
