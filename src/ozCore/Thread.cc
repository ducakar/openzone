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

#ifndef EMSCRIPTEN

#include "Thread.hh"

#include "System.hh"
#include "StackTrace.hh"
#include "Pepper.hh"

#include <cstdlib>
#include <cstring>

#if defined( __ANDROID__ )
# include <jni.h>
# include <pthread.h>
#elif defined( __native_client__ )
# include <ppapi/cpp/message_loop.h>
# include <pthread.h>
#elif defined( _WIN32 )
# include <windows.h>
#else
# include <pthread.h>
#endif

namespace oz
{

#ifdef _WIN32
static DWORD          nameKey;
static volatile int   onceLock          = 0;
static volatile bool  isNameInitialised = false;
#else
static pthread_key_t  nameKey;
static pthread_once_t nameOnce = PTHREAD_ONCE_INIT;
#endif

static void initName()
{
#ifdef _WIN32

  if( isNameInitialised ) {
    return;
  }

  if( __sync_lock_test_and_set( &onceLock, 1 ) != 0 ) {
    while( onceLock != 0 );
  }
  else {
    if( !isNameInitialised ) {
      nameKey = TlsAlloc();

      if( nameKey == TLS_OUT_OF_INDEXES ) {
        OZ_ERROR( "oz::Thread: Name key creation failed" );
      }
      isNameInitialised = true;
    }
    __sync_lock_release( &onceLock );
  }

#else

  if( pthread_key_create( &nameKey, nullptr ) != 0 ) {
    OZ_ERROR( "oz::Thread: Name key creation failed" );
  }

#endif
}

// Set main thread name to "main" during static initialisation.
struct MainThreadNameInitialiser
{
  OZ_HIDDEN
  explicit MainThreadNameInitialiser()
  {
#ifdef _WIN32
    initName();
    TlsSetValue( nameKey, const_cast<char*>( "main" ) );
#else
    pthread_once( &nameOnce, initName );
    pthread_setspecific( nameKey, "main" );
#endif
  }
};

static MainThreadNameInitialiser mainThreadNameInitialiser;

struct Thread::Descriptor
{
#ifdef _WIN32
  HANDLE        thread;
#else
  pthread_t     thread;
#endif
  Thread::Main* main;
  void*         data;
  Type          type;
  char          name[StackTrace::NAME_LENGTH + 1];

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

  initName();
  TlsSetValue( nameKey, descriptor->name );

  System::threadInit();

  descriptor->main( descriptor->data );

  if( descriptor->type == DETACHED ) {
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
    OZ_ERROR( "oz::Thread: System::javaVM must be set before starting new threads" );
  }

  JNIEnv* jniEnv = nullptr;
  JavaVM* javaVM = static_cast<JavaVM*>( System::javaVM );

  javaVM->AttachCurrentThread( &jniEnv, nullptr );

#elif defined( __native_client__ )

  pp::Instance* ppInstance = Pepper::instance();

  if( ppInstance == nullptr ) {
    OZ_ERROR( "oz::Thread: NaCl application instance must be created via oz::Pepper::createModule()"
              " before starting any new threads" );
  }

  pp::MessageLoop messageLoop( ppInstance );
  messageLoop.AttachToCurrentThread();

#endif

  System::threadInit();
  descriptor->main( descriptor->data );

#ifdef __ANDROID__
  javaVM->DetachCurrentThread();
#endif

  if( descriptor->type == DETACHED ) {
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
  if( descriptor != nullptr ) {
    OZ_ERROR( "oz::Thread: Thread is already started" );
  }

  descriptor = static_cast<Descriptor*>( malloc( sizeof( Descriptor ) ) );
  if( descriptor == nullptr ) {
    OZ_ERROR( "oz::Thread: Descriptor allocation failed" );
  }

  descriptor->main = main;
  descriptor->data = data;
  descriptor->type = type;

  strncpy( descriptor->name, name, StackTrace::NAME_LENGTH );
  descriptor->name[StackTrace::NAME_LENGTH] = '\0';

#ifdef _WIN32

  descriptor->thread = CreateThread( nullptr, 0, Descriptor::threadMain, descriptor, 0, nullptr );
  if( descriptor->thread == nullptr ) {
    OZ_ERROR( "oz::Thread: Thread creation failed" );
  }

  if( type == DETACHED ) {
    CloseHandle( descriptor->thread );
  }

#else

  if( type == JOINABLE ) {
    if( pthread_create( &descriptor->thread, nullptr, Descriptor::threadMain, descriptor ) != 0 ) {
      OZ_ERROR( "oz::Thread: Thread creation failed" );
    }
  }
  else {
    pthread_attr_t attrib;

    pthread_attr_init( &attrib );
    pthread_attr_setdetachstate( &attrib, PTHREAD_CREATE_DETACHED );

    if( pthread_create( &descriptor->thread, &attrib, Descriptor::threadMain, descriptor ) != 0 ) {
      OZ_ERROR( "oz::Thread: Thread creation failed" );
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
    OZ_ERROR( "oz::Thread: Join failed" );
  }
#endif

  free( descriptor );
  descriptor = nullptr;
}

}

#endif // EMSCRIPTEN
