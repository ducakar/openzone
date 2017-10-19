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
 * @file ozCore/Monitor.hh
 *
 * `Monitor` class.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * Bundle of mutex and corresponding condition variable.
 *
 * Decoupling condition variable from a mutex is error-prone since using condition variable with
 * different mutexes at the same time leads to undefined behaviour according to pthreads. The only
 * benefit is that we might "reuse" the condition variable with a different mutex at some later time
 * and thus avoid initialising a new one. However, this may also be considered a bad programming
 * practice.
 *
 * Lazy initialisation is used for mutex and condition variable, so there is no penalty if one does
 * not use the condition variable.
 *
 * @note No error checks are performed on the underlying pthreads API so any abuse of this class
 * leads to undefined behaviour.
 *
 * @sa `oz::SpinLock`, `oz::RWLock`, `oz::Semaphore`
 */
class Monitor
{
private:

  struct Descriptor;

  Descriptor* descriptor_; ///< Internal monitor descriptor.

public:

  /**
   * Create monitor.
   */
  Monitor();

  /**
   * Destroy monitor.
   */
  ~Monitor();

  /**
   * Copying or moving is not possible.
   */
  Monitor(const Monitor&) = delete;

  /**
   * Copying or moving is not possible.
   */
  Monitor& operator=(const Monitor&) = delete;

  /**
   * Wait until mutex lock is obtained.
   *
   * @note
   * Locking a mutex that is already locked by the current thread results in undefined behaviour.
   */
  void lock();

  /**
   * Lock mutex if not already locked.
   *
   * @note
   * Locking a mutex that is already locked by the current thread results in undefined behaviour.
   *
   * @return True on success.
   */
  bool tryLock();

  /**
   * Unlock mutex.
   *
   * @note
   * Unlocking an unlocked mutex results in undefined behaviour.
   */
  void unlock();

  /**
   * Signal a single thread waiting on the condition variable.
   */
  void signal();

  /**
   * Signal all threads waiting on the condition variable.
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
  void wait();

};

}
