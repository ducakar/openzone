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
 * @file ozCore/CallOnce.hh
 *
 * `CallOnce` class.
 */

#pragma once

#include "Atomic.hh"
#include "LockGuard.hh"
#include "Monitor.hh"

namespace oz
{

/**
 * Call a function only once, even if called from several threads.
 */
class CallOnce
{
private:

  Monitor      lock_;                ///< The lock wrapping the call.
  Atomic<bool> wasCalled_ = {false}; ///< Flipped to true when the function finishes.

public:

  /**
   * Create and initialise a single-use instance.
   */
  CallOnce() = default;

  /**
   * Trivial destructor.
   */
  ~CallOnce() = default;

  /**
   * No copying.
   */
  CallOnce(const CallOnce&) = delete;

  /**
   * No moving.
   */
  CallOnce(CallOnce&&) = delete;

  /**
   * No copying.
   */
  CallOnce& operator=(const CallOnce&) = delete;

  /**
   * No moving.
   */
  CallOnce& operator=(CallOnce&&) = delete;

  /**
   * Call function if this is the first call on this object.
   *
   * This function is implemented as a typical double-checked lock and guarantees that the function
   * is called only once even when called simultaneously from two threads.
   */
  template <typename Function>
  void operator<<(Function function)
  {
    if (!wasCalled_.load<ACQUIRE>()) {
      LockGuard<Monitor> guard(&lock_);

      if (!wasCalled_.load<RELAXED>()) {
        function();
        wasCalled_.store<RELEASE>(true);
      }
    }
  }

};

}
