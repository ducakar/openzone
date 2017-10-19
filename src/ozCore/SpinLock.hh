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
 * @file ozCore/SpinLock.hh
 *
 * `SpinLock` class.
 */

#pragma once

#include "Atomic.hh"

namespace oz
{

/**
 * Spin lock.
 *
 * @sa `oz::Monitor`, `oz::RWLock`
 */
class SpinLock
{
private:

  Atomic<bool> isLocked_ = {false}; ///< True iff locked.

public:

  /**
   * Create new instance.
   */
  SpinLock() = default;

  /**
   * Copying or moving is not possible.
   */
  SpinLock(const SpinLock&) = delete;

  /**
   * Copying or moving is not possible.
   */
  SpinLock& operator=(const SpinLock&) = delete;

  /**
   * Loop performing a lock operation until it succeeds.
   */
  void lock()
  {
    while (isLocked_.testAndSet<ACQUIRE>()) {
#if defined(__ARM_ACLE__)
      __builtin_arm_yield();
#elif defined(__i386__) || defined(__x86_64__)
      __builtin_ia32_pause();
#endif
    }
  }

  /**
   * Atomically lock and check if it has already been locked.
   *
   * @return True iff it was unlocked.
   */
  bool tryLock()
  {
    return !isLocked_.testAndSet<ACQUIRE>();
  }

  /**
   * Unlock.
   */
  void unlock()
  {
    isLocked_.clear<RELEASE>();
  }

};

}
