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

#include <SDL/SDL.h>
#include <GLES2/gl2.h>

#include <ppapi/cpp/completion_callback.h>
#include <ppapi/cpp/core.h>
#include <ppapi/cpp/file_system.h>
#include <ppapi/cpp/file_ref.h>
#include <ppapi/cpp/file_io.h>

#define DEFINE_CALLBACK( name, code ) \
  struct Callback##name { \
    static void main##name( void* data, int ) { static_cast<void>( data ); { code } } \
  };

#define CALLBACK_OBJECT( name, arg ) \
  pp::CompletionCallback( Callback##name::main##name, arg )

#define MAIN_CALL( core, barrier, code ) \
  { \
    DEFINE_CALLBACK( Main, { { code } barrier.finish(); } ) \
    barrier.begin(); \
    core->CallOnMainThread( 0, CALLBACK_OBJECT( Main, oz::null ) ); \
    barrier.wait(); \
  }

using namespace oz;

static Barrier main;
static Barrier aux;

int ozMain( int, char** )
{
  System::init();

  main.init();
  aux.init();

  static pp::Core*       core    = pp::Module::Get()->core();
  static pp::FileSystem* fs      = null;
  static pp::FileRef*    fileRef = null;

  MAIN_CALL( core, main, {
    SDL_Init( SDL_INIT_NOPARACHUTE | SDL_INIT_VIDEO );
  } )

  DEFINE_CALLBACK( OpenFS, {
    log.println( "OpenFS" );
    aux.finish();
  } )

  aux.begin();

  MAIN_CALL( core, main, {
    pp::Instance* instance = static_cast<pp::Instance*>( System::instance() );

    fs      = new pp::FileSystem( instance, PP_FILESYSTEMTYPE_LOCALPERSISTENT );
    fileRef = new pp::FileRef( *fs, "/drek.txt" );

    int ret = fs->Open( 1024, CALLBACK_OBJECT( OpenFS, null ) );
    if( ret != PP_OK_COMPLETIONPENDING ) {
      log.println( "ERRROR" );
      aux.finish();
    }
  } )

  aux.wait();

  MAIN_CALL( core, main, {
    delete fileRef;
    delete fs;
  } )
  MAIN_CALL( core, main, {
    SDL_Quit();
  } )

  aux.free();
  main.free();
  return 0;
}
