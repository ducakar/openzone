/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2019 Davorin Učakar
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
#include "Pepper.hh"

#include <ctime>
#include <pthread.h>

namespace oz
{

namespace
{

const String               UNKNOWN_NAME = "UNKNOWN";
const String               MAIN_NAME    = "MAIN";
const pthread_t            MAIN_THREAD  = pthread_self();
thread_local const String* threadName   = &UNKNOWN_NAME;

timespec toTimespec(Duration duration)
{
  int64 ns = duration.ns() % 1000000000 + 1000000000;
  int64 s  = duration.ns() / 1000000000 - 1;

  return timespec{s + ns / 1000000000, long(ns % 1000000000)};
}

}

struct Thread::Descriptor
{
  pthread_t   thread;
  Main*       main;
  void*       data;
  const char* name;
  SpinLock    lock;

  OZ_INTERNAL
  static void* mainWrapper(void* handle);
};

void* Thread::Descriptor::mainWrapper(void* handle)
{
  Descriptor* descriptor = static_cast<Descriptor*>(handle);
  Main*       main       = descriptor->main;
  void*       data       = descriptor->data;
  String      name       = descriptor->name;

  descriptor->lock.unlock();

  threadName = &name;

#if defined(__native_client__)
  Semaphore localSemaphore;
  MainCall::localSemaphore_ = &localSemaphore;
#endif

  return main(data);
}

void Thread::create(const char* name, Main* main, void* data)
{
  descriptor_ = new Descriptor;
  descriptor_->main = main;
  descriptor_->data = data;
  descriptor_->name = name;
  descriptor_->lock.lock();

  if (pthread_create(&descriptor_->thread, nullptr, Descriptor::mainWrapper, descriptor_) != 0) {
    OZ_ERROR("oz::Thread: Thread creation failed");
  }
}

void Thread::sleepFor(Duration duration)
{
  timespec ts = toTimespec(duration);
# ifdef __native_client__
  nanosleep(&ts, nullptr);
# else
  clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, nullptr);
# endif
}

void Thread::sleepUntil(Instant<STEADY> instant)
{
  timespec ts = toTimespec(instant.fromEpoch());
# ifdef __native_client__
  nanosleep(&ts, nullptr);
# else
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, nullptr);
# endif
}

void Thread::sleepUntil(Instant<WALL> instant)
{
  timespec ts = toTimespec(instant.fromEpoch());
# ifdef __native_client__
  nanosleep(&ts, nullptr);
# else
  clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &ts, nullptr);
# endif
}

const String& Thread::name()
{
  return isMain() ? MAIN_NAME : *threadName;
}

bool Thread::isMain()
{
  return pthread_equal(pthread_self(), MAIN_THREAD) != 0;
}

Thread::~Thread()
{
  if (descriptor_ != nullptr) {
    join();
  }
}

Thread::Thread(Thread&& other) noexcept
  : Thread()
{
  swap(*this, other);
}

Thread& Thread::operator=(Thread&& other) noexcept
{
  swap(*this, other);
  return *this;
}

void Thread::detach()
{
  if (descriptor_ == nullptr) {
    OZ_ERROR("oz::Thread: Detaching invalid thread");
  }

  if (pthread_detach(descriptor_->thread) != 0) {
    OZ_ERROR("oz::Thread: Detach failed");
  }

  // Wait while the thread accesses name pointer and descriptor during its initialisation. We need
  // this to assure the descriptor is not freed while the new thread still accesses it.
  descriptor_->lock.lock();

  delete descriptor_;
  descriptor_ = nullptr;
}

void* Thread::join()
{
  if (descriptor_ == nullptr) {
    OZ_ERROR("oz::Thread: Joining invalid thread");
  }

  void* result = nullptr;

  if (pthread_join(descriptor_->thread, &result) != 0) {
    OZ_ERROR("oz::Thread: Join failed");
  }

  delete descriptor_;
  descriptor_ = nullptr;

  return result;
}

}
