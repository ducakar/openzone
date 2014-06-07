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

  /// %Thread's main function type.
  typedef void Main( void* data );

private:

  struct Descriptor;

  Descriptor* descriptor; ///< Internal thread descriptor.

public:

  /**
   * Get current thread's name.
   *
   * If the thread hasn't been started through this class, "" is returned or "main" if called on the
   * main thread.
   */
  static const char* name();

  /**
   * True iff called from the main thread.
   */
  static bool isMain();

  /**
   * Create and initialise new instance.
   */
  Thread();

  /**
   * Joins a started but not-yet-joined if thread if present and destroy instance.
   */
  ~Thread();

  /**
   * Copying or moving is not possible.
   */
  Thread( const Thread& ) = delete;

  /**
   * Copying or moving is not possible.
   */
  Thread& operator = ( const Thread& ) = delete;

  /**
   * True iff a joinable thread has been started but not yet joined or detached.
   */
  OZ_ALWAYS_INLINE
  bool isValid() const
  {
    return descriptor != nullptr;
  }

  /**
   * Create and start a new joinable thread.
   *
   * A new joinable thread is started and attached to the `Thread` object that created it. `join()`
   * should be called later to ensure thread's termination and to release its resources or
   * `detach()` to detach the thread and automatically release its resources upon finishing.
   *
   * After the thread has been either joined or detached the `Thread` object can be reused to start
   * another thread.
   *
   * @note
   * On Android, thread is registered at VM if `Java::vm()` returns a valid handle (i.e.
   * `JavaVM::AttachCurrentThread()` and `JavaVM::DetachCurrentThread()` are invoked).
   *
   * @param name thread name (copied to an internal buffer).
   * @param main pointer to the thread's main function.
   * @param data pointer to user data, passed to the thread's main function.
   */
  void start( const char* name, Main* main, void* data = nullptr );

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
