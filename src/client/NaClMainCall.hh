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
 * @file client/NaClMainCall.hh
 */

#pragma once

#include "client/common.hh"

#ifdef __native_client__

#define OZ_MAIN_CALL( this, code ) \
  { \
    typedef decltype( this ) _This; \
    struct _Callback { \
      static void _main( void* data, int ) \
      { \
        _This _this = static_cast<_This>( data ); \
        static_cast<void>( _this ); \
        { code } \
        oz::client::NaClMainCall::semaphore.post(); \
      } \
    }; \
    if( oz::client::NaClMainCall::isMainThread() ) { \
      _Callback::_main( this, 0 ); \
    } \
    else { \
      oz::client::NaClMainCall::call( _Callback::_main, this ); \
    } \
  }

namespace oz
{
namespace client
{

class NaClMainCall
{
  public:

    typedef void Callback( void*, int );

    static Semaphore semaphore;

    NaClMainCall() = delete;

    static bool isMainThread();
    static void call( Callback* callback, void* caller );

    static void init();
    static void free();

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
