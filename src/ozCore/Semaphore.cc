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

#include "Semaphore.hh"

#include "System.hh"
#include "Atomic.hh"

#include <pthread.h>

namespace oz
{

struct Semaphore::Descriptor
{
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;
  Atomic<uint>    value;
};

Semaphore::Semaphore()
  : Semaphore(0)
{}

Semaphore::Semaphore(uint initialValue)
  : descriptor_(new Descriptor)
{
  descriptor_->value.value = initialValue;
}

Semaphore::~Semaphore()
{
  pthread_cond_destroy(&descriptor_->cond);
  pthread_mutex_destroy(&descriptor_->mutex);
  delete descriptor_;
}

uint Semaphore::counter() const
{
  return descriptor_->value.load<RELAXED>();
}

bool Semaphore::post()
{
  bool hasSucceeded = false;

  pthread_mutex_lock(&descriptor_->mutex);

  if (descriptor_->value.value != UINT_MAX) {
    ++descriptor_->value.value;
    hasSucceeded = true;
  }

  pthread_mutex_unlock(&descriptor_->mutex);

  if (hasSucceeded) {
    pthread_cond_signal(&descriptor_->cond);
  }
  return hasSucceeded;
}

void Semaphore::wait()
{
  pthread_mutex_lock(&descriptor_->mutex);

  while (descriptor_->value.value == 0) {
    pthread_cond_wait(&descriptor_->cond, &descriptor_->mutex);
  }
  --descriptor_->value.value;

  pthread_mutex_unlock(&descriptor_->mutex);
}

bool Semaphore::tryWait()
{
  bool hasSucceeded = false;

  if (descriptor_->value.load<ACQUIRE>() != 0) {
    pthread_mutex_lock(&descriptor_->mutex);

    if (descriptor_->value.value != 0) {
      --descriptor_->value.value;
      hasSucceeded = true;
    }

    pthread_mutex_unlock(&descriptor_->mutex);
  }

  return hasSucceeded;
}

}
