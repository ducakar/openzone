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

#include "System.hh"
#include "String.hh"
#include "Semaphore.hh"
#include "Thread.hh"

#if defined( __native_client__ ) || defined( DOXYGEN_IGNORE )

/**
 * @def OZ_NACL_IS_MAIN
 * Check if on the main thread (debug mode only).
 */
#define OZ_NACL_IS_MAIN( boolean ) \
  hard_assert( Thread::isMain() == boolean )

/**
 * @def OZ_NACL_ENTRY_POINT
 * Implement `CreateModule()` entry point for a NaCl application.
 *
 * This macro has no effect for platforms other than NaCl.
 */
#define OZ_NACL_ENTRY_POINT() \
  namespace pp \
  { \
    pp::Module* CreateModule(); \
    pp::Module* CreateModule() { return oz::Pepper::createModule(); } \
  }

namespace pp
{

class Module;
class Instance;

}

/**
 * Main function (entry point) for NaCl applications.
 *
 * For a NaCl application, you must implement this function and put `OZ_NACL_ENTRY_POINT()` macro in
 * a `.cc` file (out of any namespace). It is run in a new thread named "naclMain". An empty string
 * is passed as the first argument (i.e. `argc = 1` and `argv = { "" }`).
 */
int naclMain( int argc, char** argv );

#else

#define OZ_NACL_IS_MAIN( boolean )
#define OZ_NACL_ENTRY_POINT()

#endif

namespace oz
{

#if defined( __native_client__ ) || defined( DOXYGEN_IGNORE )

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
private:

  /**
   * Internal `pp::Instance` implementation.
   */
  struct Instance;

  /**
   * Internal `pp::Module` implementation.
   */
  struct Module;

public:

  /**
   * PPAPI callback type.
   */
  typedef void Callback( void*, int );

  static int   width;    ///< Module area width.
  static int   height;   ///< Module area height.

  static float moveX;    ///< Mouse X axis.
  static float moveY;    ///< Mouse Y axis.
  static float moveZ;    ///< Mouse wheel (horizontal scroll).
  static float moveW;    ///< Mouse wheel (vertical scroll).

  static bool  hasFocus; ///< True iff focused and mouse is captured.

public:

  /**
   * Forbid instances.
   */
  Pepper() = delete;

  /**
   * True iff called on the module's main thread.
   */
  static bool isMainThread();

  /**
   * Execute asynchronous callback on the module's main thread.
   */
  static void mainCall( Callback* callback, void* data );

  /**
   * Return `pp::Instance` for a NaCl application or `nullptr` if not created.
   */
  static pp::Instance* instance();

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
   * Create PPAPI module instance.
   */
  static pp::Module* createModule();

};

#endif

/**
 * Utility for executing code blocks on the NaCl's main thread.
 *
 * Methods schedulled for the main thread should be lambda functions as those can (and often need
 * to) access local variables of a function on a non-main thread and are quick to implement.
 *
 * A typical scenario:
 * <code>
 * GLuint id = loadTexture( file );
 * MainCall() << [&]
 * {
 *   glBindTexture( GL_TEXTURE_2D, id );
 * };
 * </code>
 *
 * When used on the main thread or on a platform other than NaCl the method is executed immediately
 * on the caller thread.
 */
class MainCall
{
#if defined( __native_client__ ) || defined( DOXYGEN_IGNORE )

private:

  /**
   * Callback type.
   */
  typedef void Callback( void* data, int );

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

  static LocalSemaphore localSemaphore; ///< Semaphore for synchronous calls.

#endif

public:

  /**
   * Call a method on the NaCl main thread synchronously.
   *
   * The method can also be a lambda expression with captures.
   *
   * On platforms other that NaCl the code is executed immediately on the caller thread.
   */
  template <typename Method>
  void operator << ( Method method ) const
  {
#ifdef __native_client__

    if( Thread::isMain() ) {
      method();
    }
    else {
      struct CallbackWrapper
      {
        Method     method;
        Semaphore* semaphore;

        static void callback( void* data, int )
        {
          const CallbackWrapper* cw = static_cast<const CallbackWrapper*>( data );

          cw->method();
          cw->semaphore->post();
        }
      };
      CallbackWrapper cw = { method, &localSemaphore.sem };

      Pepper::mainCall( CallbackWrapper::callback, &cw );
      localSemaphore.sem.wait();
    }

#else

    method();

#endif
  }

  /**
   * Call a method on the NaCl main thread asynchronously.
   *
   * The method can be a lambda expression but captures are discouraged for asynchronous calls as
   * local variables may change till the function is executed or the local stack may not even exist
   * any more.
   *
   * On platforms other that NaCl the code is executed immediately on the caller thread.
   */
  template <typename Method>
  void operator += ( Method method ) const
  {
#ifdef __native_client__

    struct CallbackWrapper
    {
      Method method;

      static void callback( void* data, int )
      {
        const CallbackWrapper* cw = static_cast<const CallbackWrapper*>( data );

        cw->method();
      }
    };
    CallbackWrapper cw = { method };

    Pepper::mainCall( CallbackWrapper::callback, &cw );

#else

    method();

#endif
  }

};

}
