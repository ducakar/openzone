/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2016 Davorin Učakar
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
# include <ctime>
# include <jni.h>
# include <pthread.h>
#else
# include <ctime>
# include <pthread.h>
#endif

namespace oz
{

static const pthread_t          MAIN_THREAD = pthread_self();
static thread_local const char* threadName  = "";

struct Thread::Descriptor
{
  pthread_t   thread;
  const char* name;
  Main*       main;
  void*       data;
  SpinLock    lock;

  OZ_INTERNAL
  static void* mainWrapper(void* handle);
};

void* Thread::Descriptor::mainWrapper(void* handle)
{
  Descriptor* descriptor            = static_cast<Descriptor*>(handle);
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
  MainCall::localSemaphore_ = &localSemaphore;

#endif

  main(data);

#ifdef __ANDROID__
  if (javaVM != nullptr) {
    javaVM->DetachCurrentThread();
  }
#endif

  return nullptr;
}

void Thread::sleepFor(Duration duration)
{
  struct timespec ts = {time_t(duration.s()), long(duration.ns() % 1000000000)};
# ifdef __native_client__
  nanosleep(&ts, nullptr);
# else
  clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, nullptr);
# endif
}

void Thread::sleepUntil(Instant instant)
{
  struct timespec ts = {time_t(instant.s()), long(instant.ns() % 1000000000)};
# ifdef __native_client__
  nanosleep(&ts, nullptr);
# else
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, nullptr);
# endif
}

const char* Thread::name()
{
  return isMain() ? "MAIN" : threadName;
}

bool Thread::isMain()
{
  return pthread_equal(pthread_self(), MAIN_THREAD);
}

Thread::Thread(const char* name, Main* main, void* data)
{
  descriptor_ = new(malloc(sizeof(Descriptor))) Descriptor;
  if (descriptor_ == nullptr) {
    OZ_ERROR("oz::Thread: Descriptor allocation failed");
  }

  descriptor_->name = name;
  descriptor_->main = main;
  descriptor_->data = data;
  descriptor_->lock.lock();

  if (pthread_create(&descriptor_->thread, nullptr, Descriptor::mainWrapper, descriptor_) != 0) {
    OZ_ERROR("oz::Thread: Thread creation failed");
  }

  // Wait while the thread accesses name pointer and descriptor during its initialisation.
  descriptor_->lock.lock();
}

Thread::~Thread()
{
  if (descriptor_ != nullptr) {
    join();
  }
}

Thread::Thread(Thread&& other) noexcept
  : descriptor_(other.descriptor_)
{
  other.descriptor_ = nullptr;
}

Thread& Thread::operator=(Thread&& other) noexcept
{
  if (&other != this) {
    if (descriptor_ != nullptr) {
      join();
    }

    descriptor_ = other.descriptor_;

    other.descriptor_ = nullptr;
  }
  return *this;
}

void Thread::detach()
{
  if (descriptor_ == nullptr) {
    OZ_ERROR("oz::Thread: Detaching invalid thread");
  }

  pthread_detach(descriptor_->thread);

  free(descriptor_);
  descriptor_ = nullptr;
}

void Thread::join()
{
  if (descriptor_ == nullptr) {
    OZ_ERROR("oz::Thread: Joining invalid thread");
  }

  if (pthread_join(descriptor_->thread, nullptr) != 0) {
    OZ_ERROR("oz::Thread: Join failed");
  }

  free(descriptor_);
  descriptor_ = nullptr;
}

}
