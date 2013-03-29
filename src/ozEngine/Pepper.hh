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

/**
 * @def OZ_MAIN_CALL
 * On NaCl, execute the given code in a callback on the main thread and block until finished.
 *
 * This version passes `this` pointer of the current object to the callback as `_this` variable.
 * For platforms other than NaCl the code is just inserted at the macro position.
 */
#ifdef __native_client__

# define OZ_MAIN_CALL( this, code ) \
  if( oz::Pepper::isMainThread() ) { \
    typedef decltype( this ) _This; \
    _This _this = ( this ); \
    static_cast<void>( _this ); \
    { code } \
  } \
  else { \
    typedef decltype( this ) _This; \
    struct _Callback \
    { \
      static void _main( void* data, int ) \
      { \
        _This _this = static_cast<_This>( data ); \
        static_cast<void>( _this ); \
        { code } \
        oz::Pepper::mainCallSemaphore.post(); \
      } \
    }; \
    oz::Pepper::mainCall( _Callback::_main, ( this ) ); \
  }

#else

# define OZ_MAIN_CALL( this, code ) \
  { \
    decltype( this ) _this = ( this ); \
    static_cast<void>( _this ); \
    { code } \
  }

#endif

/**
 * @def OZ_STATIC_MAIN_CALL
 * On NaCl, execute the given code in a callback on the main thread and block until finished.
 *
 * This version does not pass this pointer or any data to the callback.
 * For platforms other than NaCl the code is just inserted at the macro position.
 */
#ifdef __native_client__

# define OZ_STATIC_MAIN_CALL( code ) \
  if( oz::Pepper::isMainThread() ) { \
    { code } \
  } \
  else { \
    struct _Callback \
    { \
      static void _main( void*, int ) \
      { \
        { code } \
        oz::Pepper::mainCallSemaphore.post(); \
      } \
    }; \
    oz::Pepper::mainCall( _Callback::_main, nullptr ); \
  }

#else

# define OZ_STATIC_MAIN_CALL( code ) \
  { code }

#endif

#if defined( __native_client__ ) || defined( DOXYGEN_IGNORE )

namespace oz
{

/**
 * High-level interface to NaCl's Pepper API (PPAPI).
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

    static Semaphore mainCallSemaphore; ///< Semaphore primarily meant for use in conjunction with
                                        ///< `OZ_MAIN_CALL` and `OZ_STATIC_MAIN_CALL` macros.

    static int       width;             ///< Module area width.
    static int       height;            ///< Module area height.

    static float     moveX;             ///< Mouse X axis.
    static float     moveY;             ///< Mouse Y axis.
    static float     moveZ;             ///< Mouse wheel (horizontal scroll).
    static float     moveW;             ///< Mouse wheel (vertical scroll).

    static bool      hasFocus;          ///< True iff fullscreen and mouse captured.

    /**
     * Forbid instances.
     */
    explicit Pepper() = delete;

    /**
     * True iff called on the module's main thread.
     */
    static bool isMainThread();

    /**
     * Call execute callback on the module's main thread.
     */
    static void mainCall( Callback* callback, void* caller );

    /**
     * Post a message to JavaScript running on the page.
     */
    static void post( const char* message );

    /**
     * Pop next incoming message from the queue.
     */
    static String poll();

    /**
     * Push message back to the queue of incoming messages.
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

}

#endif
