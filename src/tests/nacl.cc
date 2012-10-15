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
 * @file tests/nacl.cc
 */

#include <oz/oz.hh>
#include "main.hh"

#include <ppapi/cpp/fullscreen.h>
#include <ppapi/cpp/input_event.h>
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/core.h>
#include <ppapi/cpp/module.h>
#include <ppapi/cpp/completion_callback.h>
#include <ppapi/cpp/graphics_2d.h>

#include <SDL/SDL.h>
#include <GLES2/gl2.h>
#include <physfs.h>

#define DEFINE_CALLBACK( name, code ) \
  struct _Callback##name { \
    static void _main##name( void* _data, int _result ) \
    { \
      FileDesc* _fd = static_cast<FileDesc*>( _data ); \
      static_cast<void>( _fd ); \
      static_cast<void>( _result ); \
      code \
    } \
  };

#define CALLBACK_OBJECT( name, arg ) \
  pp::CompletionCallback( _Callback##name::_main##name, arg )

#define MAIN_CALL( code ) \
  { \
    DEFINE_CALLBACK( Main, code mainSemaphore.post(); ) \
    System::core->CallOnMainThread( 0, CALLBACK_OBJECT( Main, nullptr ) ); \
    mainSemaphore.wait(); \
  }

using namespace oz;

static Semaphore mainSemaphore;

int main( int, char** )
{
  System::init();
  Log() << "[" << Time::local().toString() << "] START\n";

  File::init( File::PERSISTENT, 1024 );

//   char content[] = "Drek na palci";

//   File( "/drek.txt" ).write( content, sizeof( content ) );
  Buffer b = File( "/drek.txt" ).read();

  Log::println( "%s", b.begin() );

  File::free();

  static_assert( sizeof( uint64_t ) == 8, "error" );

  Log() << "[" << Time::local().toString() << "] END\n";
  return 0;
}
