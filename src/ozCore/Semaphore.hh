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
 * Besides the ordinary semaphore functions it also provides `waitAll()` and `tryWaitAll()` methods
 * that zero the counter so it can also be used as a binary semaphore.
 *
 * The couter is clamped to [0, maxValue].
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
   * Create and initialise semaphore.
   */
  Semaphore(int initialValue = 0, int maxValue = INT_MAX);

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
  Semaphore& operator=(const Semaphore&) = delete;

  /**
   * Get current counter value.
   */
  int counter() const;

  /**
   * Atomically increment counter and signal waiting threads.
   *
   * If counter needs to be increased past `INT_MAX` this method fails with counter remaining intact
   * and no signals are sent to the waiting threads.
   *
   * @return Counter has been successfully increased.
   */
  bool post(int increment = 1);

  /**
   * Wait until counter becomes positive. Then atomically decrement it and resume.
   */
  void wait(int decrement = 1);

  /**
   * Wait until counter becomes positive. Then atomically zero it and resume.
   */
  void waitAll();

  /**
   * Atomically check if counter is positive and decrement it if it is.
   *
   * @return True iff counter was decremented.
   */
  bool tryWait(int decrement = 1);

  /**
   * Atomically check if counter is positive and zero it if it is.
   *
   * @return True iff counter was zeroed.
   */
  bool tryWaitAll();

};

}
