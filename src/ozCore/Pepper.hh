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
 * @file ozCore/Pepper.hh
 *
 * `Pepper` class.
 */

#pragma once

#include "String.hh"
#include "Semaphore.hh"
#include "Thread.hh"

#if defined(__native_client__) || defined(DOXYGEN_IGNORE)

/**
 * @def OZ_NACL_IS_MAIN
 * Check if on the main thread (debug mode only).
 */
# define OZ_NACL_IS_MAIN(boolean) \
  hard_assert(Thread::isMain() == boolean)

#else

# define OZ_NACL_IS_MAIN(boolean)

#endif

namespace oz
{

#if defined(__native_client__) || defined(DOXYGEN_IGNORE)

/**
 * High-level interface to NaCl %Pepper API (PPAPI).
 *
 * It provides NaCl module and instance creation and event main loop, facilities for invoking code
 * on the main thread, passing messages between application and JavaScript on the web page and
 * feedback information about module's geometry and mouse movement since the latter is not
 * implemented in the NaCl port of SDL.
 */
class Pepper
{
public:

  /**
   * PPAPI callback type.
   */
  typedef void Callback(void*, int);

public:

  /**
   * Forbid instances.
   */
  Pepper() = delete;

  /**
   * Execute asynchronous callback on the module's main thread.
   */
  static void mainCall(Callback* callback, void* data);

  /**
   * Post a message to JavaScript running on the page.
   */
  static void post(const char* message);

  /**
   * Pop next message from the incoming messages queue.
   */
  static String pop();

  /**
   * Push message to the incoming messages queue.
   */
  static void push(const char* message);

  /**
   * Initialise PPAPI interfaces required for messaging to work.
   */
  static void init();

};

#endif

/**
 * Utility for executing code blocks on the NaCl's main thread.
 *
 * Functions scheduled for the main thread should be lambda functions as those can (and often need
 * to) access local variables of a function on a non-main thread and are quick to implement.
 *
 * A typical scenario:
 * <code>
 * GLuint id = loadTexture(file);
 * MainCall() << [&]
 * {
 *   glBindTexture(GL_TEXTURE_2D, id);
 * };
 * </code>
 *
 * When used on the main thread or on a platform other than NaCl the function is executed
 * immediately on the caller's thread.
 */
class MainCall
{
#if defined(__native_client__) || defined(DOXYGEN_IGNORE)

  friend class Thread;

private:

  /**
   * Callback type.
   */
  typedef void Callback(void* data, int);

private:

  static thread_local Semaphore* localSemaphore; ///< Thread-local semaphore for synchronous calls.

#endif

public:

  /**
   * Call a function on the NaCl main thread synchronously.
   *
   * The function can also be a lambda expression with captures.
   *
   * On platforms other that NaCl the code is executed immediately on caller's thread.
   */
  template <typename Function>
  void operator << (Function function) const
  {
#ifdef __native_client__

    if (Thread::isMain()) {
      function();
    }
    else {
      struct CallbackWrapper
      {
        Function   function;
        Semaphore* semaphore;

        static void callback(void* data, int)
        {
          const CallbackWrapper* cw = static_cast<const CallbackWrapper*>(data);

          cw->function();
          cw->semaphore->post();
        }
      };
      CallbackWrapper cw = { function, localSemaphore };

      Pepper::mainCall(CallbackWrapper::callback, &cw);
      localSemaphore->wait();
    }

#else

    function();

#endif
  }

  /**
   * Call a function on the NaCl main thread asynchronously.
   *
   * The function can be a lambda expression but captures are discouraged for asynchronous calls as
   * local variables may change till the function is executed or the local stack may not even exist
   * any more.
   *
   * On platforms other that NaCl the code is executed immediately on caller's thread.
   */
  template <typename Function>
  void operator += (Function function) const
  {
#ifdef __native_client__

    struct CallbackWrapper
    {
      Function function;

      static void callback(void* data, int)
      {
        const CallbackWrapper* cw = static_cast<const CallbackWrapper*>(data);

        cw->function();
      }
    };
    CallbackWrapper cw = { function };

    Pepper::mainCall(CallbackWrapper::callback, &cw);

#else

    function();

#endif
  }

};

}
