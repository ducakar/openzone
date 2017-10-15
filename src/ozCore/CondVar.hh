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
 * @file ozCore/CondVar.hh
 *
 * `CondVar` class.
 */

#pragma once

#include "Mutex.hh"

namespace oz
{

/**
 * Condition variable.
 */
class CondVar
{
private:

  struct Descriptor;

  Descriptor* descriptor_; ///< Internal semaphore descriptor.

public:

  /**
   * Create and initialise condition variable.
   */
  CondVar();

  /**
   * Destroy condition variable.
   */
  ~CondVar();

  /**
   * Copying or moving is not possible.
   */
  CondVar(const CondVar&) = delete;

  /**
   * Copying or moving is not possible.
   */
  CondVar& operator=(const CondVar&) = delete;

  /**
   * Signal a single thread waiting on this condition variable.
   */
  void signal();

  /**
   * Signal all threads waiting on this condition variable.
   */
  void broadcast();

  /**
   * Wait on the condition variable.
   *
   * This function atomically blocks the current thread and releases the mutex. Upon a recieved
   * signal is the mutex is acquired again by the current thread and this function unblocks.
   *
   * @note Spurious wakeups are possible.
   */
  void wait(Mutex& mutex);

};

}
