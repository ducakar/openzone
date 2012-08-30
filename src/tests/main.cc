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

#include <main.hh>

#ifdef __native_client__

#include <client/Window.hh>

#include <ppapi/gles2/gl2ext_ppapi.h>
#include <GLES2/gl2.h>

using oz::client::window;

namespace oz
{

void* MainInstance::mainThreadMain( void* )
{
  ozMain( 0, nullptr );
  return null;
}

MainInstance::MainInstance( PP_Instance instance_ ) :
  pp::Instance( instance_ ), pp::MouseLock( this ), fullscreen( this ), isContextBound( false ),
  isMouseLocked( false ), mainThread( 0 )
{
  System::module   = pp::Module::Get();
  System::instance = this;
  System::core     = pp::Module::Get()->core();

  RequestInputEvents( PP_INPUTEVENT_CLASS_KEYBOARD | PP_INPUTEVENT_CLASS_MOUSE );
}

MainInstance::~MainInstance()
{
  if( mainThread != 0 ) {
    pthread_join( mainThread, nullptr );
    mainThread = nullptr;
  }
}

bool MainInstance::Init( uint32_t, const char**, const char** )
{
  return true;
}

// static void flushCallback( void*, int )
// {}

void MainInstance::DidChangeView( const pp::View& )
{
//   int width  = view.GetRect().width();
//   int height = view.GetRect().height();
//
//   if( width == window.width && height == window.height && isContextBound ) {
//     return;
//   }
//
//   window.width  = width;
//   window.height = height;
//
//   if( !glInitializePPAPI( System::module->get_browser_interface() ) ) {
//     OZ_ERROR( "Failed to initialize PPAPI GLES2" );
//   }
//
//   int attribs[] = {
//     PP_GRAPHICS3DATTRIB_ALPHA_SIZE, 8,
//     PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 24,
//     PP_GRAPHICS3DATTRIB_STENCIL_SIZE, 8,
//     PP_GRAPHICS3DATTRIB_SAMPLES, 0,
//     PP_GRAPHICS3DATTRIB_SAMPLE_BUFFERS, 0,
//     PP_GRAPHICS3DATTRIB_WIDTH, window.width,
//     PP_GRAPHICS3DATTRIB_HEIGHT, window.height,
//     PP_GRAPHICS3DATTRIB_NONE
//   };
//
//   Log::println( "aa\n" );
//
//   context = pp::Graphics3D( System::instance, pp::Graphics3D(), attribs );
//   if( context.is_null() ) {
//     OZ_ERROR( "Graphics3D surface creation failed" );
//   }
//   isContextBound = BindGraphics( context );
//
//   glSetCurrentContextPPAPI( context.pp_resource() );
//
//   Log::println( "bb\n" );
//
//   glClearColor( 1.0f, 0.0f, 1.0f, 1.0f );
//   glClear( GL_COLOR_BUFFER_BIT );
//
//   Log::println( "cc\n" );
//
//   glFlush();
//
//   Log::println( "dd\n" );
//
//   context.SwapBuffers( pp::CompletionCallback( &flushCallback, nullptr ) );
//
//   Log::println( "ee\n" );

  if( mainThread == 0 ) {
    pthread_create( &mainThread, nullptr, mainThreadMain, this );
  }
}

void MainInstance::DidChangeView( const pp::Rect&, const pp::Rect& )
{
  PP_NOTREACHED();
}

bool MainInstance::HandleInputEvent( const pp::InputEvent& event )
{
  switch( event.GetType() ) {
    case PP_INPUTEVENT_TYPE_MOUSEDOWN: {
      if( !isMouseLocked && fullscreen.IsFullscreen() ) {
        LockMouse( pp::CompletionCallback( &DidMouseLock, this ) );
      }
      break;
    }
    case PP_INPUTEVENT_TYPE_KEYDOWN: {
      pp::KeyboardInputEvent keyEvent( event );

      if( ( keyEvent.GetKeyCode() == 122 || keyEvent.GetKeyCode() == 13 ) &&
          ( event.GetModifiers() & PP_INPUTEVENT_MODIFIER_ALTKEY ) )
      {
        isContextBound = false;

        if( fullscreen.IsFullscreen() ) {
          fullscreen.SetFullscreen( false );
        }
        else if( fullscreen.SetFullscreen( true ) && !isMouseLocked ) {
          LockMouse( pp::CompletionCallback( &DidMouseLock, this ) );
        }
      }
      break;
    }
    default: {
      break;
    }
  }

//   SDL_NACL_PushEvent( event );
  return true;
}

void MainInstance::MouseLockLost()
{
  isMouseLocked = false;
}

void MainInstance::Empty( void*, int )
{}

void MainInstance::DidMouseLock( void* data, int result )
{
  MainInstance* instance = static_cast<MainInstance*>( data );
  instance->isMouseLocked = result == PP_OK;
}

pp::Instance* MainModule::CreateInstance( PP_Instance instance )
{
  return new MainInstance( instance );
}

}

namespace pp
{

pp::Module* CreateModule()
{
  return new oz::MainModule();
}

}

#endif
