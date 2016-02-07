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
 * @file ozCore/Semaphore.hh
 *
 * `Semaphore` class.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * %Semaphore.
 *
 * It is implemented as a wrapper for condition variable if a platform supports it since it should
 * yield a better performance.
 *
 * @sa `oz::SpinLock`, `oz::Mutex`, `oz::CallOnce`, `oz::Thread`
 */
class Semaphore
{
private:

  struct Descriptor;

  Descriptor* descriptor = nullptr; ///< Internal semaphore descriptor.

public:

  /**
   * Create and initialise semaphore.
   */
  Semaphore();

  /**
   * Destroy semaphore.
   */
  ~Semaphore();

  /**
   * Copying or moving is not possible.
   */
  Semaphore(const Semaphore&) = delete;

  /**
   * Copying or moving is not possible.
   */
  Semaphore& operator =(const Semaphore&) = delete;

  /**
   * Get current counter value.
   */
  int counter() const;

  /**
   * Atomically increment counter and signal waiting threads.
   */
  void post() const;

  /**
   * Wait until counter becomes positive. Then atomically decrement it and resume.
   */
  void wait() const;

  /**
   * Atomically check if counter is positive and decrement it if it is.
   *
   * @return True iff counter was decremented.
   */
  bool tryWait() const;

};

}
