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
 * Besides the ordinary semaphore functions it also provides `waitAll()` and `tryWaitAll()` methods
 * that zero the counter so it can also be used as a binary semaphore.
 *
 * The counter is clamped to [0, maxValue].
 *
 * @note No error checks are performed on the underlying pthreads API so any abuse of this class
 * leads to undefined behaviour.
 *
 * @sa `oz::Monitor`
 */
class Semaphore
{
private:

  struct Descriptor;

  Descriptor* descriptor_; ///< Internal semaphore descriptor.

public:

  /**
   * Create semaphore with initial value 0.
   */
  Semaphore();

  /**
   * Create and initialise semaphore.
   */
  explicit Semaphore(uint initialValue);

  /**
   * Destroy semaphore.
   */
  ~Semaphore();

  /**
   * No copying.
   */
  Semaphore(const Semaphore&) = delete;

  /**
   * No moving.
   */
  Semaphore(Semaphore&&) noexcept = delete;

  /**
   * No copying.
   */
  Semaphore& operator=(const Semaphore&) = delete;

  /**
   * No moving.
   */
  Semaphore& operator=(Semaphore&&) noexcept = delete;

  /**
   * Get current counter value.
   */
  uint counter() const;

  /**
   * Atomically increment counter and signal a waiting thread.
   *
   * If counter needs to be increased past `UINT_MAX` this method fails with counter remaining
   * intact and no signal is sent to the waiting threads.
   *
   * @return Counter has been successfully increased.
   */
  bool post();

  /**
   * Wait until counter becomes positive. Then atomically decrement it and resume.
   */
  void wait();

  /**
   * Atomically check if counter is positive and decrement it if it is.
   *
   * @return True iff counter was decremented.
   */
  bool tryWait();

};

}
