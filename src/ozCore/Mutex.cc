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

#include "Mutex.hh"

#include "System.hh"

#include <pthread.h>

namespace oz
{

struct Mutex::Descriptor
{
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
};

Mutex::Mutex()
  : descriptor_(new Descriptor)
{}

Mutex::~Mutex()
{
  pthread_mutex_destroy(&descriptor_->mutex);
  delete descriptor_;
}

void Mutex::lock()
{
  pthread_mutex_lock(&descriptor_->mutex);
}

bool Mutex::tryLock()
{
  return pthread_mutex_trylock(&descriptor_->mutex) == 0;
}

void Mutex::unlock()
{
  pthread_mutex_unlock(&descriptor_->mutex);
}

}
