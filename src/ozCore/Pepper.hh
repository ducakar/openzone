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

#if defined( __native_client__ ) || defined( DOXYGEN_IGNORE )

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
 * is passed as argument zero (i.e. `argc = 1` and `argv = { "" }`).
 */
int naclMain( int argc, char** argv );

#else

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
  class Instance;

  /**
   * Internal `pp::Module` implementation.
   */
  class Module;

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

}
