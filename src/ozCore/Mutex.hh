/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file ozCore/Mutex.hh
 *
 * `Mutex` class.
 */

#pragma once

#ifndef EMSCRIPTEN

#include "common.hh"

namespace oz
{

/**
 * %Mutex.
 *
 * @sa `oz::SpinLock`, `oz::Semaphore`, `oz::CallOnce`, `oz::Thread`
 */
class Mutex
{
private:

  struct Descriptor;

  Descriptor* descriptor; ///< Internal mutex descriptor.

public:

  /**
   * Create uninitialised instance.
   */
  explicit Mutex();

  /**
   * Destructor, destroys mutex if initialised.
   */
  ~Mutex();

  /**
   * Move constructor.
   */
  Mutex( Mutex&& m );

  /**
   * Move operator.
   */
  Mutex& operator = ( Mutex&& m );

  /**
   * True iff initialised.
   */
  OZ_ALWAYS_INLINE
  bool isValid() const
  {
    return descriptor != nullptr;
  }

  /**
   * Wait until lock is obtained.
   *
   * @note
   * Locking a mutex that is already locked by the current thread results in undefined behaviour.
   */
  void lock() const;

  /**
   * Lock if not already locked.
   *
   * @note
   * Locking a mutex that is already locked by the current thread results in undefined behaviour.
   *
   * @return True on success.
   */
  bool tryLock() const;

  /**
   * Unlock.
   *
   * @note
   * Unlocking an unlocked mutex results in undefined behaviour.
   */
  void unlock() const;

  /**
   * Initialise mutex.
   *
   * Initialising an already initialised mutex is an error.
   */
  void init();

  /**
   * Destroy mutex and release resources.
   *
   * Destroying uninitialised mutex is a legal NOP.
   */
  void destroy();

};

}

#endif // EMSCRIPTEN
