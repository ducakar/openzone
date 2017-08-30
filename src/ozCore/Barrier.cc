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

#include "Barrier.hh"

#include "System.hh"

#include <cstdlib>
#include <pthread.h>

namespace oz
{

struct Barrier::Descriptor
{
  pthread_barrier_t barrier;
};

Barrier::Barrier(int count)
{
  if (count <= 0) {
    OZ_ERROR("oz::Barrier: Count must be > 0");
  }

  descriptor_ = new(malloc(sizeof(Descriptor))) Descriptor;
  if (descriptor_ == nullptr) {
    OZ_ERROR("oz::Barrier: Descriptor initialisation failed");
  }

  pthread_barrier_init(&descriptor_->barrier, nullptr, count);
}

Barrier::~Barrier()
{
  pthread_barrier_destroy(&descriptor_->barrier);

  free(descriptor_);
}

void Barrier::wait()
{
  pthread_barrier_wait(&descriptor_->barrier);
}

}
