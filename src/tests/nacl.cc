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

#include "oz/oz.hh"
#include "oz/windefs.h"
#include "ozmain/main.hh"

#include <../../../include/ppapi/cpp/instance.h>
#include <../../../include/ppapi/cpp/fullscreen.h>
#include "../../../include/ppapi/cpp/input_event.h"
#include "../../../include/ppapi/cpp/instance.h"
#include "../../../include/ppapi/cpp/core.h"
#include "../../../include/ppapi/cpp/module.h"
#include "../../../include/ppapi/cpp/completion_callback.h"
#include "../../../include/ppapi/cpp/graphics_2d.h"

#include <SDL/SDL.h>
#include <GLES2/gl2.h>

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
    System::core->CallOnMainThread( 0, CALLBACK_OBJECT( Main, null ) ); \
    mainSemaphore.wait(); \
  }

using namespace oz;

static Semaphore mainSemaphore;

int main( int, char** )
{
  System::init();

  mainSemaphore.init();

  MAIN_CALL( {
    System::instance->PostMessage( pp::Var( "drek" ) );
  } )

  Log::print( "[" ); Log::printTime(); Log::printEnd( "] START" );

  File::init( File::TEMPORARY, 1024 );

  File file( "/drek.txt" );

  char s[] = "Drek na palci";
  if( !file.write( s, String::length( s ) ) ) {
    printf( "ERROR\n" );
  }

  if( !file.map() ) {
    printf( "2ERROR\n" );
  }

  InputStream is = file.inputStream();
  while( is.isAvailable() ) {
    Log::printRaw( "%c", is.readChar() );
  }
  Log::printEnd();

  File::free();

  mainSemaphore.destroy();

  Log::print( "[" ); Log::printTime(); Log::printEnd( "] END" );
  return 0;
}
