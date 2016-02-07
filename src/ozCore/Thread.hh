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
 * @file ozCore/Thread.hh
 *
 * `Thread` class.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * %Thread.
 *
 * @sa `oz::SpinLock`, `oz::Mutex`, `oz::Semaphore`, `oz::CallOnce`
 */
class Thread
{
public:

  /// Maximum length for a thread name.
  static const int NAME_LENGTH = 15;

public:

  /// %Thread's main function type.
  typedef void Main(void* data);

private:

  struct Descriptor;

  Descriptor* descriptor = nullptr; ///< Internal thread descriptor.

public:

  /**
   * Get current thread's name.
   *
   * This method always returns "main" for the main thread and "" for other threads that were not
   * started through this class.
   */
  static const char* name();

  /**
   * True iff called from the main thread.
   */
  static bool isMain();

  /**
   * Create an empty instance, no thread is started.
   */
  Thread() = default;

  /**
   * Create and start a new joinable thread.
   *
   * A new joinable thread is started and attached to the `Thread` object that created it. `join()`
   * should be called later to ensure thread's termination and to release its resources or
   * `detach()` to detach the thread and automatically release its resources upon finishing.
   *
   * @note
   * On Android, the thread is registered at VM if `Java::vm()` returns a valid handle (i.e.
   * `JavaVM::AttachCurrentThread()` and `JavaVM::DetachCurrentThread()` are invoked).
   *
   * @param name thread name (copied to an internal buffer).
   * @param main pointer to the thread's main function.
   * @param data pointer to user data, passed to the thread's main function.
   */
  explicit Thread(const char* name, Main* main, void* data = nullptr);

  /**
   * Join started but not yet joined thread if present.
   */
  ~Thread();

  /**
   * Move constructor.
   */
  Thread(Thread&& t);

  /**
   * Move operator.
   */
  Thread& operator =(Thread&& t);

  /**
   * True iff a joinable thread has been started but not yet joined or detached.
   */
  OZ_ALWAYS_INLINE
  bool isValid() const
  {
    return descriptor != nullptr;
  }

  /**
   * Detach a joinable thread.
   */
  void detach();

  /**
   * Wait for a joinable thread to finish execution and release its resources.
   */
  void join();

};

}
