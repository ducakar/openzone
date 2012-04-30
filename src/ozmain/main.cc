/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file ozmain/main.cc
 *
 * Platform-specific main function implementation.
 */

#include "main.hh"

#ifdef __native_client__

#include <SDL/SDL.h>

namespace oz
{

void* MainInstance::mainThreadMain( void* )
{
  ozMain( 0, null );
  return null;
}

MainInstance::MainInstance( PP_Instance instance ) :
  pp::Instance( instance ), mainThread( 0 ), width( 0 ), height( 0 )
{
  System::setInstance( this );
}

MainInstance::~MainInstance()
{
  if( mainThread != 0 ) {
    pthread_join( mainThread, null );
    SDL_Quit();
  }
}

void MainInstance::DidChangeView( const pp::View& view )
{
  width  = view.GetRect().width();
  height = view.GetRect().height();

  if( mainThread == 0 ) {
    SDL_Init( SDL_INIT_NOPARACHUTE | SDL_INIT_VIDEO );
    pthread_create( &mainThread, null, mainThreadMain, this );
  }
}

void MainInstance::DidChangeView( const pp::Rect& position, const pp::Rect& )
{
  width  = position.width();
  height = position.height();

  if( mainThread == 0 ) {
    SDL_Init( SDL_INIT_NOPARACHUTE | SDL_INIT_VIDEO );
    pthread_create( &mainThread, null, mainThreadMain, this );
  }
}

bool MainInstance::Init( uint32_t, const char*[], const char*[] )
{
  return true;
}

bool MainInstance::HandleInputEvent( const pp::InputEvent& )
{
  return true;
}

pp::Instance* MainModule::CreateInstance( PP_Instance instance )
{
  return new MainInstance( instance );
}

}

namespace pp
{

Module* CreateModule()
{
  return new oz::MainModule();
}

}

#else

int main( int argc, char** argv )
{
  return ozMain( argc, argv );
}

#endif
