/*
 * ozEngine - OpenZone Engine Library.
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
 * @file ozEngine/Pepper.hh
 *
 * `Pepper` class.
 */

#pragma once

#include "common.hh"

namespace oz
{

#if defined( __native_client__ ) || defined( DOXYGEN_IGNORE )

/**
 * High-level interface to NaCl %Pepper API (PPAPI).
 *
 * It provides facilities for invoking code on the main thread, passing messages between application
 * and JavaScript on the web page and feedback information about module's geometry and mouse
 * movement since the latter is not implemented in NaCl SDL port.
 */
class Pepper
{
  public:

    /**
     * PPAPI callback type.
     */
    typedef void Callback( void*, int );

    static Semaphore mainCallSemaphore; ///< Semaphore for use in conjunction with `OZ_MAIN_CALL`
                                        ///< and `OZ_STATIC_MAIN_CALL` macros.

    static int       width;             ///< Module area width.
    static int       height;            ///< Module area height.

    static float     moveX;             ///< Mouse X axis.
    static float     moveY;             ///< Mouse Y axis.
    static float     moveZ;             ///< Mouse wheel (horizontal scroll).
    static float     moveW;             ///< Mouse wheel (vertical scroll).

    static bool      hasFocus;          ///< True iff focused and mouse is captured.

    /**
     * Forbid instances.
     */
    explicit Pepper() = delete;

    /**
     * True iff called on the module's main thread.
     */
    static bool isMainThread();

    /**
     * Execute asynchronous callback on the module's main thread.
     */
    static void mainCall( Callback* callback, void* data );

    /**
     * Post a message to JavaScript running on the page.
     */
    static void post( const char* message );

    /**
     * Pop next message from the incoming messages queue.
     */
    static String pop();

    /**
     * Push message to the incoming messages queue.
     */
    static void push( const char* message );

    /**
     * Initialise `Pepper` class.
     */
    static void init();

    /**
     * Deinitialise `Pepper` class.
     */
    static void destroy();

};

/**
 * Utility for performing synchronous calls on the main NaCl thread.
 *
 * Methods called on the main thread would typically be lambda functions as those often need to
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
 * This mechanism also works if used from the main thread.
 *
 * On platforms other than NaCl the lambda function is called immediately.
 */
struct MainCall
{
  /**
   * Call a method without parameters on the main thread and wait for it to finish.
   */
  template <typename Method>
  void operator << ( Method method ) const
  {
    if( Pepper::isMainThread() ) {
      method();
    }
    else {
      struct CallbackWrapper
      {
        Method method;

        static void callback( void* data, int )
        {
          const CallbackWrapper* cw = static_cast<const CallbackWrapper*>( data );

          cw->method();
          Pepper::mainCallSemaphore.post();
        }
      };
      CallbackWrapper callbackWrapper = { method };

      Pepper::mainCall( CallbackWrapper::callback, &callbackWrapper );
      Pepper::mainCallSemaphore.wait();
    }
  }
};

#else

struct MainCall
{
  template <typename Method>
  void operator << ( Method method ) const
  {
    method();
  }
};

#endif

}
