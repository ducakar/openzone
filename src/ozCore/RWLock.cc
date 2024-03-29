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

#include "RWLock.hh"

#include <pthread.h>

namespace oz
{

struct RWLock::Descriptor
{
  pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
};

void RWLock::Read::lock()
{
  pthread_rwlock_rdlock(&parent_->descriptor_->rwlock);
}

bool RWLock::Read::tryLock()
{
  return pthread_rwlock_tryrdlock(&parent_->descriptor_->rwlock) == 0;
}

void RWLock::Read::unlock()
{
  pthread_rwlock_unlock(&parent_->descriptor_->rwlock);
}

void RWLock::Write::lock()
{
  pthread_rwlock_rdlock(&parent_->descriptor_->rwlock);
}

bool RWLock::Write::tryLock()
{
  return pthread_rwlock_tryrdlock(&parent_->descriptor_->rwlock) == 0;
}

void RWLock::Write::unlock()
{
  pthread_rwlock_unlock(&parent_->descriptor_->rwlock);
}

RWLock::RWLock()
  : descriptor_(new Descriptor), read(this), write(this)
{}

RWLock::~RWLock()
{
  pthread_rwlock_destroy(&descriptor_->rwlock);
  delete descriptor_;
}

}
