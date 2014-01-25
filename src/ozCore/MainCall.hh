/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file ozCore/MainCall.hh
 *
 * `MainCall` class.
 */

#pragma once

#if 0

#include "Semaphore.hh"

namespace oz
{

/**
 * Utility for executing code blocks on the main thread.
 *
 * Methods schedulled for the main thread would typically be lambda functions as those often need to
 * access local variables of a function on a non-main thread and are quick to implement.
 *
 * A typical scenario:
 * <code>
 * GLuint id = loadTexture( file );
 * MainCall() << [&]()
 * {
 *   glBindTexture( GL_TEXTURE_2D, id );
 * };
 * </code>
 *
 * This mechanism hangs up the application if used from the main thread.
 */
class MainCall
{
private:

  /**
   * Callback type.
   */
#ifdef __native_client__
  typedef void Callback( void* data, int );
#else
  typedef void Callback( void* data );
#endif

  /**
   * Semaphore wrapper that constructs and destructs it with a thread.
   */
  struct LocalSemaphore
  {
    Semaphore sem; ///< Wrapped semaphore.

    LocalSemaphore();
    ~LocalSemaphore();
  };

private:

#ifndef __native_client__
  static thread_local LocalSemaphore localSemaphore; ///< Semaphore for synchronous calls.
#endif

private:

  /**
   * Schedule a callback for execution on the main thread.
   */
  static bool schedule( Callback* callback, void* data );

public:

  /**
   * Call a method on the main thread synchronously.
   *
   * The method can also be a lambda expression with captures. In case the method queue is full
   * false is returned.
   */
  template <typename Method>
  bool operator << ( Method method ) const
  {
    struct CallbackWrapper
    {
      Method     method;
      Semaphore* semaphore;

#ifdef __native_client__
      static void callback( void* data, int )
#else
      static void callback( void* data )
#endif
      {
        const CallbackWrapper* cw = static_cast<const CallbackWrapper*>( data );

        cw->method();
        cw->semaphore->post();
      }
    };
    CallbackWrapper cw = { method, &localSemaphore.sem };

    if( !schedule( CallbackWrapper::callback, &cw ) ) {
      return false;
    }
    localSemaphore.sem.wait();
    return true;
  }

  /**
   * Call a method on the main thread asynchronously.
   *
   * The method can also be a lambda expression with captures. In case the method queue is full
   * false is returned.
   */
  template <typename Method>
  bool operator += ( Method method ) const
  {
    struct CallbackWrapper
    {
      Method method;

#ifdef __native_client__
      static void callback( void* data, int )
#else
      static void callback( void* data )
#endif
      {
        const CallbackWrapper* cw = static_cast<const CallbackWrapper*>( data );

        cw->method();
      }
    };
    CallbackWrapper cw = { method };

    return schedule( CallbackWrapper::callback, &cw );
  }

  /**
   * Wait until the main thread executes all scheduled closures.
   */
  static void wait();

  /**
   * Same as `wait()`, plus terminate the loop executing closures in the main thread.
   */
  static void terminate();

  /**
   * Run loop that executes closures sent by other threads.
   */
  static void loop();

  /**
   * Initialise class.
   */
  static void init();

  /**
   * Destroy class.
   */
  static void destroy();

};

}

#else

namespace oz
{

class MainCall
{
public:

  template <typename Method>
  bool operator << ( Method method ) const
  {
    method();
    return true;
  }

  template <typename Method>
  bool operator += ( Method method ) const
  {
    method();
    return true;
  }

  static void wait() {}
  static void terminate() {}
  static void loop() {}
  static void init() {}
  static void destroy() {}

};

}

#endif
