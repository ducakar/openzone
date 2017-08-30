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

#include "Semaphore.hh"

#include "System.hh"
#include "Atomic.hh"

#include <cstdlib>
#include <pthread.h>

namespace oz
{

struct Semaphore::Descriptor
{
  pthread_mutex_t mutex   = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t  cond    = PTHREAD_COND_INITIALIZER;
  Atomic<int>     counter;
};

Semaphore::Semaphore(int value)
{
  if (value < 0) {
    OZ_ERROR("oz::Semaphore: Initial counter value must be >= 0");
  }

  descriptor_ = new(malloc(sizeof(Descriptor))) Descriptor;
  if (descriptor_ == nullptr) {
    OZ_ERROR("oz::Semaphore: Descriptor initialisation failed");
  }

  descriptor_->counter.value = value;
}

Semaphore::~Semaphore()
{
  pthread_cond_destroy(&descriptor_->cond);
  pthread_mutex_destroy(&descriptor_->mutex);

  free(descriptor_);
}

int Semaphore::counter() const
{
  return descriptor_->counter.load<ATOMIC_RELAXED>();
}

bool Semaphore::post(int increment)
{
  OZ_ASSERT(increment > 0);

  bool isSuccessful = false;

  pthread_mutex_lock(&descriptor_->mutex);

  if (descriptor_->counter.value != INT_MAX) {
    descriptor_->counter.value += increment;
    isSuccessful = true;
  }

  pthread_mutex_unlock(&descriptor_->mutex);

  if (isSuccessful) {
    if (increment == 1) {
      pthread_cond_signal(&descriptor_->cond);
    }
    else {
      pthread_cond_broadcast(&descriptor_->cond);
    }
  }
  return isSuccessful;
}

void Semaphore::wait(int decrement)
{
  OZ_ASSERT(decrement > 0);

  pthread_mutex_lock(&descriptor_->mutex);

  while (descriptor_->counter.value < decrement) {
    pthread_cond_wait(&descriptor_->cond, &descriptor_->mutex);
  }
  descriptor_->counter.value -= decrement;

  pthread_mutex_unlock(&descriptor_->mutex);
}

void Semaphore::waitAll()
{
  pthread_mutex_lock(&descriptor_->mutex);

  while (descriptor_->counter.value != 0) {
    pthread_cond_wait(&descriptor_->cond, &descriptor_->mutex);
  }
  descriptor_->counter.value = 0;

  pthread_mutex_unlock(&descriptor_->mutex);
}

bool Semaphore::tryWait(int decrement)
{
  OZ_ASSERT(decrement > 0);

  bool hasSucceeded = false;

  pthread_mutex_lock(&descriptor_->mutex);

  if (descriptor_->counter.value < decrement) {
    descriptor_->counter.value -= decrement;
    hasSucceeded = true;
  }

  pthread_mutex_unlock(&descriptor_->mutex);

  return hasSucceeded;
}

bool Semaphore::tryWaitAll()
{
  bool hasSucceeded = false;

  pthread_mutex_lock(&descriptor_->mutex);

  if (descriptor_->counter.value != 0) {
    descriptor_->counter.value = 0;
    hasSucceeded = true;
  }

  pthread_mutex_unlock(&descriptor_->mutex);

  return hasSucceeded;
}

}
