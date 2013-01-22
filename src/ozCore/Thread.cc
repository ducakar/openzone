/*
 * ozCore - OpenZone Core Library.
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
 * @file ozCore/Thread.cc
 */

#include "Thread.hh"

#include "System.hh"

#include <cstdlib>
#include <cstring>

#if defined( __ANDROID__ )
# include <jni.h>
# include <pthread.h>
// #elif defined( __native_client__ )
// # include <ppapi/cpp/instance_handle.h>
// # include <ppapi/cpp/dev/message_loop_dev.h>
// # include <pthread.h>
#elif defined( _WIN32 )
# include <windows.h>
#else
# include <pthread.h>
#endif

namespace oz
{

static const int NAME_LENGTH = 15;

#ifdef _WIN32
static DWORD          nameKey;
static volatile bool  nameOnce          = false;
static volatile bool  isNameInitialised = false;
#else
static pthread_key_t  nameKey;
static pthread_once_t nameOnce = PTHREAD_ONCE_INIT;
#endif

static void initName()
{
#ifdef _WIN32

  nameKey = TlsAlloc();
  if( nameKey == TLS_OUT_OF_INDEXES ) {
    OZ_ERROR( "Thread name key creation failed" );
  }

#else

  if( pthread_key_create( &nameKey, nullptr ) != 0 ) {
    OZ_ERROR( "Thread name key creation failed" );
  }

#endif
}

// Set main thread name to "main" during static initialisation.
static struct MainThreadNameInitialiser
{
  OZ_HIDDEN
  explicit MainThreadNameInitialiser()
  {
#ifdef _WIN32

    if( !isNameInitialised ) {
      if( __sync_lock_test_and_set( &nameOnce, true ) ) {
        while( !isNameInitialised );
      }
      else {
        initName();
        isNameInitialised = true;
      }
    }

    TlsSetValue( nameKey, const_cast<char*>( "main" ) );

#else

    pthread_once( &nameOnce, initName );
    pthread_setspecific( nameKey, "main" );

#endif
  }
}
mainThreadNameInitialiser;

struct Thread::Descriptor
{
#ifdef _WIN32
  HANDLE        thread;
#else
  pthread_t     thread;
#endif
  Thread::Main* main;
  void*         data;
  bool          isDetached;
  char          name[NAME_LENGTH + 1];

#ifdef _WIN32
  static DWORD WINAPI threadMain( void* data );
#else
  static void* threadMain( void* data );
#endif
};

#ifdef _WIN32

OZ_HIDDEN
DWORD WINAPI Thread::Descriptor::threadMain( void* data )
{
  Descriptor* descriptor = static_cast<Descriptor*>( data );

  if( !isNameInitialised ) {
    if( __sync_lock_test_and_set( &nameOnce, true ) ) {
      while( !isNameInitialised );
    }
    else {
      __sync_synchronize();

      initName();
      isNameInitialised = true;
    }
  }
  TlsSetValue( nameKey, descriptor->name );

  System::threadInit();

  descriptor->main( descriptor->data );

  if( descriptor->isDetached ) {
    CloseHandle( descriptor->thread );
  }
  return 0;
}

#else

OZ_HIDDEN
void* Thread::Descriptor::threadMain( void* data )
{
  Descriptor* descriptor = static_cast<Descriptor*>( data );

  pthread_once( &nameOnce, initName );
  pthread_setspecific( nameKey, descriptor->name );

#if defined( __ANDROID__ )
  if( System::javaVM == nullptr ) {
    OZ_ERROR( "System::javaVM must be set before starting new threads" );
  }

  JNIEnv* jniEnv = nullptr;
  JavaVM* javaVM = static_cast<JavaVM*>( System::javaVM );

  javaVM->AttachCurrentThread( &jniEnv, nullptr );
#elif defined( __native_client__ )
//   if( System::javaVM == nullptr ) {
//     OZ_ERROR( "System::instance must be set before starting new threads" );
//   }
//
//   pp::MessageLoop_Dev messageLoop( *System::instance );
//   messageLoop.AttachToCurrentThread();
#endif

  System::threadInit();

#ifdef __ANDROID__
  javaVM->DetachCurrentThread();
#endif

  descriptor->main( descriptor->data );

  if( descriptor->isDetached ) {
    free( descriptor );
  }
  return nullptr;
}

#endif

const char* Thread::name()
{
#ifdef _WIN32
  void* data = TlsGetValue( nameKey );
#else
  void* data = pthread_getspecific( nameKey );
#endif
  return static_cast<const char*>( data );
}

void Thread::start( const char* name, Type type, Main* main, void* data )
{
  hard_assert( descriptor == nullptr );

  descriptor = static_cast<Descriptor*>( malloc( sizeof( Descriptor ) ) );
  if( descriptor == nullptr ) {
    OZ_ERROR( "Thread resource allocation failed" );
  }

  descriptor->main       = main;
  descriptor->data       = data;
  descriptor->isDetached = type != JOINABLE;

  strncpy( descriptor->name, name, NAME_LENGTH );
  descriptor->name[NAME_LENGTH] = '\0';

#ifdef _WIN32

  descriptor->thread = CreateThread( nullptr, 0, Descriptor::threadMain, descriptor, 0, nullptr );
  if( descriptor->thread == nullptr ) {
    OZ_ERROR( "Thread creation failed" );
  }

  if( type != JOINABLE ) {
    CloseHandle( descriptor->thread );
  }

#else

  if( type == JOINABLE ) {
    if( pthread_create( &descriptor->thread, nullptr, Descriptor::threadMain, descriptor ) != 0 ) {
      OZ_ERROR( "Thread creation failed" );
    }
  }
  else {
    pthread_attr_t  attrib;

    pthread_attr_init( &attrib );
    pthread_attr_setdetachstate( &attrib, PTHREAD_CREATE_DETACHED );

    if( pthread_create( &descriptor->thread, &attrib, Descriptor::threadMain, descriptor ) != 0 ) {
      OZ_ERROR( "Thread creation failed" );
    }

    pthread_attr_destroy( &attrib );
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
