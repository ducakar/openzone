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
 */

#pragma once

#include "common.hh"

#ifdef __native_client__

#define OZ_MAIN_CALL( t, code ) \
  if( oz::Pepper::isMainThread() ) { \
    decltype( t ) _this = ( t ); \
    static_cast<void>( _this ); \
    { code } \
  } \
  else { \
    typedef decltype( t ) _This; \
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
    oz::Pepper::call( _Callback::_main, ( t ) ); \
  }

#define OZ_STATIC_MAIN_CALL( code ) \
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
    oz::Pepper::call( _Callback::_main, nullptr ); \
  }

namespace oz
{

class Pepper
{
  public:

    typedef void Callback( void*, int );

    static Semaphore mainCallSemaphore;

    static int       width;    ///< Module area width.
    static int       height;   ///< Module area height.

    static float     moveX;    ///< Mouse X axis.
    static float     moveY;    ///< Mouse Y axis.
    static float     moveZ;    ///< Mouse wheel (horizontal scroll).
    static float     moveW;    ///< Mouse wheel (vertical scroll).

    static bool      hasFocus; ///< True iff fullscreen and mouse captured.

    explicit Pepper() = delete;

    /*
     * Main thread call.
     */
    static bool isMainThread();
    static void call( Callback* callback, void* caller );

    /*
     * JavaScript messages.
     */
    static void post( const char* message );
    static String poll();
    static void push( const char* message );

    /*
     * General
     */
    static void init();
    static void destroy();

};

}

#else

#define OZ_MAIN_CALL( this, code ) \
  { \
    decltype( this ) _this = ( this ); \
    static_cast<void>( _this ); \
    { code } \
  }

#define OZ_STATIC_MAIN_CALL( code ) \
  { code } \

#endif
