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

/**
 * @file ozCore/Barrier.hh
 *
 * `Barrier` class.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * %Barrier.
 *
 * Object for synchronising a given number of threads at a barrier. Calling `wait()` blocks the
 * thread until the number of waiting threads reaches the number givent in initialisation.
 */
class Barrier
{
private:

  struct Descriptor;

  Descriptor* descriptor_ = nullptr; ///< Internal semaphore descriptor.

public:

  /**
   * Create and initialise barrier.
   */
  Barrier(int count);

  /**
   * Destroy barrier.
   */
  ~Barrier();

  /**
   * Copying or moving is not possible.
   */
  Barrier(const Barrier&) = delete;

  /**
   * Copying or moving is not possible.
   */
  Barrier& operator=(const Barrier&) = delete;

  /**
   * Synchronise at barrier.
   *
   * Block until the number of waiting threads is reached. The barrier is reset to its initial state
   * upon unblocking.
   */
  void wait();

};

}
