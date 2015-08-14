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

#include "Thread.hh"

#include "SpinLock.hh"
#include "Java.hh"
#include "Pepper.hh"

#include <cstdlib>
#include <cstring>

#if defined(__ANDROID__)
# include <jni.h>
# include <pthread.h>
#elif defined(_WIN32)
# include <windows.h>
#else
# include <pthread.h>
#endif

namespace oz
{

#ifdef _WIN32
static const HANDLE             MAIN_THREAD = GetCurrentThread();
#else
static const pthread_t          MAIN_THREAD = pthread_self();
#endif
static thread_local const char* threadName  = "";

struct Thread::Descriptor
{
#ifdef _WIN32
  HANDLE      thread;
#else
  pthread_t   thread;
#endif
  const char* name;
  Main*       main;
  void*       data;
  SpinLock    lock;

#ifdef _WIN32
  static DWORD WINAPI threadMain(void* descriptor);
#else
  static void* threadMain(void* descriptor);
#endif
};

OZ_INTERNAL
#ifdef _WIN32
DWORD WINAPI Thread::Descriptor::threadMain(void* descriptor_)
#else
void* Thread::Descriptor::threadMain(void* descriptor_)
#endif
{
  Descriptor* descriptor            = static_cast<Descriptor*>(descriptor_);
  Main*       main                  = descriptor->main;
  void*       data                  = descriptor->data;
  char        name[NAME_LENGTH + 1];

  if (descriptor->name != nullptr) {
    strncpy(name, descriptor->name, NAME_LENGTH);
    name[NAME_LENGTH] = '\0';

    threadName = name;
  }

  descriptor->lock.unlock();

#if defined(__ANDROID__)

  JavaVM* javaVM = Java::vm();

  if (javaVM != nullptr) {
    JNIEnv* jniEnv = nullptr;
    javaVM->AttachCurrentThread(&jniEnv, nullptr);
  }

#elif defined(__native_client__)

  Semaphore localSemaphore;
  MainCall::localSemaphore = &localSemaphore;

#endif

  main(data);

#ifdef __ANDROID__
  if (javaVM != nullptr) {
    javaVM->DetachCurrentThread();
  }
#endif

#ifdef _WIN32
  return 0;
#else
  return nullptr;
#endif
}

const char* Thread::name()
{
  return isMain() ? "MAIN" : threadName;
}

bool Thread::isMain()
{
#ifdef _WIN32
  return GetCurrentThread() == MAIN_THREAD;
#else
  return pthread_equal(pthread_self(), MAIN_THREAD);
#endif
}

Thread::Thread(const char* name, Main* main, void* data)
{
  if (descriptor != nullptr) {
    OZ_ERROR("oz::Thread: Thread is already started");
  }

  descriptor = new(malloc(sizeof(Descriptor))) Descriptor;
  if (descriptor == nullptr) {
    OZ_ERROR("oz::Thread: Descriptor allocation failed");
  }

  descriptor->name = name;
  descriptor->main = main;
  descriptor->data = data;
  descriptor->lock.lock();

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

  // Wait while the thread accesses name pointer and descriptor during its initialisation.
  descriptor->lock.lock();
}

Thread::~Thread()
{
  if (descriptor != nullptr) {
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
    if (descriptor != nullptr) {
      join();
    }

    descriptor = t.descriptor;

    t.descriptor = nullptr;
  }
  return *this;
}

void Thread::detach()
{
  if (descriptor == nullptr) {
    OZ_ERROR("oz::Thread: Detaching invalid thread");
  }

#ifdef _WIN32
  CloseHandle(descriptor->thread);
#else
  pthread_detach(descriptor->thread);
#endif

  free(descriptor);
  descriptor = nullptr;
}

void Thread::join()
{
  if (descriptor == nullptr) {
    OZ_ERROR("oz::Thread: Joining invalid thread");
  }

#ifdef _WIN32
  if (WaitForSingleObject(descriptor->thread, INFINITE) != WAIT_OBJECT_0) {
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
