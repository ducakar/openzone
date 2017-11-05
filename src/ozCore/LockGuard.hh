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
 * @file ozCore/LockGuard.hh
 *
 * `LockGuard` class.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * Lock guard.
 *
 * RAII wrapper for SpinLock, Monitor, RWLock::Read and RWLock::Write.
 *
 * @sa `oz::SpinLock`, `oz::Monitor`, `oz::RWLock`
 */
template <class Lock>
class LockGuard
{
private:

  Lock& lock_; ///< Wrapped lock.

public:

  /**
   * Acquire lock on construction.
   */
  explicit LockGuard(Lock& lock)
    : lock_(lock)
  {
    lock_.lock();
  }

  /**
   * Release lock on destruction.
   */
  ~LockGuard()
  {
    lock_.unlock();
  }

  /**
   * Copying or moving is not possible.
   */
  LockGuard(const LockGuard&) = delete;

  /**
   * Copying or moving is not possible.
   */
  LockGuard& operator=(const LockGuard&) = delete;

};

}
