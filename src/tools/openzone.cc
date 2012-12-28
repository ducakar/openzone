/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file client/openzone.cc
 */

#include <stable.hh>
#include <client/openzone.hh>

#include <client/Client.hh>
#include <client/NaClPlatform.hh>

#include <SDL.h>

#if defined( __ANDROID__ )
# include <jni.h>
extern "C" void SDL_Android_Init( JNIEnv* env, jclass clazz );
extern "C" void Java_com_openzone_SDLActivity_nativeInit( JNIEnv* env, jclass clazz );
#elif defined( __native_client__ )
# include <SDL_nacl.h>
#elif defined( _WIN32 )
extern "C" int SDL_main( int argc, char **argv );
#endif

using namespace oz;
using namespace oz::client;

#if defined( __ANDROID__ )
void Java_com_openzone_SDLActivity_nativeInit( JNIEnv* env, jclass clazz )
#elif defined( __native_client__ )
void MainInstance::mainThreadMain( void* instance )
#elif defined( _WIN32 )
int SDL_main( int argc, char** argv )
#else
int main( int argc, char** argv )
#endif
{
#if defined( __ANDROID__ )
  JavaVM* javaVM;
  env->GetJavaVM( &javaVM );
  System::javaVM = javaVM;
#elif defined( __native_client__ )
  System::instance = static_cast<pp::Instance*>( instance );
#endif

  System::init( System::DEFAULT_MASK, SDL_Quit );

  int exitCode = EXIT_FAILURE;

  Log::printRaw( "OpenZone " OZ_VERSION "\n"
                 "Copyright © 2002-2013 Davorin Učakar\n"
                 "This program comes with ABSOLUTELY NO WARRANTY.\n"
                 "This is free software, and you are welcome to redistribute it\n"
                 "under certain conditions; See COPYING file for details.\n\n" );

#if defined( __ANDROID__ )
  SDL_Android_Init( env, clazz );
#elif defined( __native_client__ )
  NaClPlatform::post( "init:" );
#endif

#if defined( __ANDROID__ ) || defined( __native_client__ )
  int   argc    = 1;
  char  argv0[] = "openzone";
  char* argv[]  = { argv0, nullptr };
#endif

  exitCode = client::client.init( argc, argv );

  if( exitCode == EXIT_SUCCESS ) {
    exitCode = client::client.main();
  }

  client::client.shutdown();

  if( Alloc::count != 0 ) {
    Log::verboseMode = true;
    bool isOutput = Alloc::printLeaks();
    Log::verboseMode = false;

    if( isOutput ) {
      Log::println( "There are some memory leaks. See '%s' for details.", Log::logFile() );
    }
  }

#if defined( __ANDROID__ )
  static_cast<void>( exitCode );
#elif defined( __native_client__ )
  NaClPlatform::post( "quit:" );
#else
  return exitCode;
#endif
}

#ifdef __native_client__

MainInstance::MainInstance( PP_Instance instance_ ) :
  pp::Instance( instance_ ), pp::MouseLock( this ), fullscreen( this )
{
  NaClPlatform::init();

  RequestInputEvents( PP_INPUTEVENT_CLASS_KEYBOARD | PP_INPUTEVENT_CLASS_MOUSE |
                      PP_INPUTEVENT_CLASS_WHEEL );
}

MainInstance::~MainInstance()
{
  if( mainThread.isValid() ) {
    mainThread.join();
  }

  NaClPlatform::destroy();
}

bool MainInstance::Init( uint32_t, const char**, const char** )
{
  return true;
}

void MainInstance::DidChangeView( const pp::View& view )
{
  int width  = view.GetRect().width();
  int height = view.GetRect().height();

  if( width == NaClPlatform::width && height == NaClPlatform::height ) {
    return;
  }

  NaClPlatform::width  = width;
  NaClPlatform::height = height;

  if( !mainThread.isValid() ) {
    SDL_NACL_SetInstance( pp_instance(), NaClPlatform::width, NaClPlatform::height );

    mainThread.start( "main", Thread::JOINABLE, mainThreadMain, this );
  }
}

void MainInstance::DidChangeView( const pp::Rect&, const pp::Rect& )
{
  PP_NOTREACHED();
}

void MainInstance::HandleMessage( const pp::Var& message )
{
  NaClPlatform::push( message.AsString().c_str() );
}

bool MainInstance::HandleInputEvent( const pp::InputEvent& event )
{
  switch( event.GetType() ) {
    case PP_INPUTEVENT_TYPE_MOUSEDOWN: {
      if( !NaClPlatform::hasFocus ) {
        LockMouse( pp::CompletionCallback( &onMouseLocked, this ) );
        return true;
      }
      break;
    }
    case PP_INPUTEVENT_TYPE_MOUSEMOVE: {
      pp::MouseInputEvent mouseEvent( event );
      pp::Point move = mouseEvent.GetMovement();

      NaClPlatform::moveX += float( move.x() );
      NaClPlatform::moveY += float( move.y() );
      break;
    }
    case PP_INPUTEVENT_TYPE_WHEEL: {
      pp::WheelInputEvent wheelEvent( event );
      pp::FloatPoint move = wheelEvent.GetDelta();

      NaClPlatform::moveZ += move.x();
      NaClPlatform::moveW += move.y();
      break;
    }
    case PP_INPUTEVENT_TYPE_KEYDOWN: {
      pp::KeyboardInputEvent keyEvent( event );

      if( ( keyEvent.GetKeyCode() == 122 || keyEvent.GetKeyCode() == 13 ) &&
          event.GetModifiers() == 0 )
      {
        fullscreen.SetFullscreen( !fullscreen.IsFullscreen() );
        return true;
      }
      break;
    }
    default: {
      break;
    }
  }

  SDL_NACL_PushEvent( event );
  return true;
}

void MainInstance::MouseLockLost()
{
  NaClPlatform::hasFocus = false;
  fullscreen.SetFullscreen( false );
}

void MainInstance::onMouseLocked( void*, int result )
{
  NaClPlatform::hasFocus = result == PP_OK;
}

pp::Instance* MainModule::CreateInstance( PP_Instance instance )
{
  return new MainInstance( instance );
}

namespace pp
{

pp::Module* CreateModule()
{
  return new MainModule();
}

}

#endif
