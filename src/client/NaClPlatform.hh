/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file client/NaClPlatform.hh
 */

#pragma once

#include <client/common.hh>

#ifdef __native_client__

#define OZ_MAIN_CALL( t, code ) \
  if( oz::client::NaClPlatform::isMainThread() ) { \
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
        oz::client::NaClPlatform::mainCallSemaphore.post(); \
      } \
    }; \
    oz::client::NaClPlatform::call( _Callback::_main, ( t ) ); \
  }

namespace oz
{
namespace client
{

class NaClPlatform
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

    explicit NaClPlatform() = delete;

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
}

#else

#define OZ_MAIN_CALL( this, code ) \
  { \
    decltype( this ) _this = ( this ); \
    static_cast<void>( _this ); \
    { code } \
  }

#endif
