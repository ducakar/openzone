/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
#include "StackTrace.hh"
#include "Java.hh"
#include "Pepper.hh"

#include <cstdlib>

#if defined(__ANDROID__)
# include <jni.h>
# include <pthread.h>
#elif defined(__native_client__)
# include <ppapi/cpp/message_loop.h>
# include <pthread.h>
#elif defined(_WIN32)
# include <windows.h>
#else
# include <pthread.h>
#endif

namespace oz
{

#ifdef _WIN32
static DWORD          nameKey;
static HANDLE         mainThread = GetCurrentThread();
#else
static pthread_key_t  nameKey;
static pthread_t      mainThread = pthread_self();
#endif

// Create thread name key and set main thread's name to "main" during static initialisation.
struct MainThreadNameInitialiser
{
  OZ_HIDDEN
  MainThreadNameInitialiser()
  {
#ifdef _WIN32

    nameKey = TlsAlloc();
    if (nameKey == TLS_OUT_OF_INDEXES) {
      OZ_ERROR("oz::Thread: Name key creation failed");
    }
    TlsSetValue(nameKey, const_cast<char*>("main"));

#else

    if (pthread_key_create(&nameKey, nullptr) != 0) {
      OZ_ERROR("oz::Thread: Name key creation failed");
    }
    pthread_setspecific(nameKey, "main");

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
  bool          isDetached;
  char          name[StackTrace::NAME_LENGTH + 1];

#ifdef _WIN32
  static DWORD WINAPI threadMain(void* data);
#else
  static void* threadMain(void* data);
#endif
};

#ifdef _WIN32

OZ_HIDDEN
DWORD WINAPI Thread::Descriptor::threadMain(void* data)
{
  Descriptor* descriptor = static_cast<Descriptor*>(data);

  TlsSetValue(nameKey, descriptor->name);

  System::threadInit();
  descriptor->main(descriptor->data);
  return 0;
}

#else

OZ_HIDDEN
void* Thread::Descriptor::threadMain(void* data)
{
  Descriptor* descriptor = static_cast<Descriptor*>(data);

  pthread_setspecific(nameKey, descriptor->name);

#if defined(__ANDROID__)

  JavaVM* javaVM = Java::vm();

  if (javaVM != nullptr) {
    void* jniEnv = nullptr;
    javaVM->AttachCurrentThread(&jniEnv, nullptr);
  }

#elif defined(__native_client__)

  pp::Instance* ppInstance = Pepper::instance();

  if (ppInstance == nullptr) {
    OZ_ERROR("oz::Thread: NaCl application instance must be created via oz::Pepper::createModule()"
             " before starting any new threads");
  }

  pp::MessageLoop messageLoop(ppInstance);
  messageLoop.AttachToCurrentThread();

  Semaphore localSemaphore;
  MainCall::localSemaphore = &localSemaphore;

#endif

  System::threadInit();
  descriptor->main(descriptor->data);

#ifdef __ANDROID__

  if (javaVM != nullptr) {
    javaVM->DetachCurrentThread();
  }

#endif

  if (descriptor->isDetached) {
    free(descriptor);
  }
  return nullptr;
}

#endif

const char* Thread::name()
{
#ifdef _WIN32
  void* data = TlsGetValue(nameKey);
#else
  void* data = pthread_getspecific(nameKey);
#endif
  return static_cast<const char*>(data == nullptr ? "" : data);
}

bool Thread::isMain()
{
#ifdef _WIN32
  return GetCurrentThread() == mainThread;
#else
  return pthread_equal(pthread_self(), mainThread);
#endif
}

Thread::Thread() :
  descriptor(nullptr)
{}

Thread::~Thread()
{
  if (descriptor != nullptr && !descriptor->isDetached) {
    join();
  }
}

Thread::Thread(Thread&& t) :
  descriptor(t.descriptor)
{
  t.descriptor = nullptr;
}

Thread& Thread::operator = (Thread&& t)
{
  if (&t != this) {
    descriptor = t.descriptor;

    t.descriptor = nullptr;
  }
  return *this;
}

void Thread::start(const char* name, Main* main, void* data)
{
  if (descriptor != nullptr) {
    OZ_ERROR("oz::Thread: Thread is already started");
  }

  descriptor = static_cast<Descriptor*>(malloc(sizeof(Descriptor)));
  if (descriptor == nullptr) {
    OZ_ERROR("oz::Thread: Descriptor allocation failed");
  }

  descriptor->main       = main;
  descriptor->data       = data;
  descriptor->isDetached = false;

  strlcpy(descriptor->name, name, StackTrace::NAME_LENGTH);

#ifdef _WIN32
  descriptor->thread = CreateThread(nullptr, 0, Descriptor::threadMain, descriptor, 0, nullptr);
  if (descriptor->thread == nullptr) {
    OZ_ERROR("oz::Thread: Thread creation failed");
  }
#else
  if (pthread_create(&descriptor->thread, nullptr, Descriptor::threadMain, descriptor) != 0) {
    OZ_ERROR("oz::Thread: Thread creation failed");
  }
#endif
}

void Thread::detach()
{
  if (descriptor == nullptr) {
    OZ_ERROR("oz::Thread: Detaching invalid thread");
  }

  descriptor->isDetached = true;
#ifdef _WIN32
  CloseHandle(descriptor->thread);
#else
  pthread_detach(descriptor->thread);
#endif

  descriptor = nullptr;
}

void Thread::join()
{
  if (descriptor == nullptr) {
    OZ_ERROR("oz::Thread: Detaching invalid thread");
  }

#ifdef _WIN32
  if (WaitForSingleObject(descriptor->thread, INFINITE) == WAIT_FAILED) {
    OZ_ERROR("oz::Thread: Join failed");
  }
  CloseHandle(descriptor->thread);
#else
  if (pthread_join(descriptor->thread, nullptr) != 0) {
    OZ_ERROR("oz::Thread: Join failed");
  }
#endif

  free(descriptor);
  descriptor = nullptr;
}

}
