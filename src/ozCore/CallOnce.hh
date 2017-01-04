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
 * @file ozCore/CallOnce.hh
 *
 * `CallOnce` class.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * Call a function only once, even if called from several threads.
 *
 * @sa `oz::SpinLock`, `oz::Mutex`, `oz::Semaphore`, `oz::Thread`
 */
class CallOnce
{
public:

  /// %Function type.
  typedef void Function();

private:

  volatile bool hasEntered_  = false; ///< Flipped to true when function starts executing.
  volatile bool hasFinished_ = false; ///< Flipped to true when function finishes.

public:

  /**
   * Create and initialise a single-use instance.
   */
  CallOnce() = default;

  /**
   * Copying or moving is not possible.
   */
  CallOnce(const CallOnce&) = delete;

  /**
   * Copying or moving is not possible.
   */
  CallOnce& operator=(const CallOnce&) = delete;

  /**
   * Call function if this is the first call on this object.
   *
   * The `call()` function only executes the given function first time it is called on this object.
   * Internal locking mechanism prevents race conditions when this is done from two threads
   * simultaneously.
   *
   * If `function` is null, this object is just set as used, as if it would have already called a
   * function.
   */
  void call(Function* function);

};

}
